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
#include "KrisOs.h"
#include "heap.h"
#include "scheduler.h"


/*-------------------------------------------------------------------------------
* Flag indicating whether the operating system is running
--------------------------------------------------------------------------------*/
uint32_t OS_RUNNING;


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
	
	OS_RUNNING = 0;
	
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
* Function:    	os_start
* Purpose:    	Start the operating system
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_start(void) {
	
	// Helper pointer for specifying register address within the task's stack frame
	uint32_t* taskFramePtr; 	
	
	// Index of the first task to run
	runPtr = readyQueue.tasks[readyQueue.noOfTasks - 1];
	
	// Set the initial value of svc_exc_return, CONTROL register as well as PSP.
	// PSP should be pointing to the position of PC in the stack frame of the first task
	// to run.
	__set_psp(runPtr->sp + (STACK_FRAME_R0 << WORD_ACCESS_SHIFT)); 
	taskFramePtr = (uint32_t*) (runPtr->sp + (STACK_FRAME_CONTROL << WORD_ACCESS_SHIFT));
	__set_control(*taskFramePtr);
	taskFramePtr = (uint32_t*) runPtr->sp;
	svc_exc_return = *taskFramePtr;

	// Set the status of the first task to run as RUNNING
	runPtr->status = RUNNING;
	
	// Schedule the tasks
	run_scheduler();
	
	// Set up periodic interrupts
	systick_config(TIME_SLICE);	
	OS_RUNNING = 1;
}


/*-------------------------------------------------------------------------------
* Function:		os_sleep
* Purpose:    	Put the operating system into power-saving mode
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_sleep(void) {
	while(1) __wfi();
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

	// Update task counters
	update_counters();
	
	// Set the PendSV_IRQ only if the next task to run is different from 
	// the current one
	if (runPtr != runPtr->next)
		SCB->ICSR |= (1 << PENDSV_Pos);	
}


