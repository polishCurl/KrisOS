/*******************************************************************************
* File:     	light_sensor.c
* Brief:    	Illumination level monitor driver
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	20/02/2017
* Last mod: 	21W/02/2017
*
* Note: 
*******************************************************************************/
#include "KrisOS.h"



/*-------------------------------------------------------------------------------
* Semaphore for synchronisation between the digital comparator interrupt handler
* and the task for reacting to
--------------------------------------------------------------------------------*/
Semaphore lightSensorSem;



/*-------------------------------------------------------------------------------
* Function:    	light_sensor_init
* Purpose:    	Initialise illumination level monitor. Set the ADC to conitnuously 
*				monitor the voltage level from the photoresistor and trigger an 
*				interrupt if the illumionation level is too high.
* Arguments:	
*		threshold - maximum illumination level threshold
* Returns: 		-	
--------------------------------------------------------------------------------*/
void light_sensor_init(uint32_t threshold) {
	
	// Check if the threshold is valid (12-bit)
	if (threshold & ~0xFFF)
		return;
	
	// Activate the ADC0 module and enable clock on port B. Add a small delay.
	SYSCTL->RCGCADC |= 1 <<	RCGC_ADC0;
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTB); 
	while ((SYSCTL->RCGCGPIO & (1 << RCGCGPIO_PORTB)) == 0);
	
	// Configure PB4 as analog input
	GPIOB->AFSEL |= 1 << PIN4;
	GPIOB->DEN &= ~(1 << PIN4);
	GPIOB->AMSEL |= 1 << PIN4;

	// Disable Sample Sequencer 3 for the time of configuration. SS3 has FIFO of
	// length 1 and on default it has the lowest priority of all sequencers.
	ADC0->ACTSS &= ~(1 << ACTSS_ASEN3);
	
	// Enable hardware averaging of 64 samples at the cost of ADC throughput.
	// This gives more stable results
	ADC0->SAC = 6;
	
	// Configure the digital comparator: set high band interrupt condition, 
	// Enable comparator interrupts and generate interrupts only once when the 
	// high-band region is entered.
	ADC0->DCCTL0 |= 0x3 << DCCTL0_CIC;
	ADC0->DCCTL0 |= 0x3 << DCCTL0_CIM;
	ADC0->DCCTL0 |= 1 << DCCTL0_CIE;
	
	// Set the digital comparator threshold value
	ADC0->DCCMP0 = 0;   
	ADC0->DCCMP0 |= ((threshold - 500) << DCCMP0_COMP0) | (threshold << DCCMP0_COMP1); 
	
	// Set continuous sampling on SS3 and specify that AIN10 is used for 
	// analog-to-digital conversion
	ADC0->EMUX |= 0xF << EMUX_EM3;
	ADC0->SSMUX3 = 10;
	
	// Disable temperature sensor reading, single sample is always the end of 
	// a sequence. Enable interrupt generation by the sample and disable
	// differential input
	ADC0->SSCTL3 = 0;
	ADC0->SSCTL3 |= (1 << SSCTL3_END0) | (1 << SSCTL3_IE0);
	
	// Send samples from SS3 to digital comparator 0
	ADC0->SSDC3 = 0;
	ADC0->SSOP3 = 1;
	
	// Disable raw interrupts, but enable the digital comparator on SS3.
	// eenable the sequencer
	ADC0->IM &= ~(1 << IM_MASK3);
	ADC0->IM |= (1 << IM_DCONSS3);
	ADC0->ACTSS |= 1 << ACTSS_ASEN3;
	
	// Register the SS3 irqs at NVIC
	KrisOS_irq_set_prio(ADC0SS3_IRQn, 1);
	KrisOS_irq_enable(ADC0SS3_IRQn);	
}



/*-------------------------------------------------------------------------------
* Function:    	buzzer_init
* Purpose:    	Initialise the piezo buzzer at GPIO PB1 for generating beeps 
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void buzzer_init(void) {
	
	// Enable clock on port B. Add a small delay.
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTB); 
	while ((SYSCTL->RCGCGPIO & (1 << RCGCGPIO_PORTB)) == 0);
	
	// Configure PB1 as digital GPIO output for driving the piezo buzzer
	GPIOB->DIR |= 1 << PIN1;
	GPIOB->AFSEL &= ~(1 << PIN1);
	GPIOB->DEN |= 1 << PIN1;
	GPIOB->AMSEL &= ~(1 << PIN1);

}



/*-------------------------------------------------------------------------------
* Function:    	ADC0SS3_Handler
* Purpose:    	IRQ handler which deals with illumination level exceeding allowed
*				range
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void ADC0SS3_Handler(void) {
	
	// Clear the interrupt signal
	ADC0->DCISC |= 1 << DCISC_DCINT0;
	ADC0->ISC |= 1 << ISC_DCINSS3;
	
	// Notify the task responsible for handling the excessive light energy amount
	KrisOS_sem_release_from_ISR(&lightSensorSem);
}


