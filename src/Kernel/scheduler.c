/*******************************************************************************
* File:     	scheduler.c
* Brief:    	Task scheduler implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/10/2016
* Last mod: 	16/03/2017
*
* Note: 		
*	The KrisOS scheduler is a priority preemptive scheduler. This means that, 
*	whenever a higher priority task becomes ready, it preempts the currently running
*	one. If two tasks have the same priority, the one that becomes ready (if it wasn't
*	before, will preempt the other (executing task). If two tasks of the same 
*	priority run uninterrupted until completion then time-sliced preemption is 
*	applied with the time slice length of TIME_SLICE OS 'ticks;
*
*	All task queues are arranged into doubly linked lists for constant removal
*	time. The status of a task is reflected by the queue it belongs to:
*		1. ready queue - a task is waiting for its turn to get CPU time
*		2. blocked queue - a task is temporarily/pernamently suspended and won't
*		get a chance to resume execution until the wait timout is reached
*		3. mutex wait queue - a task is waiting to access a shared resource
*		which is currently occupied by some other task
*		4. semaphore wait queue - a task is waiting for a semaphore counter value
*		to be positive, so that it can proceed.
*
*	Every queue (except from the blocked one) is sorted in descending task priority order.
*	This implies that basic re-scheduling takes contant time, as it requires
*	taking the first task located at the ready queue head. The lower the task
*	priority the longer the insertion time. The blocked queue is arranged in ascending
*	'wait deadline' value. This means that on each OS timer interrupt, only if the first
*	task in the queue has become ready, then the scheduler has to further investigate
*	the blocked queue, but usually not the whole of it (because of sorting).
*
*	The currently running task doesn't have a separate queue for itself. It is also
*	located in the ready queue. If a task is removed, it is permanently deregistered 
*	from the scheduler.
*
*	The tasks which terminate (don't run forever) are dealt with by the 
* 	task_complete_handler which removes the task which returns from the scheduler
*	and deletes it if allocated dynamically.
*
*	If the SHOW_DIAGNOSTIC_DATA option is enabled, an extra task registry is used
*	to keep track of all the active tasks, no matter which queue they are in.
*	This solves the problem of tracking all the tasks between various data structures
*	such as semaphores/mutexes which can be created or removed at runtime.
*	These primitives (semaphores/mutexes) aren't directly connected with the scheduler,
*	so an extra array is used to store information about all the currently active
*	tasks. This is easier than registering the KrisOS synchronisation and communication
*	primitives instead. The TASK_REGISTRY_SIZE can be controlled, but nevertheless,
*	it imposes an explicit limit on the number of tasks created. However, if the 
*	SHOW_DIAGNOSTIC_DATA is disabled (after all debugging and performance analysis)
*	then there is no limit on the number of tasks created except for the available
*	RAM memory.
*******************************************************************************/
#include "KrisOS.h"
#include "kernel.h"
#include "system.h"



