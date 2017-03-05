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
#include "light_sensor.h"
#include "nokia5110.h"



/*-------------------------------------------------------------------------------
* Semaphore for synchronisation between the digital comparator interrupt handler
* and the task for reacting to
--------------------------------------------------------------------------------*/
Semaphore lightSensorSem;



/*******************************************************************************
* Task: 	lightSensor
* Purpose: 	The illumination level monitor task
*******************************************************************************/
void lightSensor(void) {
	
	// Helper cursor for drawing horizontal bars of the warining message
	uint32_t cursor;
	
	// Initialise the semaphore for interrupt-task synchronisation
	KrisOS_sem_init(&lightSensorSem, 0);
	
	// Initialise the ADC module with digital comparator interrupts when the input
	// value exceeds given threshold
	light_sensor_init(3000);
	
	// Initialise the buzzer
	buzzer_init();

	while(1) {
		// Wait on the semaphore - wait untill ADC0SS3 IRQ is generated which 
		// releases this semaphore
		KrisOS_sem_acquire(&lightSensorSem);
		
		// Output the warning message
		KrisOS_mutex_lock(nokiaMtx);
		nokia5110_set_cursor(0, 1);
		for (cursor = 0; cursor < NOKIA5110_WIDTH; cursor++)
			nokia5110_send(DATA, 0x20);
		fprintf(&nokia5110, "TOO MUCH LIGHT");
		for (cursor = 0; cursor < NOKIA5110_WIDTH; cursor++)
			nokia5110_send(DATA, 0x02);
		KrisOS_mutex_unlock(nokiaMtx);
		
		// Generate the 'beep' alert
		buzzer_tone(700);
		KrisOS_task_sleep(1000);
		buzzer_tone(500);
		KrisOS_task_sleep(1000);
		buzzer_tone(1000);
		KrisOS_task_sleep(1000);
		buzzer_off();
		
		// Clear the warning message
		KrisOS_task_sleep(5000);
		KrisOS_mutex_lock(nokiaMtx);
		nokia5110_set_cursor(0, 1);
		for (cursor = 0; cursor < 3 * NOKIA5110_WIDTH; cursor++)
			nokia5110_send(DATA, 0x0);
		KrisOS_mutex_unlock(nokiaMtx);
	}
}



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
	ADC0->DCCMP0 |= ((threshold - 1000) << DCCMP0_COMP0) | (threshold << DCCMP0_COMP1); 
	
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
* Purpose:    	Initialise the piezo buzzer at GPIO PB6 for generating beeps using
*				Pulse Width Modulator.
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void buzzer_init(void) {
	
	// Activate PWM Module 0 and enable clock on port B. Add a small delay.
	SYSCTL->RCGCPWM |= 1 <<	RCGC_PWM0;	
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTB); 
	while ((SYSCTL->RCGCGPIO & (1 << RCGCGPIO_PORTB)) == 0);
	
	// Configure PB6 as M0PWM0 output
	GPIOB->AFSEL |= 1 << PIN6;
	GPIOB->PCTL &= ~(0xF << PCTL_PMC6);
	GPIOB->PCTL |= 0x4 << PCTL_PMC6;
	GPIOB->AMSEL &= ~(1 << PIN6);
	GPIOB->DEN |= 1 << PIN6;
	GPIOB->DIR |= 1 << PIN6;

	// Set 500Hz frequency. configure the PWM generator for immediate updates and 
	// countdown mode. High on comparator down and low on load
	PWM0->_0_CTL = 0;
	PWM0->_0_GENA = (0x2 << PWMGEN_ACTLOAD) | (0x3 << PWMGEN_ACTCMPAD);
	
	// Enable PWM on PB6
	PWM0->_0_CTL |= 1 << PWMCTL_ENABLE;
}



/*-------------------------------------------------------------------------------
* Function:    	buzzer_tone
* Purpose:    	Beep the buzzer. Emit a sound of given frequency for the duration
*				of time specified.
* Arguments:	
*		frequency - output sound frequency in Hz
*		duration - beep sound duration in OS 'ticks'
* Returns: 		-	
--------------------------------------------------------------------------------*/
void buzzer_tone(uint32_t frequency) {
	
	// Set the PWM clock to be derived from the system clock with division by 64
	SYSCTL->RCC |= 0x7 << RCC_PWMDIV;
	SYSCTL->RCC |= 1 << RCC_USEPWMDIV;
	
	// Temporarily disable the PWM output on the GPIO pin
	PWM0->ENABLE &= ~(1 << PWMENABLE_PWM0EN);	
	
	// Set the PWM frequency and 50% duty cycle
	PWM0->_0_LOAD = (SYSTEM_CLOCK_FREQ / 64 / frequency) - 1;
	PWM0->_0_CMPA = (SYSTEM_CLOCK_FREQ / 64 / frequency / 2) - 1;
	
	// Output the sound
	PWM0->ENABLE |= 1 << PWMENABLE_PWM0EN;	
}



/*-------------------------------------------------------------------------------
* Function:    	buzzer_off
* Purpose:    	Switch off the buzzer
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void buzzer_off(void) {
	PWM0->ENABLE &= ~(1 << PWMENABLE_PWM0EN);
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
	KrisOS_sem_release_ISR(&lightSensorSem);
}


