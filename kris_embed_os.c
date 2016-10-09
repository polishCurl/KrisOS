/*******************************************************************************
* File:     	kris_embed_os.c
* Brief:    	Implementation of functions visible to OS users
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 	
*******************************************************************************/
#include "system.h"
#include "heap.h"


/*-------------------------------------------------------------------------------
* Function:    	OS_init
* Purpose:    	OS initialisation method
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void OS_init(void) {
	
	// Initialise the system and important peripherals
	__disable_irqs();								// switch off interrupts
	__enable_fpu(); 								// switch on the FPU
	system_clock_config(MAIN_OSC, 10);				// set up the system clock
	systick_config(SYSTEM_CLOCK_FREQ);				// set up periodic irqs
	uart_init(9600, 3, 0, 0, 0); 
	
	// Initialise the OS components
	heap_init();									// initialise the heap
	__enable_irqs();								// enable interrupts again
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


