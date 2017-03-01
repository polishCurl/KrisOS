/*******************************************************************************
* File:     	scheduler.c
* Brief:    	Task scheduler implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/10/2016
* Last mod: 	09/12/2016
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"
#include "kernel.h"
#include "system.h"



/*-----------------------------------------------------------------------------
* Declare the user tasks
------------------------------------------------------------------------------*/
KrisOS_task_static_template(idle, 100, UINT8_MAX)
#ifdef SHOW_DIAGNOSTIC_DATA
	KrisOS_task_static_template(stats, 512, DIAG_DATA_PRIO)
#endif



/*-------------------------------------------------------------------------------
* Scheduler declaration
--------------------------------------------------------------------------------*/
Scheduler scheduler;



/*-------------------------------------------------------------------------------
* Function:    	scheduler_init
* Purpose:    	Initialise the scheduler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_init(void) {
	
	// Initialise the scheduler queues
	scheduler.blocked = scheduler.ready = NULL;
	
	// Start assigning task IDs from 1. (Actually +-1 as system tasks have negative 
	// IDs and user tasks have positive IDs
	scheduler.lastIDUsed = 1;
	
	// Reset the task counter and preprocess the idle task's stack memory for
	// stack usage estimation. 
	#ifdef SHOW_DIAGNOSTIC_DATA
		scheduler.totalTaskNo = 0;
		KrisOS_task_stack_usage((uint32_t*) &idleStack[0], idleStackSize);
		
	#endif	
	
	// Create the idle task
	task_create_static(&idleTask, idle, &idleStack[idleStackSize], idlePriority, 1);
	
	// Create the KrisOS usage monitor task 
	#ifdef SHOW_DIAGNOSTIC_DATA
		KrisOS_task_stack_usage((uint32_t*) &statsStack[0], statsStackSize);
		task_create_static(&statsTask, stats, &statsStack[statsStackSize], 
						   statsPriority, 1);
	#endif
		
	// Set the runPtr to the idle task
	scheduler.runPtr = &idleTask;
	
	// Context switching should give way to other events (interrupt handlers). Thus, 
	// PendSV interrupts, where context switching occurs, should have the lowest priority
	nvic_set_priority(PendSV_IRQn, 7);
}



/*-------------------------------------------------------------------------------
* Function:    	scheduler_run
* Purpose:    	Run the scheduler to determine the next task to run.
* Arguments:	-
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t scheduler_run(void) {
	
	__start_critical();
	{
		// Pick either the top priority ready task or (if the time-sliced preemption 
		// flag is set) the task next in queue with respect to the currently running one
		if (scheduler.preemptFlag && scheduler.runPtr->next->priority == scheduler.runPtr->priority)
			scheduler.topPrioTask = scheduler.runPtr->next;
		else
			scheduler.topPrioTask = scheduler.ready;
		
		// Perform context-switch only if the next task to run is different from the 
		// current one, according to the scheduling policy
		if (scheduler.topPrioTask != scheduler.runPtr) {
			SCB->ICSR |= (1 << PENDSV);	
			
			// Update current task's state only if it hasn't been updated yet due to some request.
			// (Sleep/Mutex wait/...)
			if (scheduler.runPtr->status == RUNNING)
				scheduler.runPtr->status = READY;
			scheduler.topPrioTask->status = RUNNING; 
			
			// The current time-slice will now be divided between more than one task so 
			// time-sliced preemption is switched off until the next time slice is entered
			scheduler.preemptFlag = 0;
			
			// Update the context switch counter
			#ifdef SHOW_DIAGNOSTIC_DATA
				scheduler.contextSwitchNo++;
			#endif
		}
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	scheduler_wake_tasks
* Purpose:    	Check which tasks are ready, and wake them up
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_wake_tasks(void) {
	
	// Pointer to the task to wake
	Task* toWake;
	
	// Semaphore for which a task was waiting (with timout)
	#ifdef USE_SEMAPHORE
		Semaphore* semWaitedFor;
	#endif
	
	__start_critical();
	{
		// Go through the delayed queue until a 'not ready' task is encountered (task are 
		// sorted in ascending delay time)
		while (scheduler.blocked != NULL && scheduler.blocked->waitCounter <= KrisOS.ticks) {
			
			// Remove the ready task from delayed queue and reset its wait counter
			toWake = scheduler.blocked;
			task_remove(&scheduler.blocked, toWake);
			toWake->waitCounter = 0;
			
			#ifdef USE_SEMAPHORE
				if (toWake->status == SEM_WAIT) {
					semWaitedFor = toWake->waitingObj;
					task_remove(&semWaitedFor->waitingQueue, toWake); 
					toWake->waitingObj = NULL;
				}
			#endif
			
			// Insert the task back to the ready queue
			toWake->status = READY;
			task_add(&scheduler.ready, toWake);
		}
		// Reschedule tasks as the state of ready queues have changed
		scheduler_run();
	}
	__end_critical();
}



#ifdef USE_HEAP
/*-------------------------------------------------------------------------------
* Function:    	task_create_dynamic
* Purpose:    	Create a task using heap and add it to the ready queue
* Arguments:	
*		startAddr - starting address of the task to add
*		stackSize - stack size (in bytes) required by the task
*		priority - the higher the number the lower the priority
*		isPrivileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		
*		pointer to the task created
--------------------------------------------------------------------------------*/
Task* task_create_dynamic(void* startAddr, uint32_t stackSize, uint32_t priority,
						  uint32_t isPrivileged) {
	
	// Pointer to the task to create
	Task* toCreate;				 
						 
	// Test if the starting address of the task code is valid
	TEST_NULL_POINTER(startAddr)
	
	// Adjust the stack size to comply with the stack alignment
	if (stackSize % STACK_ALIGNMENT)
		stackSize = stackSize + (STACK_ALIGNMENT - stackSize % STACK_ALIGNMENT);
	
	// Allocate memory for the task control block. 
	toCreate = malloc(sizeof(Task));
	TEST_NULL_POINTER(toCreate)
	
	// Allocate memory for the task's private stack
	toCreate->stackBase = malloc(stackSize);
	TEST_NULL_POINTER(toCreate->stackBase)
	toCreate->sp = ((uint32_t) toCreate->stackBase) + stackSize - (STACK_FRAME_SIZE << WORD_ACCESS_SHIFT);
	
	#ifdef SHOW_DIAGNOSTIC_DATA
		// Indicate that the task is placed on heap and store the stack size
		toCreate->memoryType = DYNAMIC;
		toCreate->stackSize = stackSize;
	
		// Preprocess the stack memory for debugging purpose (estimating stack usage)
		KrisOS_task_stack_usage(toCreate->stackBase, stackSize);
	#endif
	
	// Initialise the task's control block and stack frame
	task_init(toCreate, startAddr, isPrivileged, priority);
	
	// Return the pointer tu the task created
	return toCreate;
}
#endif



