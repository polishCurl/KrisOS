/*******************************************************************************
* File:     	os.c
* Brief:    	KrisOS base kernel code
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	16/10/2016
* Last mod: 	13/03/2017
*
* Note: 	
*	os.c and scheduler.c form the basis of KrisOS kernel code with other source
*	files heap.c, mutex.c, ... being optional features. Here, the OS clock timer
*	interrupts as well as the 'C' part of the SVC calls (OS calls) are defined.
*	In addition to this the methods for initialising tha starting the OS are 
*	defined here.
*******************************************************************************/
#include "kernel.h"
#include "system.h"
#include "KrisOS.h"



/*-------------------------------------------------------------------------------
* Semihosting is not supported by MDK-ARM. Semihosting is a mechanism that enables 
* code running on an ARM target to communicate and use the Input/Output facilities 
* on a host computer that is running a debugger.
--------------------------------------------------------------------------------*/
#pragma import(__use_no_semihosting_swi)



/*-------------------------------------------------------------------------------
* Kernel control block - static memory allocation
--------------------------------------------------------------------------------*/
Kernel KrisOS;



/*-------------------------------------------------------------------------------
* Function:    	os_init
* Purpose:    	Initialise the operating system
* Arguments:	-
* Returns: 
* 		exit status		
--------------------------------------------------------------------------------*/
uint32_t os_init(void) {

	__disable_irqs();	
	{
		// Set the initial OS state and enable the Floating-Point Unit
		KrisOS.isRunning = 0;
		__enable_fpu();
		
		// Set up the system clock
		system_clock_config(CLOCK_SOURCE, SYSCLOCK_DIVIDER);
		
		// Reset the mutex, semaphore and queue counters 
		#if defined SHOW_DIAGNOSTIC_DATA && defined USE_MUTEX
			KrisOS.totalMutexNo = 0;
		#endif
			
		#if defined SHOW_DIAGNOSTIC_DATA && defined USE_SEMAPHORE
				KrisOS.totalSemNo = 0;
		#endif
		
		#if defined SHOW_DIAGNOSTIC_DATA && defined USE_QUEUE
				KrisOS.totalQueueNo = 0;
		#endif
		
		// Initialse the scheduler
		scheduler_init();
			
		// Initialise the heap manager
		#ifdef USE_HEAP
			heap_init();
		#endif			
			
		// Initialise the uart serial interface
		#ifdef USE_UART		
			uart_init(); 
		#endif
		
		// SVC calls (software interrupts) are used for interaction between the user and 
		// the operating system. Handling external interrupts should have the priority
		// over handling OS calls (SVC calls)
		nvic_set_priority(SVCall_IRQn, 7);
	}
	__enable_irqs();	
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	os_start
* Purpose:    	Start the operating system by redirecting the execution to the top
*				priority ready task in the scheduler
* Arguments:	-
* Returns: 
* 		exit status	
--------------------------------------------------------------------------------*/
uint32_t os_start(void) {
	
	// Helper pointer for navigating around the private stack memory of the first
	// task to run
	uint32_t* taskFramePtr; 	

	// Find the first task to run
	scheduler_run();
	scheduler.runPtr = scheduler.topPrioTask;
	scheduler.topPrioTask->status = READY;
	
	// Prepare to run the first task by loading the values of CONTROL and PSP 
	// registers that have been specified during the task creation
	__set_psp(scheduler.runPtr->sp + (STACK_FRAME_R0 << 2)); 
	taskFramePtr = (uint32_t*) (scheduler.runPtr->sp + (STACK_FRAME_CONTROL << 2));
	__set_control(*taskFramePtr);
	taskFramePtr = (uint32_t*) scheduler.runPtr->sp;
	scheduler.svcExcReturn = *taskFramePtr;
	
	// It is assumed that the first task run will use up its timeslice completely.
	// So the preemption flag should be raised as if the tasks executed uninterrupted
	// for 
	scheduler.preemptFlag = 1;
	
	// Set up periodic interrupts - the OS clock
	systick_config(SYSTEM_CLOCK_FREQ / OS_CLOCK_FREQ);	
	
	// OS is now running
	KrisOS.isRunning = 1;
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	SysTick_Handler
* Purpose:    	OS 'tick' timer interrupt handler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void SysTick_Handler(void) {
	
	// Increment the OS ticks counter
	KrisOS.ticks++;
	
	// On each OS timer interrupt increment a counter of the currently running task
	// so that the CPU usage can be computed later by the KrisOS usage statistics 
	// task
	#ifdef SHOW_DIAGNOSTIC_DATA
		scheduler.runPtr->cpuUsage++;
	#endif
	
	// If there is curently at least one suspended task in the scheduler, check if it
	// is now ready, if so, wake all the tasks that reached their wait timout value
	if (scheduler.blocked != NULL && scheduler.blocked->waitCounter <= KrisOS.ticks)
		scheduler_wake_tasks();
		
	// If the currently running task has used up its entire time slice, then
	// it should be preempted. Otherwise, mark it as a future candidate for
	// preemption (if it doesn't get disturbed for the whole duration of a 
	// time slice.
	if (KrisOS.ticks % TIME_SLICE == 0) {
		if (scheduler.preemptFlag)
			scheduler_run();
		else
			scheduler.preemptFlag = 1;
	}
}



/*-------------------------------------------------------------------------------
* Function:    	SVC_Handler_C
* Purpose:    	The 'C' part of the SVC call handler - the mechanism for requesting
*				KrisOS servises by the user code.
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void SVC_Handler_C(uint32_t* svcArgs) {
	
	// Extract the SVC number and use it to run the right subroutine 
	uint8_t svcNumber = ((uint8_t*) svcArgs[6])[-2];
	switch(svcNumber) {
// ---- OS initialisation and launch SVC calls  ---------------------------------
		case SVC_OS_INIT: svcArgs[0] = os_init(); break;
		case SVC_OS_START: svcArgs[0] = os_start(); break;	
		
// ---- NVIC Interrupt control SVC calls ----------------------------------------
		case SVC_IRQ_EN: svcArgs[0] = nvic_enable_irq((IRQn_Type) svcArgs[0]); break;
		case SVC_IRQ_DIS: svcArgs[0] = nvic_disable_irq((IRQn_Type) svcArgs[0]); break;
		case SVC_IRQ_SET_PEND: svcArgs[0] = nvic_set_pending((IRQn_Type) svcArgs[0]); break;
		case SVC_IRQ_CLEAR_PEND: svcArgs[0] = nvic_clear_pending((IRQn_Type) svcArgs[0]); break;
		case SVC_IRQ_READ_ACTIVE: svcArgs[0] = nvic_read_active((IRQn_Type) svcArgs[0]); break;
		case SVC_IRQ_SET_PRIO: svcArgs[0] = nvic_set_priority((IRQn_Type) svcArgs[0], svcArgs[1]); break;
		case SVC_IRQ_GET_PRIO: svcArgs[0] = nvic_get_priority((IRQn_Type) svcArgs[0]); break;		
			
// ---- Task scheduling SVC calls -----------------------------------------------
		#ifdef USE_HEAP
		case SVC_TASK_NEW: svcArgs[0] = (uint32_t) task_create_dynamic((void*) svcArgs[0], 
			svcArgs[1], svcArgs[2], 0); break;
		#endif
		case SVC_TASK_NEW_S: svcArgs[0] = task_create_static((void*) svcArgs[0], 
			(void*) svcArgs[1], (void*) svcArgs[2], svcArgs[3], 0); break;
		case SVC_TASK_SLEEP: svcArgs[0] = task_sleep(svcArgs[0]);  break;
		case SVC_TASK_YIELD: svcArgs[0] = scheduler_run(); break;
		case SVC_TASK_DELETE: svcArgs[0] = task_delete(); break;
		
// ---- Heap management SVC calls -----------------------------------------------
		#ifdef USE_HEAP	
		case SVC_HEAP_ALLOC: svcArgs[0] = (uint32_t) malloc(svcArgs[0]); break;
		case SVC_HEAP_FREE: free((void*) svcArgs[0]); break;
		#endif
		
// ---- Mutual exclusion lock management SVC calls ------------------------------
		#ifdef USE_MUTEX
		case SVC_MTX_INIT: svcArgs[0] = mutex_init((void*) svcArgs[0]); break;
		#ifdef USE_HEAP
			case SVC_MTX_CREATE: svcArgs[0] = (uint32_t) mutex_create(); break;
			case SVC_MTX_DELETE: svcArgs[0] = mutex_delete((void*) svcArgs[0]); break;
		#endif
		case SVC_MTX_TRY_LOCK: svcArgs[0] = mutex_try_lock((void*) svcArgs[0]); break;
		case SVC_MTX_LOCK: svcArgs[0] = (uint32_t) mutex_lock((void*) svcArgs[0]); break;
		case SVC_MTX_UNLOCK: svcArgs[0] = mutex_unlock((void*) svcArgs[0]); break;
		#endif
		
// ---- Semaphore management SVC calls -------------------------------------------
		#ifdef USE_SEMAPHORE
		case SVC_SEM_INIT: svcArgs[0] = sem_init((void*) svcArgs[0], svcArgs[1]); break;
		#ifdef USE_HEAP
			case SVC_SEM_CREATE: svcArgs[0] = (uint32_t) sem_create(svcArgs[0]); break;
			case SVC_SEM_DELETE: svcArgs[0] = sem_delete((void*) svcArgs[0]); break;
		#endif
		case SVC_SEM_TRY_ACQUIRE: svcArgs[0] = (uint32_t) sem_try_acquire((void*) svcArgs[0]); break;
		case SVC_SEM_ACQUIRE: svcArgs[0] = sem_acquire((void*) svcArgs[0]); break;
		case SVC_SEM_RELEASE: svcArgs[0] = (uint32_t) sem_release((void*) svcArgs[0]); break;
		#endif 
		
// ---- Inter-task queue management SVC calls -------------------------------------------
		#ifdef USE_QUEUE
		case SVC_QUEUE_INIT: svcArgs[0] = queue_init((void*) svcArgs[0], (void*) svcArgs[1],
			svcArgs[2], svcArgs[3]); break;
		#ifdef USE_HEAP
			case SVC_QUEUE_CREATE: svcArgs[0] = (uint32_t) queue_create(svcArgs[0], 
				svcArgs[1]); break;
			case SVC_QUEUE_DELETE: svcArgs[0] = queue_delete((void*) svcArgs[0]); break;
		#endif
		case SVC_QUEUE_TRY_WRITE: svcArgs[0] = queue_try_write((void*) svcArgs[0], 
			(const void*) svcArgs[1]); break;	
		case SVC_QUEUE_TRY_READ: svcArgs[0] = queue_try_read((void*) svcArgs[0], 
			(void*) svcArgs[1]); break;
		case SVC_QUEUE_ENQUEUE: svcArgs[0] = queue_enqueue((void*) svcArgs[0], 
			(const void*) svcArgs[1]); break;	
		case SVC_QUEUE_DEQUEUE: svcArgs[0] = queue_dequeue((void*) svcArgs[0], 
			(void*) svcArgs[1]); break;	
		#endif 		
		
		default: break;
	}
	return;
}



/*-------------------------------------------------------------------------------
* Function:    	_sys_exit
* Purpose:    	Terminate the operating system 
* Arguments:	
*		exitCode - a number representing the cause of KrisOS termination
* Returns: 		-
--------------------------------------------------------------------------------*/
void _sys_exit(int return_code) {
	
	// Display informative message specifying the cause of KrisOS termination
	#if defined USE_UART && defined SHOW_DIAGNOSTIC_DATA
		switch (return_code) {
			case EXIT_SUCCESS: 
				fprintf(&uart, "\nShutting down KrisOS. No errors..."); 
				break;
			case EXIT_NULL: 
				fprintf(&uart, "\nInvalid reference! NULL pointer passed...");
				break;
			case EXIT_INVALID_IRQ_PRIO: 
				fprintf(&uart, "\nInvalid interrupt priority specified! Greater than 7...");
				break;
			case EXIT_HEAP_TOO_SMALL: 
				fprintf(&uart, "\nNo more heap space available! Increase the heap size...");
				break;
			case EXIT_INVALID_SIZE:
				fprintf(&uart, "\nInvalid size argument specified! Should be greated than 0...");
				break;
			case EXIT_INVALID_OS_CLOCK_FREQ:
				fprintf(&uart, "\nInvalid OS clock frequency specified! Try a different value such as 100Hz or 100000Hz...");
				break;
			// With the invalid baud rate displaying any error message over serial monitor
			// doesn't make sense
			case EXIT_UART_INVALID_BAUD_RATE:
				while(1);
			default: break;
		}

		fprintf(&uart, "\nTerminating...");
	#endif
	
	// Semihosting is not supported, so this method should not return 
	while(1);
}

