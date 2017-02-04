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
#include "KrisOS.h"
#include "system.h"
#include "kernel.h"


/*-------------------------------------------------------------------------------
* Kernel status block
--------------------------------------------------------------------------------*/
Kernel KrisOS;



/*-------------------------------------------------------------------------------
* Value of exception return for SVC calls
--------------------------------------------------------------------------------*/
uint32_t svc_exc_return;



/*-------------------------------------------------------------------------------
* Function:		idle_task
* Purpose:    	Idle task. Lowest priority task used for power saving.
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void idle_task(void) {
	while(1) __wfi();
}

KrisOS_task_define(idle, 100)



#ifdef SHOW_DIAGNOSTIC_DATA
/*-------------------------------------------------------------------------------
* Function:		stats_task
* Purpose:    	KrisOS performance statistics task. Displays diagnostic data. 
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void stats_task(void) {
	
	// Per-task CPU usage, the last time the statistics task was run and the current 
	// value of OS timer
	uint32_t cpuUsage;
	uint64_t lastRun;
	uint64_t currentTime;
	
	// Task registry iterator
	int32_t index;
	Task* iterator;
	
	// Stack usage helper iterator and stack usage value
	uint32_t* stackUsageHelper;
	uint32_t stackUsage;
	
	while(1) {
		
		// Reset the usage data
		scheduler.idleTime = scheduler.contextSwitchNo = 0;
		#ifdef USE_MUTEX
			KrisOS.maxMtxCriticalSection = 0;
		#endif
		
		// Take note of the time the task iteration is taking place
		lastRun = KrisOS.ticks;
		
		// Suspend the task in order to take time to gather usage data
		task_sleep(DIAG_DATA_RATE, SLEEPING);
		
		currentTime = KrisOS.ticks;
		
		#ifdef USE_MUTEX
			mutex_lock(&uartMtx);
		#endif
		{
			// Display OS usage data
			fprintf(uart, "\n------------------------------------------------------------------------------\n");
			fprintf(uart, "Time running:\t\t%d days, %d hours, %d minutes, %d seconds\n", 
				    (uint32_t) (currentTime / 86400000UL),
				   (uint32_t) ((currentTime % 86400000UL)) / 3600000, 
				   (uint32_t) ((currentTime % 3600000)) / 60000, 
				   (uint32_t) ((currentTime % 60000)) / 1000);
			fprintf(uart, "Measurement period:\t%d ms\n", (uint32_t) (currentTime - lastRun));
			fprintf(uart, "Context switches:\t%d\n", scheduler.contextSwitchNo);
			fprintf(uart, "Total task number:\t%d\n", scheduler.totalTaskNo);
			
			#ifdef USE_MUTEX
				fprintf(uart, "Total mutex number:\t%d\n", KrisOS.totalMutexNo);
				fprintf(uart, "Max mutex lock time:\t%d ms\n", KrisOS.maxMtxCriticalSection);
			#endif
			
			#ifdef USE_SEMAPHORE
				fprintf(uart, "Total semaphore number:\t%d\n", KrisOS.totalSemNo);
			#endif
			
			#ifdef USE_HEAP
				fprintf(uart, "Heap usage:\t\t%dB/%dB = %d%%\n", heapBytesUsed, HEAP_SIZE, heapBytesUsed * 100 / HEAP_SIZE);
			#endif
			
			// Display the task manager (per-task statistics)
			fprintf(uart, "\nTID\tCPU usage\tStack usage\tPriority\tStatus\t\tMemory\n");
			for (index = 0; index < scheduler.totalTaskNo; index++) {
				iterator = scheduler.taskRegistry[index];
				
				// Compute the CPU usage as the proportion of the OS ticks spend executing the task
				// to the total number of OS ticks since the last time the statistics task was run
				cpuUsage = iterator->cpuUsage * 100 / (currentTime - lastRun);
				
				// Calculate the stack usage by calculating the offset from the stack base to the first
				// memory location that hasn't been modified
				stackUsageHelper = (uint32_t*) iterator->sp;
				while (*stackUsageHelper-- != 0xDEADBEEF);
				if (iterator->memoryType == DYNAMIC) 
					stackUsage = iterator->stackSize - ((stackUsageHelper - iterator->stackBase) << 2);
				else 
					stackUsage = (iterator->stackBase - stackUsageHelper) << 2;
					
				fprintf(uart, "%d\t%d%%\t\t%dB\t\t%d\t\t", iterator->id, cpuUsage, stackUsage, iterator->priority);
				iterator->cpuUsage = 0;
				
				// Display the current task status 
				switch(iterator->status) {
					case RUNNING: fprintf(uart, "RUNNING\t\t"); break;
					case READY: fprintf(uart, "READY\t\t"); break;
					case SLEEPING: fprintf(uart, "SLEEPING\t"); break;
					case MTX_WAIT: fprintf(uart, "MUTEX WAIT\t"); break;
					case SEM_WAIT: fprintf(uart, "SEM WAIT\t"); break;
					default: break;
				}

				// Display the type of memory allocation used for the task and its stack (static/dynamic)
				switch(iterator->memoryType) {
					case STATIC: fprintf(uart, "Static\n"); break;
					case DYNAMIC: fprintf(uart, "Dynamic\n"); break;
					default: break;
				}
			}
			fprintf(uart, "------------------------------------------------------------------------------\n");
		}
		#ifdef USE_MUTEX
			mutex_unlock(&uartMtx);
		#endif
	}
}

KrisOS_task_define(stats, 400)
#endif



/*-------------------------------------------------------------------------------
* Function:    	os_init
* Purpose:    	KrisOS initialisation method
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_init(void) {

	__disable_irqs();	
	{
		// Set the initial OS state and enable the Floating-Point Unit
		KrisOS.isRunning = 0;
		__enable_fpu();
		
		// Set up the system clock
		system_clock_config(CLOCK_SOURCE, SYSCLOCK_DIVIDER);					

		// Initialise the heap
		#ifdef USE_HEAP
			heap_init();
		#endif		
		
		// Initialse the scheduler
		scheduler_init();
		
		// Create system tasks:
		// 1. Idle task (priviliged, lowest possible task priority)
		#ifdef SHOW_DIAGNOSTIC_DATA
			KrisOS_stack_usage((uint32_t*) &idleStack[0], idleStackSize);
		#endif
		task_create_static(&idleTask, idle_task, &idleStack[idleStackSize], UINT8_MAX, 1);
		
		#ifdef SHOW_DIAGNOSTIC_DATA
			// 2. OS performance statistics task. Displays usage data periodically.
			KrisOS_stack_usage((uint32_t*) &statsStack[0], statsStackSize);
			task_create_static(&statsTask, stats_task, &statsStack[statsStackSize], UINT8_MAX - 1, 1);	

			// Reset the mutex counter
			#ifdef USE_MUTEX
				KrisOS.totalMutexNo = 0;
			#endif
			
			// Reset the mutex counter
			#ifdef USE_SEMAPHORE
				KrisOS.totalSemNo = 0;
			#endif
		#endif
			
		// Initialise the uart serial interface
		#ifdef USE_UART		
			uart_init(UART_BAUD_RATE, UART_WORD_LEN, UART_D0_PARITY_CHECK, UART_PARITY, UART_STOP_BITS); 
		#endif
		
		// Set the IRQ priority of Interrupts for task scheduling 
		// 1. SysTick (highest possible)
		// 2. SVC and PendSV (lowest possible) IRQs
		nvic_set_priority(SysTick_IRQn, 0);
		nvic_set_priority(PendSV_IRQn, 7);
		nvic_set_priority(SVCall_IRQn, 7);
	}
	__enable_irqs();	
}



/*-------------------------------------------------------------------------------
* Function:    	os_start
* Purpose:    	Start the operating system by setting up the first task to run
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_start(void) {
	
	// Helper pointer for specifying register address within the task's stack frame
	uint32_t* taskFramePtr; 	

	// Find the first task to run. 
	scheduler_run();
	scheduler.runPtr = scheduler.topPrioTask;
	scheduler.topPrioTask->status = READY;
	
	// Set the initial value of svc_exc_return, CONTROL register as well as PSP.
	// PSP should be pointing to the position of PC in the stack frame of the first task
	// to run.
	__set_psp(scheduler.runPtr->sp + (STACK_FRAME_R0 << WORD_ACCESS_SHIFT)); 
	taskFramePtr = (uint32_t*) (scheduler.runPtr->sp + (STACK_FRAME_CONTROL << WORD_ACCESS_SHIFT));
	__set_control(*taskFramePtr);
	taskFramePtr = (uint32_t*) scheduler.runPtr->sp;
	svc_exc_return = *taskFramePtr;
	
	// Initially it is assumed that the first task will run until it's time preempted
	// (will use its time slice completely)
	scheduler.status |= (1 << TIME_PREEMPT_Pos);
	
	// Set up periodic interrupts
	systick_config(SYSTEM_CLOCK_FREQ / OS_CLOCK_FREQ);	
	
	// OS is now running
	KrisOS.isRunning = 1;
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
	
	// Update the cpu usage data of currently running task
	#ifdef SHOW_DIAGNOSTIC_DATA
		scheduler.runPtr->cpuUsage++;
	#endif
	
	// If the soonest delayed task has become ready then wake all the currently ready tasks up
	if (scheduler.blocked != NULL && scheduler.blocked->waitCounter <= KrisOS.ticks)
		scheduler_wake_tasks();
		
	// If the currently running task has used up its entire time slice, then
	// it should be preempted.
	if (KrisOS.ticks % TIME_SLICE == 0) {
		if (scheduler.status & (1 << TIME_PREEMPT_Pos))
			scheduler_run();
		else
			scheduler.status |= (1 << TIME_PREEMPT_Pos);
	}
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
// ---- OS initialisation and launch --------------------------------------------
		case SVC_OS_INIT: os_init(); break;
		case SVC_OS_START: os_start(); break;	
		
// ---- Interrupt control SVC calls ---------------------------------------------
		case SVC_IRQ_EN: 
			nvic_enable_irq((IRQn_Type) svcArgs[0]);
			svcArgs[0] = EXIT_SUCCESS;
			break;
		case SVC_IRQ_DIS: 
			nvic_disable_irq((IRQn_Type) svcArgs[0]);
			svcArgs[0] = EXIT_SUCCESS;
			break;
		case SVC_IRQ_SET_PEND: 
			nvic_set_pending((IRQn_Type) svcArgs[0]);
			svcArgs[0] = EXIT_SUCCESS;
			break;
		case SVC_IRQ_CLEAR_PEND: 
			nvic_clear_pending((IRQn_Type) svcArgs[0]);
			svcArgs[0] = EXIT_SUCCESS;
			break;
		case SVC_IRQ_READ_ACTIVE: svcArgs[0] = nvic_read_active((IRQn_Type) svcArgs[0]);
			break;
		case SVC_IRQ_SET_PRIO: 
			if (nvic_irq_prio_check(svcArgs[1]))
				svcArgs[0] = EXIT_FAILURE;
			else {
				nvic_set_priority((IRQn_Type) svcArgs[0], svcArgs[1]);
				svcArgs[0] = EXIT_SUCCESS;
			}
			break;		
		case SVC_IRQ_GET_PRIO: svcArgs[0] = nvic_get_priority((IRQn_Type) svcArgs[0]);
			break;		
			
// ---- Task scheduling SVC calls -----------------------------------------------
		#ifdef USE_HEAP
		case SVC_TASK_NEW: 	svcArgs[0] = (uint32_t) task_create_dynamic((void*) svcArgs[0], 
																		svcArgs[1], svcArgs[2], 0); 
							break;
		#endif
		case SVC_TASK_NEW_S: svcArgs[0] = task_create_static((void*) svcArgs[0], (void*) svcArgs[1],  
														     (void*) svcArgs[2], svcArgs[3], 0); break;
		case SVC_TASK_SLEEP: svcArgs[0] = task_sleep(svcArgs[0], SLEEPING);  break;
		case SVC_TASK_YIELD: scheduler_run(); break;
		case SVC_TASK_DELETE: task_delete(); break;
		
// ---- Heap management SVC calls -----------------------------------------------
		#ifdef USE_HEAP	
		case SVC_HEAP_ALLOC: svcArgs[0] = (uint32_t) malloc(svcArgs[0]); break;
		case SVC_HEAP_FREE: free((void*) svcArgs[0]); break;
		#endif
		
// ---- Mutual exclusion lock management SVC calls ------------------------------
		#ifdef USE_MUTEX
		case SVC_MTX_INIT: svcArgs[0] = mutex_init((void*) svcArgs[0]); break;
		case SVC_MTX_CREATE: svcArgs[0] = (uint32_t) mutex_create(); break;
		case SVC_MTX_TRY_LOCK: svcArgs[0] = mutex_try_lock((void*) svcArgs[0]); break;
		case SVC_MTX_LOCK: svcArgs[0] = (uint32_t) mutex_lock((void*) svcArgs[0]); break;
		case SVC_MTX_UNLOCK: svcArgs[0] = mutex_unlock((void*) svcArgs[0]); break;
		case SVC_MTX_DELETE: svcArgs[0] = mutex_delete((void*) svcArgs[0]); break;
		#endif
		
// ---- Semaphore management SVC calls -------------------------------------------
		case SVC_SEM_INIT: svcArgs[0] = sem_init((void*) svcArgs[0], svcArgs[1]); break;
		case SVC_SEM_CREATE: svcArgs[0] = (uint32_t) sem_create(svcArgs[0]); break;
		case SVC_SEM_DELETE: svcArgs[0] = sem_delete((void*) svcArgs[0]); break;
		case SVC_SEM_TRY_ACQUIRE: svcArgs[0] = (uint32_t) sem_try_acquire((void*) svcArgs[0]); break;
		case SVC_SEM_ACQUIRE: svcArgs[0] = sem_acquire((void*) svcArgs[0], TIME_INFINITY); break;
		case SVC_SEM_ACQUIRE_TIME: svcArgs[0] = sem_acquire((void*) svcArgs[0], svcArgs[1]); break;
		case SVC_SEM_RELEASE: svcArgs[0] = (uint32_t) sem_release((void*) svcArgs[0]); break;
		default: break;
	}
	return;
}
