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
	
	// Initialise each of the queues in the scheduler to NULL, which means they are
	// empty. Also initialise the lastRunTask pointers for each of the ready queues
	int32_t queueIndex;
	for (queueIndex = 0; queueIndex < READY_QUEUES_NO; queueIndex++) {
		scheduler.ready[queueIndex] = NULL;
		scheduler.lastRunTask[queueIndex] = NULL;	
	}
	scheduler.blocked = NULL;
	
	// Start assigning task IDs from 1. (Actually +-1 as system tasks have negative 
	// IDs and user tasks have positive IDs
	scheduler.lastIDUsed = 1;
	
	// Initialise the status bits
	scheduler.status = 0;
	
	// Set the runPtr to the idle task
	scheduler.runPtr = &idleTask;
}



/*-------------------------------------------------------------------------------
* Function:    	scheduler_run
* Purpose:    	Run the scheduler to determine the next task to run.
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_run(void) {
	
	// Iterator through the ready queues
	int32_t queueIndex = 0;
	
	__start_critical();
	{
		// Find the first non-empty ready queue
		while (scheduler.ready[queueIndex] == NULL) 
			queueIndex++;
				
		// If no task has ever been run from the given ready queue or there is only
		// one task in the given queue then run the first task from the queue otherwise 
		// do round-robin by picking the next task in the queue with respect to the one
		// last run
		if (scheduler.lastRunTask[queueIndex] != NULL && scheduler.lastRunTask[queueIndex]->next != NULL) 
			scheduler.lastRunTask[queueIndex] = scheduler.topPrioTask = scheduler.lastRunTask[queueIndex]->next;
		else
			scheduler.lastRunTask[queueIndex] = scheduler.topPrioTask = scheduler.ready[queueIndex];
		
		if (scheduler.runPtr->status == RUNNING)
			scheduler.runPtr->status = READY;
		scheduler.topPrioTask->status = RUNNING; 
		
		// The current time-slice will now be divided between more than one task so 
		// time-sliced preemption is switched off until the next time slice is entered
		scheduler.status &= ~(1 << TIME_PREEMPT_Pos);
		
		// Perform context-switch only if the next task to run is different from the 
		// current one, according to the scheduling policy
		if (scheduler.topPrioTask != scheduler.runPtr) {
			SCB->ICSR |= (1 << PENDSV_Pos);	
#ifdef SHOW_DIAGNOSTIC_DATA
			KrisOS.contextSwitchNo++;
#endif
		}
	}
	__end_critical();
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
	
	__start_critical();
	{
		// Go through the delayed queue until a 'not ready' task is encountered (task are 
		// sorted in ascending delay time)
		while (scheduler.blocked != NULL && scheduler.blocked->waitCounter <= OS_TICKS) {
			
			// Remove the ready task from delayed queue and reset its wait counter
			toWake = scheduler.blocked;
			task_remove(&scheduler.blocked, toWake);
			toWake->waitCounter = 0;
			toWake->status = READY;
			
			// Insert the task at the beginning of the ready queue of tasks with same
			// priority
			task_add(&scheduler.ready[toWake->priority], toWake);
		}
		// Reschedule tasks as the state of ready queues have changed
		scheduler_run();
	}
	__end_critical();
}



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
#ifdef USE_HEAP
Task* task_create_dynamic(void* startAddr, size_t stackSize, uint32_t priority,
						  uint32_t isPrivileged) {
	
	// Pointer to the task to create
	Task* toCreate;				 
						 
	// Test if priority is matching one of the allowed priority levels and the starting 
	// address of the task code is valid
	if (priority > TASK_PRIO_LVL_NO)
		return NULL;
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

	// Initialise the task's control block and stack frame
	task_init(toCreate, startAddr, isPrivileged, priority);
	
	// Insert the task at the beginning of the ready queue of task with the same 
	// priority as the one to insert. And reschedule task if OS is already running
	__start_critical();
	{
		task_add(&scheduler.ready[priority], toCreate);
		if (KrisOS.isRunning)
			scheduler_run();
	} 
	__end_critical();
	
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
	if (priority > TASK_PRIO_LVL_NO)
		return EXIT_FAILURE;
	TEST_NULL_POINTER(toDeclare)
	TEST_NULL_POINTER(stackBase)
	TEST_NULL_POINTER(startAddr)
	
	// Attach the stack to the task control block of the task to declare
	toDeclare->stackBase = stackBase;
	toDeclare->sp = ((uint32_t) toDeclare->stackBase) - (STACK_FRAME_SIZE << WORD_ACCESS_SHIFT);

	// Initialise the task's control block and stack frame
	task_init(toDeclare, startAddr, isPrivileged, priority);
	
	// Insert the task at the beginning of the ready queue of task with the same 
	// priority as the one to insert. And reschedule task if OS is already running
	__start_critical();
	{
		task_add(&scheduler.ready[priority], toDeclare);
		if (KrisOS.isRunning)
			scheduler_run();
	} 
	__end_critical();

	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_sleep
* Purpose:    	Suspend the execution of the running task for specified amount of
* 				OS ticks.
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
#ifdef USE_MUTEX 	
		// Release any locks the calling tasks has		
		mutex_release_all(scheduler.runPtr);
#endif		
		// Only the calling task (currently executing) can delay itself. Remove the
		// running task from the queue it belongs to
		toDelay = scheduler.runPtr;
		task_remove(&scheduler.ready[toDelay->priority], toDelay);
		
		// Update the wait counter and reschedule tasks
		toDelay->waitCounter = delay == TIME_INFINITY ? UINT64_MAX : OS_TICKS + delay;
		toDelay->status = SLEEPING;
		scheduler_run();
		
		// Insert the task to the queue with delayed tasks. Insertion sort it 
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
			while (iterator != NULL && toDelay->waitCounter >= iterator->waitCounter) {
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
* Returns: 		-
--------------------------------------------------------------------------------*/
void task_delete(void) {
	
	Task* toDelete;
	__start_critical();
	{	
		toDelete = scheduler.runPtr;
		
#ifdef USE_MUTEX 	
		// Release any locks the calling tasks has		
		mutex_release_all(toDelete);
#endif		
		// Update the task's status 
		task_remove(&scheduler.ready[toDelete->priority], toDelete);
		toDelete->status = REMOVED;
		
#ifdef USE_HEAP
		// Free the heap memory the task occupies (if any)
		free(toDelete->stackBase);
		free(toDelete);
#endif
		scheduler_run();
	}
	__end_critical();	
}



