/*******************************************************************************
* File:     	os_tasks.c
* Brief:    	System (priviliged) tasks created by KrisOS
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	07/02/2017
* Last mod: 	07/02/2017
*
* Note: 		
*******************************************************************************/
#include "common.h"
#include "kernel.h"



/*******************************************************************************
* Task: 	idle
* Purpose: 	Idle task. Lowest priority task used for power saving when no other 
*			task is currently ready.
*******************************************************************************/
void idle(void) {
	while(1) __wfi();
}



#ifdef SHOW_DIAGNOSTIC_DATA
/*******************************************************************************
* Task: 	stats
* Purpose: 	KrisOS performance statistics task. Displays diagnostic data 
*			periodically
*******************************************************************************/
void stats(void) {
	
	// A Task's CPU usage both as a floating-point number and corresponding fixed
	// point representation
	uint32_t cpuUsageInt;
	uint32_t cpuUsageFrac;
	float32_t cpuUsage;
	
	// The last time the statistics task was run and the current value of OS timer
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
		task_sleep(DIAG_DATA_RATE);
		
		// Update the current time 
		currentTime = KrisOS.ticks;
		
		#ifdef USE_MUTEX
			mutex_lock(&uartMtx);
		#endif
		{
			// Display OS usage data
			fprintf(&uart, "\n------------------------------------------------------------------------------\n");
			fprintf(&uart, "Time running:\t\t%d days, %d hours, %d minutes, %d seconds\n", 
				    (uint32_t) (currentTime / (86400UL * OS_CLOCK_FREQ)),
				   (uint32_t) ((currentTime % (86400UL * OS_CLOCK_FREQ)) / (3600UL * OS_CLOCK_FREQ)), 
				   (uint32_t) ((currentTime % (3600UL * OS_CLOCK_FREQ)) / (60UL * OS_CLOCK_FREQ)), 
				   (uint32_t) ((currentTime % (60UL * OS_CLOCK_FREQ))) / OS_CLOCK_FREQ);
			fprintf(&uart, "Measurement period:\t%d ms\n", (uint32_t) (currentTime - lastRun) * 1000 / OS_CLOCK_FREQ);
			fprintf(&uart, "MCU clock frequency:\t%d Hz\n", SYSTEM_CLOCK_FREQ);
			fprintf(&uart, "KrisOS clock frequency:\t%d Hz\n", OS_CLOCK_FREQ);
			fprintf(&uart, "Context switches:\t%d\n", scheduler.contextSwitchNo);
			fprintf(&uart, "Tasks:\t\t\t%d\n", scheduler.totalTaskNo);
			
			#ifdef USE_MUTEX
				fprintf(&uart, "Mutexes:\t\t%d\n", KrisOS.totalMutexNo);
			#endif
			
			#ifdef USE_SEMAPHORE
				fprintf(&uart, "Semaphores:\t\t%d\n", KrisOS.totalSemNo);
			#endif
			
			#ifdef USE_QUEUE
				fprintf(&uart, "Queues:\t\t\t%d\n", KrisOS.totalQueueNo);
			#endif
			
			#ifdef USE_HEAP
				fprintf(&uart, "Heap usage:\t\t%dB/%dB = %d%%\n", heap.heapBytesUsed, HEAP_SIZE, 
					    heap.heapBytesUsed * 100 / HEAP_SIZE);
			#endif
			
			#ifdef USE_MUTEX
				fprintf(&uart, "Max mutex lock time:\t%d 'ticks'\n", KrisOS.maxMtxCriticalSection);
			#endif

			// Display the task manager (per-task statistics)
			fprintf(&uart, "\nTID\tCPU usage\tStack usage\tPriority\tStatus\t\tMemory\n");
			for (index = 0; index < scheduler.totalTaskNo; index++) {
				iterator = scheduler.taskRegistry[index];
				
				// Compute the CPU usage as the proportion of the OS ticks spent executing the task
				// to the total number of OS ticks since the last time the statistics task was run
				cpuUsage = (float32_t) iterator->cpuUsage * 100.f / (currentTime - lastRun);
				cpuUsageInt = (uint32_t) cpuUsage;
				cpuUsageFrac = (uint32_t) ((cpuUsage - cpuUsageInt) * 100.0f);
				
				// Estimate the stack usage by calculating the offset from the stack base to the first
				// memory location that hasn't been modified
				stackUsageHelper = (uint32_t*) iterator->sp;
				while (*stackUsageHelper-- != 0xDEADBEEF);
				if (iterator->memoryType == DYNAMIC) 
					stackUsage = iterator->stackSize - ((stackUsageHelper - iterator->stackBase) << 2);
				else 
					stackUsage = (iterator->stackBase - stackUsageHelper) << 2;
					
				// Display the per-task stats and reset the CPU usage counter
				if (iterator->id == -2) 
					fprintf(&uart, "%d\tN/A\t\t%dB\t\t%d\t\t", iterator->id, stackUsage, 
							iterator->priority);
				else
					fprintf(&uart, "%d\t%d.%d%%\t\t%dB\t\t%d\t\t", iterator->id, cpuUsageInt, 
							cpuUsageFrac, stackUsage, iterator->priority);
				iterator->cpuUsage = 0;
				
				// Display the current task status 
				switch(iterator->status) {
					case RUNNING: fprintf(&uart, "RUNNING\t\t"); break;
					case READY: fprintf(&uart, "READY\t\t"); break;
					case SLEEPING: fprintf(&uart, "SLEEPING\t"); break;
					case MTX_WAIT: fprintf(&uart, "MUTEX WAIT\t"); break;
					case SEM_WAIT: fprintf(&uart, "SEM WAIT\t"); break;
					case REMOVED: fprintf(&uart, "REMOVED\t"); break;
					default: break;
				}

				// Display the type of memory allocation used for the task and its stack (static/dynamic)
				switch(iterator->memoryType) {
					case STATIC: fprintf(&uart, "Static\n"); break;
					case DYNAMIC: fprintf(&uart, "Dynamic\n"); break;
					default: break;
				}
			}
			fprintf(&uart, "------------------------------------------------------------------------------\n");
		}
		#ifdef USE_MUTEX
			mutex_unlock(&uartMtx);
		#endif
	}
}
	
#endif

