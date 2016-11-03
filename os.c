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
#include "task_scheduling.h"
#include "os.h"



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
	
	// initialise the uart serial interface
	uart_init(SERIAL_MONITOR_BAUD_RATE, SERIAL_MONITOR_WORD_LEN,  
			  SERIAL_MONITOR_D0_PARITY_CHECK, SERIAL_MONITOR_PARITY, 
			  SERIAL_MONITOR_STOP_BITS); 
	
	// Initialise the heap
	heap_init();	

	// Initialse the task control block
	tcb_init();
	
	// Set the IRQ priority of SysTick (highest possible) and PendSV (lowest possible) IRQs
	// as well as SVC cals
	nvic_set_priority(PendSV_IRQn, 0x7);
	nvic_set_priority(SysTick_IRQn, 0x0);
	nvic_set_priority(SVCall_IRQn, 0x6);

	// Re-enable interrupts	
	__enable_irqs();	
}


/*-------------------------------------------------------------------------------
* Function:		os_sleep
* Purpose:    	Put the operating system into power-saving mode
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_sleep(void) {
	__wfi();
}


/*-------------------------------------------------------------------------------
* Function:    	SysTick_Handler
* Purpose:    	SysTick timer interrupt handler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void SysTick_Handler(void) {
	
	// Increment the clock ticks counter
	TICKS++;				

	// Check if context switch should be performed. This happens only if the next
	// task to run and the current one are different
	test_context_switch();
}


