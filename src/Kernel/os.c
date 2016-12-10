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
#include "scheduler.h"


/*-------------------------------------------------------------------------------
* Flag indicating whether the operating system is running
--------------------------------------------------------------------------------*/
uint32_t OS_RUNNING = 0;



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

	// Initialse the scheduler
	scheduler_init();
	
	// Set the IRQ priority of Interrupts for task scheduling 
	// 1. SysTick (highest possible)
	// 2. SVC and PendSV (lowest possible) IRQs
	nvic_set_priority(SysTick_IRQn, 0);
	nvic_set_priority(PendSV_IRQn, 7);
	nvic_set_priority(SVCall_IRQn, 7);

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

	// Find the first task to run. Scheduler_run determines the next task to run so
	// scheduler.runPtr is assigned mannually
	scheduler_run();
	scheduler.runPtr = scheduler.topPrioTask;
	
	// Set the initial value of svc_exc_return, CONTROL register as well as PSP.
	// PSP should be pointing to the position of PC in the stack frame of the first task
	// to run.
	__set_psp(scheduler.runPtr->sp + (STACK_FRAME_R0 << WORD_ACCESS_SHIFT)); 
	taskFramePtr = (uint32_t*) (scheduler.runPtr->sp + (STACK_FRAME_CONTROL << WORD_ACCESS_SHIFT));
	__set_control(*taskFramePtr);
	taskFramePtr = (uint32_t*) scheduler.runPtr->sp;
	svc_exc_return = *taskFramePtr;
	
	// Set up periodic interrupts
	systick_config(SYSTEM_CLOCK_FREQ / OS_CLOCK_FREQ);	
	OS_RUNNING = 1;
}



/*-------------------------------------------------------------------------------
* Function:    	SysTick_Handler
* Purpose:    	SysTick timer interrupt handler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void SysTick_Handler(void) {
	
	// Increment the OS ticks counter
	OS_TICKS++;
	
	// If a task(s) has just become ready then wake it up
	if (OS_TICKS == scheduler.nextToWake)
		scheduler_wake_tasks();
	
	// Otherwise, if the currently running task has used up its time slice then
	// check if it should be preempted
	else if (OS_TICKS % TIME_SLICE == 0) 
		scheduler_run();
	
	// Perform context-switch only if the next task to run is different from the 
	// current one, according to the scheduling policy
	if (scheduler.topPrioTask != scheduler.runPtr)
		SCB->ICSR |= (1 << PENDSV_Pos);	
}




/*-------------------------------------------------------------------------------
* Function:    	SVC_Handler_C
* Purpose:    	User interface to the operating system. Part of the SVC_Handler 
* 				written in C
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void SVC_Handler_C(uint32_t* svcArgs) {
	
	// Extract the SVC number and use it to run the right subroutine using the 
	// arguments saved on the stack
	uint8_t svcNumber = ((uint8_t*) svcArgs[6])[-2];
	switch(svcNumber) {
		case SVC_OS_INIT: os_init(); break;
		case SVC_OS_START: os_start(); break;
		case SVC_CREATE_TASK: svcArgs[0] = task_create((void*) svcArgs[0], svcArgs[1],  
														 svcArgs[2], 0); break;
		case SVC_DECLARE_TASK: svcArgs[0] = task_declare((void*) svcArgs[0], (void*) svcArgs[1],  
														 (void*) svcArgs[2], svcArgs[3], 0); 
		                                                 break;
		case SVC_DELAY_TASK: svcArgs[0] = task_delay(svcArgs[0]);  break;
		/*
		case SVC_SUSPEND_TASK: svcArgs[0] = svcArgs[0] > 0 ? task_suspend(svcArgs[0]) : 
											EXIT_FAILURE;  break;
		case SVC_RESUME_TASK: svcArgs[0] = svcArgs[0] > 0 ? task_resume(svcArgs[0]) : 
											EXIT_FAILURE;  break;
		*/
		default: break;
	}
	return;
}


