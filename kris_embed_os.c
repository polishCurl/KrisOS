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

#include "heap.h"
#include "system.h"


/*-------------------------------------------------------------------------------
* Function:    	OS_init
* Purpose:    	OS initialisation method
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void OS_init(void) {
	
	// Initialise the system and important peripherals
	__disable_irqs();										// switch off interrupts
	systick_config(SYSTEM_CLOCK_FREQ / 100000);				// set up periodic irqs
	uart_init(11520); 										// set up UART
	
	// Initialise the OS components
	heap_init();											// initialise the heap
	__enable_irqs();										// enable interrupts again
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

