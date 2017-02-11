/*******************************************************************************
* File:     	stats.c
* Brief:    	Statistics task gathering usage statistics and displaying them
* 				peridically
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	07/02/2017
* Last mod: 	07/02/2017
*
* Note: 		
*******************************************************************************/
#include "common.h"
#include "kernel.h"



#ifdef SHOW_DIAGNOSTIC_DATA
KrisOS_task_define(stats, 400, UINT8_MAX - 1)


/*-------------------------------------------------------------------------------
* Function:		stats_init
* Purpose:    	Initialise the statistics task
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void stats_init(void) {
	
	// Reset the mutex counter
	#ifdef USE_MUTEX
		KrisOS.totalMutexNo = 0;
	#endif

	// Reset the semaphore counter
	#ifdef USE_SEMAPHORE
		KrisOS.totalSemNo = 0;
	#endif
	
	// Initialise the stats task
	KrisOS_stack_usage((uint32_t*) &statsStack[0], statsStackSize);
	task_create_static(&statsTask, stats_task, &statsStack[statsStackSize], statsPriority, 1);
}



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
	
	// Reset the CPU usage counters as some high priority task has already run 
	// by the time this one starts
	for (index = 0; index < scheduler.totalTaskNo; index++) 
			scheduler.taskRegistry[index]->cpuUsage = 0;
	
	while(1) {
		// Reset the usage data
		scheduler.idleTime = scheduler.contextSwitchNo = 0;
		#ifdef USE_MUTEX
			KrisOS.maxMtxCriticalSection = 0;
		#endif
		
		// Take note of the time the task was last run and put it to sleep in order
		// to take time to gather usage data
		lastRun = KrisOS.ticks;
		task_sleep(DIAG_DATA_RATE, SLEEPING);
		
		// Update the current time 
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
				fprintf(uart, "Heap usage:\t\t%dB/%dB = %d%%\n", heap.heapBytesUsed, HEAP_SIZE, 
					    heap.heapBytesUsed * 100 / HEAP_SIZE);
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
	
#endif