/*-----------------------------------------------------------------------------
* Declare the user tasks
------------------------------------------------------------------------------*/
KrisOS_task_static_template(idle, 256, UINT8_MAX)
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
		// Pick either the top priority ready task (first in the ready queue) or 
		// (if the time-sliced preemption flag is set) the task next in queue with 
		// respect to the currently running one, if its priority is equal to the
		// one of the running task
		if (scheduler.preemptFlag && scheduler.runPtr->next->priority == scheduler.runPtr->priority)
			scheduler.topPrioTask = scheduler.runPtr->next;
		else
			scheduler.topPrioTask = scheduler.ready;
		
		// Perform context-switch only if the next task to run is different from the 
		// current one, according to the scheduling policy
		if (scheduler.topPrioTask != scheduler.runPtr) {
			SCB->ICSR |= (1 << PENDSV);	
			
			// Update current task's state only if it hasn't been updated yet due to some request.
			// (Sleep/Mutex wait/...). There are various reasons to reschedule tasks.
			// And if that reason hasn't been specified earlier (status of the last running 
			// task is still 'RUNNING' then it has to be set to 'READY'
			if (scheduler.runPtr->status == RUNNING)
				scheduler.runPtr->status = READY;
			scheduler.topPrioTask->status = RUNNING; 
			
			// The current time-slice will now be divided between more than one task so 
			// time-sliced preemption is switched off until the next time slice is entered
			// (inside the SysTick IRQ handler)
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
* Purpose:    	Wake all the tasks from the blocked queue which are now ready
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_wake_tasks(void) {
	
	// Pointer to the task to wake
	Task* toWake;
	
	__start_critical();
	{
		// Go through the blocked queue until a 'not ready' task is encountered (task are 
		// sorted in ascending delay time, so as soon as we encounter a task with wait
		// counter greated than current OS 'ticks' value then the search for task
		// to wake is over.
		while (scheduler.blocked != NULL && scheduler.blocked->waitCounter <= KrisOS.ticks) {
			
			// Remove the ready task from delayed queue and reset its wait counter
			toWake = scheduler.blocked;
			task_remove(&scheduler.blocked, toWake);
			toWake->waitCounter = 0;
			
			// Insert the task back to the ready queue
			toWake->status = READY;
			task_add(&scheduler.ready, toWake);
		}
		// Reschedule tasks as the state of ready queue has changed
		scheduler_run();
	}
	__end_critical();
}



#ifdef USE_HEAP
/*-------------------------------------------------------------------------------
* Function:    	task_create_dynamic
* Purpose:    	Create a task using heap and add it to the ready queue
* Arguments:	
*		startAddr - pointer to the task code (function)
*		stackSize - size of the task's private stack (in bytes)
*		priority - task priority. The higher the number the lower the priority.
*		isPrivileged - 1 if the task to create should be priviliged, 0 otherwise
* Returns: 		
*		pointer to the task created
--------------------------------------------------------------------------------*/
Task* task_create_dynamic(void* startAddr, size_t stackSize, uint8_t priority,
						  uint8_t isPrivileged) {
	
	// Pointer to the task to create
	Task* toCreate;				 
						 
	// Validate input arguments
	TEST_NULL_POINTER(startAddr)
	TEST_INVALID_SIZE(stackSize)
	
	// Allocate memory for the task control block. 
	toCreate = malloc(sizeof(Task));
							  
	// Adjust the stack size to comply with the double-word stack alignment
	if (stackSize % STACK_ALIGNMENT)
		stackSize = stackSize + (STACK_ALIGNMENT - stackSize % STACK_ALIGNMENT);

	// Allocate memory for the task's private stack memory and set its SP intial
	// value
	toCreate->stackBottom = malloc(stackSize);
	toCreate->sp = ((uint32_t) toCreate->stackBottom) + stackSize - (STACK_FRAME_SIZE << 2);
	
	#ifdef SHOW_DIAGNOSTIC_DATA
		// Record the type of memory allocation used for the creation of this task and
		// save the stack size inside its task control block
		toCreate->memoryType = DYNAMIC;
		toCreate->stackSize = stackSize;
	
		// Preprocess the stack memory for debugging purpose (estimating stack usage)
		KrisOS_task_stack_usage(toCreate->stackBottom, stackSize);
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
*		startAddr - pointer to the task code (function)
*		stackBottom - pointer to the private static stack area to be used by the task
*		priority - task priority. The higher the number the lower the priority.
*		isPrivileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_create_static(Task* toDeclare,  void* startAddr, void* stackBottom, 
					        uint8_t priority, uint8_t isPrivileged) {
						 				 
	// Validate input arguments
	TEST_NULL_POINTER(toDeclare)
	TEST_NULL_POINTER(stackBottom)
	TEST_NULL_POINTER(startAddr)
	
	// Attach the stack to the task control block of the task to declare and set the SP value
	toDeclare->stackBottom = stackBottom;
	toDeclare->sp = ((uint32_t) toDeclare->stackBottom) - (STACK_FRAME_SIZE << 2);
								
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
* Purpose:    	Suspend the execution of the currently running task for 
* 				specified amount of OS ticks.
* Arguments: 	
*		delay - number of OS 'ticks' do suspend execution of the task by
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_sleep(uint64_t delay) {
	
	// Iterators trough the delayed queue and a pointer to the task to delay
	Task *iterator, *previous, *toDelay;
	
	__start_critical();
	{			
		// Only the currently executing task can delay itself. So, remove the
		// calling task from the ready queue
		toDelay = scheduler.runPtr;
		task_remove(&scheduler.ready, toDelay);
		
		// Release a (potential) lock the calling tasks might own		
		#ifdef USE_MUTEX 	
			mutex_unlock(toDelay->mutexHeld);
		#endif		
		
		// Update the wait counter and the task status. If the task is suspended
		// without a timout, set its waitCounter to the maximum value possible
		toDelay->waitCounter = delay == TIME_INFINITY ? UINT64_MAX : KrisOS.ticks + delay;
		toDelay->status = SLEEPING;
		
		// The state of the ready queue has changed so rescheduling is necessary
		scheduler_run();
		
		// Insert the task to the queue with delayed tasks. Insertion sort is 
		// performed in ascending 'waitCounter' value order.
		toDelay->next = toDelay->previous = NULL;
		
		// The queue is empty case:
		if (scheduler.blocked == NULL) {
			scheduler.blocked = toDelay;
		}
		// The task to insert to the delayed queue will be the soonest to wake,
		// inserted at the beginning case:
		else if (toDelay->waitCounter <= scheduler.blocked->waitCounter) {
			toDelay->next = scheduler.blocked;
			scheduler.blocked->previous = toDelay;
			scheduler.blocked = toDelay;
		}
		// Iterate through the queue until the right spot is found 
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
* Purpose:    	Remove the currently running (calling) task from the scheduler and
*				delete it, if allocated dynamically.
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
		
		// Release a (potential) lock the calling tasks might own			
		#ifdef USE_MUTEX 	
			mutex_unlock(toDelete->mutexHeld);
		#endif	
		
		// Free the heap memory the task occupies (if any)
		#ifdef USE_HEAP
			free(toDelete->stackBottom);
			free(toDelete);
		#endif
		
		// The state of the ready queue has changed so rescheduling is necessary
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
		// The queue is empty case:
		if (*queue == NULL) {
			toInsert->next = toInsert->previous = NULL;
			*queue = toInsert;
		}
		// The task to insert has the hightest priority (is inserted at the beginnig)
		// case:
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
		// Join the neighbours of the task to delete together
		if (toRemove->previous != NULL)
			toRemove->previous->next = toRemove->next;
		if (toRemove->next != NULL)
			toRemove->next->previous = toRemove->previous;
		
		// If the task to remove is at the start of the queue then update the queue head 
		if (*queue == toRemove) 
			*queue = (*queue)->next;
		
		// Make sure that the task to remove don't have any dangling pointers
		toRemove->next = toRemove->previous = NULL;
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_init
* Purpose:    	Initialise the task control block and stack frame for the task specified
* Arguments: 	
*		toInit - pointer to the task struct to have the stack frame initialised
*		startAddr - address of the first instruction of the task to initialise
*		isPrivileged - 1 if the task to initialise should be priviliged, 0 otherwise
*		priority - priority of the task to initialise
* Returns: 
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_init(Task* toInit, void* startAddr, uint8_t isPrivileged, uint8_t priority) {
	
	// Helper pointer for specifying an address within the task's stack where specific
	// register values are stored
	uint32_t* taskFramePtr; 
	
	// Initialise the fields in the task control block
	toInit->basePrio = toInit->priority = priority;
	toInit->status = READY;
	toInit->waitCounter = 0;
	toInit->waitingObj = NULL;
	
	// Initially tasks don't own any mutual exclusion locks
	#ifdef USE_MUTEX
		toInit->mutexHeld = NULL;
	#endif
	
	// System tasks have negative IDs while user ones have positive IDs.
	toInit->id = isPrivileged ? -scheduler.lastIDUsed : scheduler.lastIDUsed;
	scheduler.lastIDUsed++;

	// Set the initial PC register value to the starting address of task's code
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_PC << 2));	
	*taskFramePtr = (uint32_t) startAddr;
	
	// Set the initial LR register value to the method for handling completed
	// tasks, the ones which have returned
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_LR << 2));				   
	*taskFramePtr = (uint32_t) task_complete_handler;
	
	// Set the initial value of xPSR register (see scheduler.h)
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_xPSR << 2));				   
	*taskFramePtr = INIT_xPSR;
	
	// Set the initial value of EXC_RETURN (see scheduler.h)
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_EXC_RETURN << 2));				   
	*taskFramePtr = EXC_RETURN_2;
	
	// Set the initial value of CONTROL register according to isPriviliged flag. 
	// This will specify access level of the task.
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_CONTROL << 2));				   
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
* Purpose:    	Code to be executed should a task return
* Arguments: 	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void task_complete_handler(void) {
	
	// Self-deletion of completed task 
	KrisOS_task_delete();
}



#ifdef SHOW_DIAGNOSTIC_DATA
/*-------------------------------------------------------------------------------
* Function:    	KrisOS_task_stack_usage
* Purpose:    	Pre-set the task's private stack memory area to some known value
*				in order to estimate the stack usage later
* Arguments:	
*		toPrepare - pointer to the top of the task's stack memory to initialise
* 		size - size of stack memory
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t KrisOS_task_stack_usage(uint32_t* toPrepare, size_t size) {
	
	// Iterator through the memory area to initialise
	uint32_t* iterator;
	
	// The end address of the memory to initialise
	uint32_t* endAddress;
	
	// Argument checks
	TEST_NULL_POINTER(toPrepare)
	TEST_INVALID_SIZE(size)
	
	// Iterate through each memory word in the given memory area and initialise 
	// it to some known value. 
	iterator = toPrepare;
	endAddress = toPrepare + size / 4;
	while (iterator < endAddress)
		*iterator++ = 0xDEADBEEF;
	
	return EXIT_SUCCESS;
}
#endif

