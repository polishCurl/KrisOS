/*******************************************************************************
* File:     	os.c
* Brief:    	KrisOS implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	16/10/2016
* Last mod: 	16/10/2016
*
* Note: 	
*******************************************************************************/
#include "system.h"
#include "os.h"


/*-------------------------------------------------------------------------------
* Function:    	os_init
* Purpose:    	KrisOS initialisation method
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_init(void) {

	// Initialise the system and important peripherals
	__disable_irqs();								// switch off interrupts
	__enable_fpu(); 								// switch on the FPU
	system_clock_config(MAIN_OSC, 10);				// set up the system clock
	systick_config(SYSTEM_CLOCK_FREQ / 100);		// set up periodic irqs
	uart_init(9600, 3, 0, 0, 0); 					// initialise the uart serial interface
	//nvic_enable_irq(QEI1_IRQn);
	//nvic_set_pending(QEI1_IRQn);
	
	
	// Initialise the OS components
	heap_init();									// initialise the heap
	__enable_irqs();								// enable interrupts again	
	
	malloc(sizeof(int32_t));
	
}


/*-------------------------------------------------------------------------------
* Function:    	SysTick_Handler
* Purpose:    	SysTick timer interrupt handler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void SysTick_Handler(void) {
	TICKS++;
}