/*-------------------------------------------------------------------------------
* Function:    	task_add
* Purpose:    	Insert the task given at the beginning of the queue specified
* Arguments: 	
*		queue - queue to update
* 		toInsert - task to insert
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_add(Task** queue, Task* toInsert) {
	
	// The task is inserted at the beginning of the queue
	toInsert->previous = NULL;
	
	__start_critical();
	{
		// The queue is empty before insertion
		if (*queue == NULL) {
			toInsert->next = NULL;
			*queue = toInsert;
		}
		// There is at least one task already in the queue
		else {
			(*queue)->previous = toInsert;
			toInsert->next = *queue;
			*queue = toInsert;
		}
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_prio_add
* Purpose:    	Add the task given to the queue specified in descending priority
* 				order.
* Arguments: 	
*		queue - queue to update
* 		toInsert - task to insert
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_prio_add(Task** queue, Task* toInsert) {
	
	// Iterators through the queue to update
	Task* iterator;
	Task* previous;
	
	__start_critical();
	{
		// The queue is empty
		if (*queue == NULL) {
			*queue = toInsert;
		}
		// The task to insert has the hightest priority (is inserted at the beginnig)
		else if (toInsert->priority <= (*queue)->priority) {
			toInsert->next = *queue;
			(*queue)->previous = toInsert;
			*queue = toInsert;
		}
		// Iterate through the queue until a lower priority task is encountered
		else {
			iterator = *queue;
			while (iterator != NULL && toInsert->priority >= iterator->priority) {
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
	toInit->priority = priority;
	toInit->status = READY;
	toInit->id = isPrivileged ? -scheduler.lastIDUsed : scheduler.lastIDUsed;
	scheduler.lastIDUsed++;
	toInit->waitCounter = 0;
#ifdef USE_MUTEX
	toInit->basePrio = priority;
	toInit->mutexHeld = toInit->mutexWaiting = NULL;
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

