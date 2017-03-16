/*******************************************************************************
* File:     	os_tasks.c
* Brief:    	System (priviliged) tasks created by KrisOS
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	07/02/2017
* Last mod: 	14/03/2017
*
* Note: 		
*	There are two OS (system) tasks registered by the KrisOS at the scheduler
*		1. The idle task - crucial to the KrisOS scheduler implementation and
*		   it is run whenever there is no other processing to do (no other
*		   task is ready).
*		2. The stats task - optional, can be disabled if no internal OS perfomance
*		   data is needed. However, this task is very useful at the application
*		   development/debug stage as it provides a number of useful figures 
*		   describing the behaviour of each task as well as other KrisOS 
*		   functionality.
*
*	A list of statistic and performance data mainained by KrisOS 'stats' task
*		1. Total time KrisOS has been running for
*		2. Measurement period - amount of time for which the usage is gathered.
*		   Specified how frequently the updated data is displayed.
*		3. System clock frequency - what is the MCU clock frequency
*		4. OS clock frequency - KrisOS time resolution, the frequency of the 
*		   OS timer which is the main source of OS timekeeping
*		5. Context switches - number of context switches which occurred during
*		   the measurement period
*		6. Total number of tasks registered at the scheduler (both user and system)
*		7. Total number of Semaphores, Mutexes and Queues initialised. These
*		   figures are displayed per data structure type, provided that a given
*		   OS feature is enabled.
*		8. Maximum Mutex lock time - if mutexes are enabled, the length (in OS 'ticks')
*		   of each critical section guarded by a mutual exclusion lock is compared
*		   with the current maximum lock time and if necessary the maximum figure
*		   is updated.
*		9. Heap usage (if heap manager is enabled) 
*		10.Per task data:
*			A. Task ID
*			B. CPU usage - proportion of time since the last stats data display
*			   the given task has been running
*			C. Stack usage - the maximum stack depth. Useful for determining the
*			   size of private task's stack necessary for the correct task operation.
*			D. Task priority - the larger the number the lower the priority
*			E. Currect task status - is the task currently suspended, ready to run,
*			   running, or maybe waiting blocked by a syncronisation structure.
*			F. Type of memory allocation - has the task been created dynamically
*			or statically (memory allocated at compile time)
*******************************************************************************/
#include "common.h"
#include "kernel.h"



/*******************************************************************************
* Task: 	idle
* Purpose: 	The idle task. Lowest priority task used for power saving when no other 
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
	// point representation. The <stdio.h> library for embedded systems is often 
	// stripped, and %f doesn't work when used with fprintf. That's why some 
	// floating point arithmetic is used for computing CPU usage but later that
	// number is converted to two fixed point numbers - the integer and fractional
	// part.
	uint32_t cpuUsageInt;
	uint32_t cpuUsageFrac;
	float32_t cpuUsage;
	
	// The last time the statistics task was run and the current OS timer value
	uint64_t lastRun;
	uint64_t currentTime;
	
	// Helper variables for iterating through the task registry
	int32_t index;
	Task* iterator;
	
	// Helper variables for estimating task's stack usage
	uint32_t* stackUsageHelper;
	uint32_t stackUsage;
	
	// Reset the CPU usage counters before the data can be collected
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
			fprintf(&uart, "\n------------------------------------------------------------------------------\n");
			
			// Displaying the elapsed time since the OS has been launched in days, hours, 
			// minutes and seconds format required a bit of processing of the total 
			// elapsed time (which is in OS 'ticks)
			fprintf(&uart, "Time running:\t\t%d days, %d hours, %d minutes, %d seconds\n", 
				    (uint32_t) (currentTime / (86400UL * OS_CLOCK_FREQ)),
				   (uint32_t) ((currentTime % (86400UL * OS_CLOCK_FREQ)) / (3600UL * OS_CLOCK_FREQ)), 
				   (uint32_t) ((currentTime % (3600UL * OS_CLOCK_FREQ)) / (60UL * OS_CLOCK_FREQ)), 
				   (uint32_t) ((currentTime % (60UL * OS_CLOCK_FREQ))) / OS_CLOCK_FREQ);
			
			// Display OS usage data
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

			// Display the task manager (per-task statistics) using the task registry.
			// Here the purpose os task registry is revealed. It keeps track of all active
			// tasks regardless of their current state and the queue they are in.
			fprintf(&uart, "\nTID\tCPU usage\tStack usage\tPriority\tStatus\t\tMemory\n");
			for (index = 0; index < scheduler.totalTaskNo; index++) {
				
				// Get the next task from the registry.
				iterator = scheduler.taskRegistry[index];
				
				// Compute the CPU usage as the proportion of the OS ticks spent executing the task
				// to the total number of OS ticks since the last time the statistics task was run
				cpuUsage = (float32_t) iterator->cpuUsage * 100.f / (currentTime - lastRun);
				cpuUsageInt = (uint32_t) cpuUsage;
				cpuUsageFrac = (uint32_t) ((cpuUsage - cpuUsageInt) * 100.0f);
				
				// Estimate the stack usage by calculating the offset from the task's stack base 
				// to the first memory location that hasn't been modified (different than 0xDEADBEEF)
				// The stack usage is estimated slightly differently for STATIC and DYNAMIC task
				// which is only the limitation of my current KrisOS implementation. The difference
				// is caused by the 'stackBottom' being set to the highest stack memory address
				// for statically allocated tasks and to the lowest stack memory adddress for
				// tasks located on heap.
				stackUsageHelper = (uint32_t*) iterator->sp;
				while (*stackUsageHelper-- != 0xDEADBEEF);
				if (iterator->memoryType == DYNAMIC) 
					stackUsage = iterator->stackSize - ((stackUsageHelper - iterator->stackBottom) << 2);
				else 
					stackUsage = (iterator->stackBottom - stackUsageHelper) << 2;
					
				// Display the Task ID, CPU ussage and Stack usage for the given task.
				// If the task is the OS usage stats one (currently running) then the 
				// CPU usage data is hard to obtain accurately as at the same time the 
				// CPU usage figure is displayed it is also updated. I decided not to
				// include the CPU figure for this task.
				if (iterator->id == -2) 
					fprintf(&uart, "%d\tN/A\t\t%dB\t\t%d\t\t", iterator->id, stackUsage, 
							iterator->priority);
				else
					fprintf(&uart, "%d\t%d.%d%%\t\t%dB\t\t%d\t\t", iterator->id, cpuUsageInt, 
							cpuUsageFrac, stackUsage, iterator->priority);
				
				// Reset the CPU usage counter
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

				// Display the type of memory allocation used for the task and its
				// stack (static/dynamic)
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

