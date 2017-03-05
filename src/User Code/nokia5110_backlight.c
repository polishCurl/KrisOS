/*******************************************************************************
* File:     	nokia5110_backlight.c
* Brief:    	Code for controlling the LCD background light in the nokia 5110
*				screen
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	25/02/2017
* Last mod: 	25/02/2017
*
* Note: 
*******************************************************************************/
#include "KrisOS.h"
#include "nokia5110_backlight.h"



/*-----------------------------------------------------------------------------
* LIGHT pin address
------------------------------------------------------------------------------*/
#define LIGHT (*((volatile uint32_t*) 0x40005004))
	


/*-------------------------------------------------------------------------------
* Semaphore to be used between the GPIOF IRQ handler and nokiaLCDBacklight task
* for controling the operation of nokia 5110 backlight
--------------------------------------------------------------------------------*/
Semaphore* backlightSem;



/*******************************************************************************
* Task: 	nokiaLCDBacklight
* Purpose: 	Task for 
*******************************************************************************/
void nokiaLCDBacklight(void) {
	
	uint32_t lightIsOff = 1;
	
	backlightSem = KrisOS_sem_create(0);
	nokia5110_backlight_switch_init();
	nokia5110_backlight_control_pin_init();
	
	while(1) {
		LIGHT = lightIsOff;
		KrisOS_sem_acquire(backlightSem);
		lightIsOff = !lightIsOff;
	}
}



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_backlight_switch_init
* Purpose:    	Initialise the switch button SW2 for the LED backlight control
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_backlight_switch_init(void) {
	
	// Enable clock at port F
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTF); 
	while ((SYSCTL->RCGCGPIO & (1 << RCGCGPIO_PORTF)) == 0);
	
	// Unlock the GPIOF port and allow changes to PF0
	GPIOF->LOCK = 0x4C4F434B;
	GPIOF->CR |= 1 << PIN0;
	
	// Configure PF0 as digital input 
	GPIOF->DIR &= ~(1 << PIN0);
	GPIOF->AFSEL &= ~(1 << PIN0);
	GPIOF->DEN |= 1 << PIN0;
	GPIOF->AMSEL &= ~(1 << PIN0);
	
	// Set drive strength to 2mA and enable weak pull-up resistor
	GPIOF->DR2R |= 1 << PIN0;
	GPIOF->PUR |= 1 << PIN0;
	
	// Enable negative edge triggered IRQs at PF0
	GPIOF->IM &= ~(1 << PIN0);
	GPIOF->IS &= ~(1 << PIN0);
	GPIOF->IBE &= ~(1 << PIN0);
	GPIOF->IEV &= ~(1 << PIN0);
	GPIOF->RIS &= ~(1 << PIN0);
	GPIOF->IM |= 1 << PIN0;
	
	// Register the GPIOF interrupt at the NVIC controller
	KrisOS_irq_set_prio(GPIOF_IRQn, 3);
	KrisOS_irq_enable(GPIOF_IRQn);
}



/*-------------------------------------------------------------------------------
* Function:    	GPIOF_Handler
* Purpose:    	IRQ handler triggered by SW2 (PF0) button press. 
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void GPIOF_Handler(void) {
	
	// Release the semaphore for which nokiaLCDBacklight Task is waiting and clear
	// the interrupt source
	KrisOS_sem_release_ISR(backlightSem);
	GPIOF->ICR |= 1 << PIN0;
}



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_backlight_control_pin_init
* Purpose:    	Initialise the GPIO pin (PB0) for controlling LCD screen backlight
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_backlight_control_pin_init(void) {
	
	// Enable clock on port B
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTB); 
	while ((SYSCTL->RCGCGPIO & (1 << RCGCGPIO_PORTB)) == 0);
	
	// Configure PB0 as digital output
	GPIOB->DIR |= 1 << PIN0;
	GPIOB->AFSEL &= ~(1 << PIN0);
	GPIOB->DEN |= 1 << PIN0;
	GPIOB->AMSEL &= ~(1 << PIN0);
	
}

