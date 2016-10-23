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
#include "heap.h"
#include "KrisOs.h"


/*-------------------------------------------------------------------------------
* Function:    	os_init
* Purpose:    	KrisOS initialisation method
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_init(void) {

	// Switch off interrupts and enable the FPU
	__disable_irqs();								
	__enable_fpu();
	
	// Set up the system clock
	system_clock_config(CLOCK_SOURCE, SYSCLOCK_DIVIDER);				
	
	// Set up periodic interrupts
	systick_config(TIME_SLICE);		
	
	// initialise the uart serial interface
	uart_init(SERIAL_MONITOR_BAUD_RATE, SERIAL_MONITOR_WORD_LEN,  
			  SERIAL_MONITOR_D0_PARITY_CHECK, SERIAL_MONITOR_PARITY, 
			  SERIAL_MONITOR_STOP_BITS); 
	
	// Initialise the heap
	heap_init();		

	// Re-enable interrupts	
	__enable_irqs();	
}


/*-------------------------------------------------------------------------------
* Function:    	SysTick_Handler
* Purpose:    	SysTick timer interrupt handler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void SysTick_Handler(void) {
	TICKS++;							// Increment the clock ticks counter
	//run_scheduler(); 					// Run the task scheduler
	//SCB->ICSR |= (1 << PENDSV_Pos);		// Set the PendSV_IRQ
}