/*-------------------------------------------------------------------------------
* Function:    	task_create_static
* Purpose:    	Create a task using statically allocated memory
* Arguments:	
* 		toDeclare - pointer to the pre-allocated task control block of the task to declare
*		startAddr - starting address of the task to add
*		stackBase - pointer to the private static stack area to be used by the task
*		priority - task priority
*		isPrivileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_create_static(Task* toDeclare,  void* startAddr, void* stackBase, 
					        uint32_t priority, uint32_t isPrivileged) {
						 				 
	// Test if input arguments are valid
	TEST_NULL_POINTER(toDeclare)
	TEST_NULL_POINTER(stackBase)
	TEST_NULL_POINTER(startAddr)
	
	// Attach the stack to the task control block of the task to declare and set the SP value
	toDeclare->stackBase = stackBase;
	toDeclare->sp = ((uint32_t) toDeclare->stackBase) - (STACK_FRAME_SIZE << WORD_ACCESS_SHIFT);
								
	// Indicate that the task is statically allocated
	#ifdef SHOW_DIAGNOSTIC_DATA
		toDeclare->memoryType = STATIC;
	#endif

	// Initialise the task's control block and stack frame
	task_init(toDeclare, startAddr, isPrivileged, priority);
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_sleep
* Purpose:    	Suspend the execution of the running task for specified amount of
* 				OS ticks.
* Arguments: 	
*		delay - number of OS 'ticks' do suspend execution of the task by
*		waitState - task state for the time it is temporarily suspended
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_sleep(uint64_t delay, TaskState state) {
	
	// Iterators trough the delayed queue and a pointer to the task to delay
	Task *iterator, *previous, *toDelay;
	
	__start_critical();
	{	
		// Release any locks the calling tasks has		
		#ifdef USE_MUTEX 	
			mutex_release_all(scheduler.runPtr);
		#endif		
		
		// Only the calling task (currently executing) can delay itself. Remove the
		// running task from the queue it belongs to
		toDelay = scheduler.runPtr;
		task_remove(&scheduler.ready, toDelay);
		
		// Update the wait counter and reschedule tasks
		toDelay->waitCounter = delay == TIME_INFINITY ? UINT64_MAX : KrisOS.ticks + delay;
		toDelay->status = state;
		scheduler_run();
		
		// Insert the task to the queue with delayed tasks. Insertion sort is 
		// performed in ascending delay value order.
		toDelay->next = toDelay->previous = NULL;
		
		// The queue is empty
		if (scheduler.blocked == NULL) {
			scheduler.blocked = toDelay;
		}
		// The task to insert to the delayed queue will be the soonest to wake
		else if (toDelay->waitCounter <= scheduler.blocked->waitCounter) {
			toDelay->next = scheduler.blocked;
			scheduler.blocked->previous = toDelay;
			scheduler.blocked = toDelay;
		}
		// Iterate through the queue until the right spot is found (ascending delay
		// value order)
		else {
			iterator = scheduler.blocked;
			while (iterator != NULL && toDelay->waitCounter > iterator->waitCounter) {
				previous = iterator;
				iterator = iterator->next;
			}
			toDelay->previous = previous;
			toDelay->next = iterator;
			previous->next = toDelay;
			if (iterator != NULL)
				iterator->previous = toDelay;
		}
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_delete
* Purpose:    	Permanently remove the calling task.
* Arguments: 	-
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_delete(void) {
	
	Task* toDelete;
	__start_critical();
	{	
		// Task registry iterator
		#ifdef SHOW_DIAGNOSTIC_DATA	
			uint32_t index = 0;
		#endif
		
		// Remove the calling task from the list of ready tasks and update its state
		toDelete = scheduler.runPtr;
		task_remove(&scheduler.ready, toDelete);
		toDelete->status = REMOVED;
		
		// Update the total number of tasks in the scheduler and remove the task to delete
		// from the task registry
		#ifdef SHOW_DIAGNOSTIC_DATA	
			while (index < scheduler.totalTaskNo && scheduler.taskRegistry[index] != toDelete)
				index++;
			
			scheduler.taskRegistry[index] = scheduler.taskRegistry[--scheduler.totalTaskNo];
		#endif
		
		// Release any locks the calling tasks has		
		#ifdef USE_MUTEX 	
			mutex_release_all(toDelete);
		#endif	
		
		// Free the heap memory the task occupies (if any)
		#ifdef USE_HEAP
			free(toDelete->stackBase);
			free(toDelete);
		#endif
		
		scheduler_run();
	}
	__end_critical();	
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_add
* Purpose:    	Add the task given to the queue specified in descending priority
* 				order.
* Arguments: 	
*		queue - queue to update
* 		toInsert - task to insert
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_add(Task** queue, Task* toInsert) {
	
	// Iterators through the queue to update
	Task* iterator;
	Task* previous;
	
	__start_critical();
	{
		// The queue is empty
		if (*queue == NULL) {
			toInsert->next = toInsert->previous = NULL;
			*queue = toInsert;
		}
		// The task to insert has the hightest priority (is inserted at the beginnig)
		else if (toInsert->priority <= (*queue)->priority) {
			toInsert->next = *queue;
			toInsert->previous = NULL;
			(*queue)->previous = toInsert;
			*queue = toInsert;
		}
		// Iterate through the queue until a lower priority task is encountered
		else {
			iterator = *queue;
			while (iterator != NULL && toInsert->priority > iterator->priority) {
				previous = iterator;
				iterator = iterator->next;
			}
			toInsert->previous = previous;
			toInsert->next = iterator;
			previous->next = toInsert;
			if (iterator != NULL)
				iterator->previous = toInsert;
		}
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_remove
* Purpose:    	Remove the task given from the queue specified
* Arguments: 
*		queue - queue to update
* 		toRemove - task to remove
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_remove(Task** queue, Task* toRemove) {

	__start_critical();
	{	
		// Join the next and previous tasks together
		if (toRemove->previous != NULL)
			toRemove->previous->next = toRemove->next;
		if (toRemove->next != NULL)
			toRemove->next->previous = toRemove->previous;
		
		// If the task to remove is at the start of the queue then update the queue head 
		if (*queue == toRemove) 
			*queue = (*queue)->next;
		
		toRemove->next = toRemove->previous = NULL;
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_init
* Purpose:    	Initialise the task control block and stack frame for the task specified
* Arguments: 	
*		toInit - pointer to the task to have its stack frame initialised
*		startAddr - address of the first instruction of the task to initialise
*		isPrivileged - task's priviliged access level flag
*		priority - priority of the task to initialise
* Returns: 
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_init(Task* toInit, void* startAddr, uint32_t isPrivileged, uint32_t priority) {
	
	// Helper pointer for specifying initial register values in the stack frame
	uint32_t* taskFramePtr; 
	
	// Assign the task's priority, id, status and sleep time. System tasks have negative
	// IDs while user ones have positive IDs. Reset the waitCounter. Finally, set the 
	// initial stack pointer value
	toInit->basePrio = toInit->priority = priority;
	toInit->status = READY;
	toInit->id = isPrivileged ? -scheduler.lastIDUsed : scheduler.lastIDUsed;
	scheduler.lastIDUsed++;
	toInit->waitCounter = 0;
	toInit->waitingObj = NULL;
	
	// Initialise the mutex info - mutexes held by task initialised 
	#ifdef USE_MUTEX
		toInit->mutexHeld = NULL;
	#endif

	// Set the initial PC to the starting address of task's code
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_PC << WORD_ACCESS_SHIFT));	
	*taskFramePtr = (uint32_t) startAddr;
	
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_LR << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = (uint32_t) task_complete_handler;
	
	// Set the initial value of xPSR
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_xPSR << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = INIT_xPSR;
	
	// Set the initial value of EXC_RETURN, which specifies the use of stack pointer PSP or MSP and floating 
	// point unit before and after return from this exception
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_EXC_RETURN << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = EXC_RETURN_2;
	
	// Set the initial value of control register according to privilege level. This will
	// specify access level of the task
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_CONTROL << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = isPrivileged ? 0x2 : 0x3;
	
	__start_critical();
	{
		// Update the total number of tasks registered at the scheduler and reset the CPU usage counter
		#ifdef SHOW_DIAGNOSTIC_DATA	
			scheduler.taskRegistry[scheduler.totalTaskNo++] = toInit;
			toInit->cpuUsage = 0;
		#endif
		
		// Insert the task to the ready queue in descending priority order and reschedule
		// task if OS is already running
		task_add(&scheduler.ready, toInit);
		if (KrisOS.isRunning)
			scheduler_run();
	} 
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_complete_handler
* Purpose:    	Code to be executed when the running task returns. 
* Arguments: 	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void task_complete_handler(void) {
	KrisOS_task_delete();
}



#ifdef SHOW_DIAGNOSTIC_DATA
/*-------------------------------------------------------------------------------
* Function:    	KrisOS_task_stack_usage
* Purpose:    	Reset the stack memory given in order to extract stack usage data later
* Arguments:	
*		toPrepare - top of the stack memory to reset
* 		size - size of stack memory
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t KrisOS_task_stack_usage(uint32_t* toPrepare, uint32_t size) {
	
	uint32_t* iterator;
	uint32_t* endAddress;
	
	// Iterate through each word in the given memory area and initialise it to some 
	// known value. This is later useful for stack usage computation
	iterator = toPrepare;
	endAddress = toPrepare + size / 4;
	while (iterator < endAddress)
		*iterator++ = 0xDEADBEEF;
	
	return EXIT_SUCCESS;
}
#endif

