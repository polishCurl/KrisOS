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
#include "common.h"
#include "scheduler.h"
#include "system.h"
#include "os.h"



/*-------------------------------------------------------------------------------
* Scheduler declaration
--------------------------------------------------------------------------------*/
Scheduler scheduler;



/*-------------------------------------------------------------------------------
* Value of exception return for SVC calls
--------------------------------------------------------------------------------*/
uint32_t svc_exc_return;



/*-------------------------------------------------------------------------------
* Function:		idle_task
* Purpose:    	Idle task. Lowest priority task for power saving.
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void idle_task(void) {
	while(1) __wfi();
}

// Idle task definition, the preallocated task control block and private stack
Task idleTask;
const size_t idleTaskStackSize = 100;
uint8_t idleTaskStack[idleTaskStackSize];



/*-------------------------------------------------------------------------------
* Function:    	scheduler_init
* Purpose:    	Initialise the scheduler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_init(void) {
	
	// Initialise each of the queues in the scheduler to NULL, which means they are
	// empty
	int32_t queueIndex;
	for (queueIndex = 0; queueIndex < TOTAL_QUEUES_NO; queueIndex++)
		scheduler.queues[queueIndex] = NULL;
	
	// Start assigning task IDs from 1. (Actually +-1 as system tasks have negative 
	// IDs and user tasks have positive IDs
	scheduler.lastIDUsed = 1;
	
	// Initialise the status bits
	scheduler.status = 0;
	
	// Create the background idle task (priviliged, lowest possible task priority)
	task_declare(&idleTask, idle_task, &idleTaskStack[idleTaskStackSize], TASK_PRIO_LVL_NO, 1);
	scheduler.runPtr = scheduler.queues[TASK_PRIO_LVL_NO];
}



/*-------------------------------------------------------------------------------
* Function:    	scheduler_run
* Purpose:    	Run the scheduler to determine the next task to run.
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_run(void) {
	
	// Go through the ready queues in decreasing priority orded, and as soon as there
	// is at least one task in a given queue, then it must be the one of the highest 
	// priority ready tasks at given time. 
	int32_t queueIndex;
	
	__start_critical();
	{
		for (queueIndex = 0; queueIndex <= TASK_PRIO_LVL_NO; queueIndex++) {
			if (scheduler.queues[queueIndex] != NULL) {
				
				// If currently running task has the same priority as the top priority one, then
				// run the next task the current task points to (round-robin).
				if (scheduler.runPtr->priority == queueIndex && scheduler.runPtr->next != NULL)
					scheduler.topPrioTask = scheduler.runPtr->next;
				
				// Otherwise, pick the first task in specified queue to be next to run
				else
					scheduler.topPrioTask = scheduler.queues[queueIndex];
				
				// The current top priority task will be now running
				scheduler.topPrioTask->status = RUNNING;
				__end_critical();
				return;		
			}
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
		while (scheduler.queues[DELAYED] != NULL && scheduler.queues[DELAYED]->waitCounter <= OS_TICKS) {
			
			// Remove the ready task from delayed queue and reset its wait counter
			toWake = scheduler.queues[DELAYED];
			task_remove_from_queue(toWake);
			toWake->waitCounter = 0;
			
			// Insert the task at the beginning of the ready queue of tasks with same
			// priority
			task_insert_at_queue_start(&scheduler.queues[toWake->priority], toWake);
		}
		
		// Reschedule tasks as the state of ready queues have changed
		scheduler_run();
		
		// The current time-slice will now be divided between more than one task so 
		// time-sliced preemption is switched off until the next time slice is entered
		scheduler.status &= ~(1 << TIME_PREEMPT_Pos);
	}
	__end_critical();
}



/*-------------------------------------------------------------------------------
* Function:    	task_create
* Purpose:    	Create a task and add it to the ready queue
* Arguments:	
*		startAddr - starting address of the task to add
*		stackSize - stack size (in bytes) required by the task
*		priority - the higher the number the lower the priority
*		isPrivileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		
*		pointer to the task created
--------------------------------------------------------------------------------*/
Task* task_create(void* startAddr, size_t stackSize, uint32_t priority,
					uint32_t isPrivileged) {
	
	// Pointer to the task to create
	Task* toCreate;				 
						 
	// Test if priority is matching one of the allowed priority levels
	if (priority > TASK_PRIO_LVL_NO)
		return NULL;
	
	// Check if the starting address of the task code is valid
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
	
	// Assign the task's priority, id, status and sleep time. System tasks have negative
	// IDs while user ones have positive IDs. Reset the waitCounter. Finally, set the 
	// initial stack pointer value
	toCreate->priority = priority;
	toCreate->status = READY;
	toCreate->id = isPrivileged ? -scheduler.lastIDUsed : scheduler.lastIDUsed;
	scheduler.lastIDUsed++;
	toCreate->waitCounter = 0;
	toCreate->sp = ((uint32_t) toCreate->stackBase) + stackSize - (STACK_FRAME_SIZE << WORD_ACCESS_SHIFT);
	
	// Initialise the stack frame and add the newly created task to the ready 
	// queue with specified priority
	task_frame_init(toCreate, startAddr, isPrivileged);
	
	// Insert the task at the beginning of the ready queue of task with the same 
	// priority as the one to insert
	task_insert_at_queue_start(&scheduler.queues[priority], toCreate);
	
	// Return the pointer tu the task created
	return toCreate;
}
					 


/*-------------------------------------------------------------------------------
* Function:    	task_declare
* Purpose:    	Register a statically allocated task at the scheduler
* Arguments:	
* 		toDeclare - pointer to the pre-allocated task control block of the task to declare
*		startAddr - starting address of the task to add
*		stackBase - pointer to the private stack area to be used by the task to create 
*		priority - task priority
*		isPrivileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_declare(Task* toDeclare,  void* startAddr, void* stackBase, 
					  int32_t priority, uint32_t isPrivileged) {
						 
						 
	// Test if input arguments are valid
	if (priority > TASK_PRIO_LVL_NO)
		return EXIT_FAILURE;
	TEST_NULL_POINTER(toDeclare)
	TEST_NULL_POINTER(stackBase)
	TEST_NULL_POINTER(startAddr)
	
	// Attach the stack to the task control block of the task to declare
	toDeclare->stackBase = stackBase;
	
	// Assign the task's priority, id, status and sleep time. System tasks have negative
	// IDs while user ones have positive IDs. Reset the waitCounter. Finally, set the 
	// initial stack pointer value
	toDeclare->priority = priority;
	toDeclare->status = READY;
	toDeclare->id = isPrivileged ? -scheduler.lastIDUsed : scheduler.lastIDUsed;
	scheduler.lastIDUsed++;
	toDeclare->waitCounter = 0;
	toDeclare->sp = ((uint32_t) toDeclare->stackBase) - (STACK_FRAME_SIZE << WORD_ACCESS_SHIFT);
	
	// Initialise the stack frame and add the newly created task to the ready 
	// queue with specified priority
	task_frame_init(toDeclare, startAddr, isPrivileged);
	
	// Insert the task at the beginning of the ready queue of tasks with the same 
	// priority
	task_insert_at_queue_start(&scheduler.queues[priority], toDeclare);

	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_delay
* Purpose:    	Delay the currently running task by 'delay' number of OS ticks
* Arguments: 	
*		delay - number of OS 'ticks' do suspend execution of the task by
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_delay(uint64_t delay) {
	
	// Iterators trough the delayed queue and a pointer to the task to delay
	Task *iterator, *previous, *toDelay;
	
	__start_critical();
	{
		// Only the calling task (currently executing) can delay itself. Remove the
		// running task from the queue it belongs to
		toDelay = scheduler.runPtr;
		task_remove_from_queue(toDelay);
		
		// Update the wait counter and reschedule tasks
		toDelay->waitCounter = OS_TICKS + delay;
		toDelay->status = DELAYED;
		scheduler_run();
		
		// Insert the task to the queue with delayed tasks. Insertion sort it 
		// performed in ascending delay value order.
		toDelay->next = toDelay->previous = NULL;
		
		// The queue is empty
		if (scheduler.queues[DELAYED] == NULL) {
			scheduler.queues[DELAYED] = toDelay;
		}
		// The task to insert to the delayed queue will be the soonest to wake
		else if (toDelay->waitCounter <= scheduler.queues[DELAYED]->waitCounter) {
			toDelay->next = scheduler.queues[DELAYED];
			scheduler.queues[DELAYED]->previous = toDelay;
			scheduler.queues[DELAYED] = toDelay;
		}
		// Iterate through the queue until the right spot is found (ascending delay
		// value order)
		else {
			iterator = scheduler.queues[DELAYED];
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
		
		// The current time-slice will now be divided between more than one task so 
		// time-sliced preemption is switched off until the next time slice is entered
		scheduler.status &= ~(1 << TIME_PREEMPT_Pos);

		// Because the currently running task has just been delayed then context switch
		// is necessary
		SCB->ICSR |= (1 << PENDSV_Pos);	
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_suspend
* Purpose:    	Suspend the task given
* Arguments:	
*		toSuspend - pointer to the task to suspend. If NULL the calling task suspends itself
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_suspend(Task* toSuspend) {
	
	__start_critical();
	{
		// Check if the caller wants to suspend itself
		if (toSuspend == NULL)
			toSuspend = scheduler.runPtr;
		
		// Move the task from the queue it is in to the blocked one and reschedule
		// tasks
		task_remove_from_queue(toSuspend);
		toSuspend->status = BLOCKED;
		scheduler_run();
		task_insert_at_queue_start(&scheduler.queues[BLOCKED], toSuspend);
		
		// Perform context-switch only if the next task to run is different from the 
		// current one. Disable time-sliced preemption, otherwise the next task to run
		// won't be given at least one full time-slice
		if (scheduler.topPrioTask != scheduler.runPtr) {
			scheduler.status &= ~(1 << TIME_PREEMPT_Pos);
			SCB->ICSR |= (1 << PENDSV_Pos);	
		}
	}
	__end_critical();
	
	return EXIT_SUCCESS;
}


/*-------------------------------------------------------------------------------
* Function:    	task_resume
* Purpose:    	Resume the task given
* Arguments:	
*		toSuspend - pointer to the task to resume.
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_resume(Task* toResume) {
	
	// Move the task from the queue it belongs to to the ready queue of tasks with
	// same priority
	__start_critical();
	{
		task_remove_from_queue(toResume);
		toResume->status = READY;
		task_insert_at_queue_start(&scheduler.queues[toResume->priority], toResume);
		scheduler_run();
		
		// Perform context-switch only if the next task to run is different from the 
		// current one. Disable time-sliced preemption, otherwise the next task to run
		// won't be given at least one full time-slice
		if (scheduler.topPrioTask != scheduler.runPtr) {
			scheduler.status &= ~(1 << TIME_PREEMPT_Pos);
			SCB->ICSR |= (1 << PENDSV_Pos);	
		}
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_insert_at_queue_start
* Purpose:    	Insert the task given at the beginning of the queue specified
* Arguments: 	
*		queue - queue to update
* 		toInsert - task to insert
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_insert_at_queue_start(Task** queue, Task* toInsert) {
	
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
* Function:    	task_remove_from_queue
* Purpose:    	Remove the task given from the queue it belongs to
* Arguments: 
* 		toRemove - task to remove
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_remove_from_queue(Task* toRemove) {
	
	// Index of the queue the task belongs to
	uint8_t queueIndex;
	
	// Determine which queue the task belongs to
	if (toRemove->status == RUNNING || toRemove->status == READY)
		queueIndex = toRemove->priority;
	else
		queueIndex = toRemove->status;
	
	__start_critical();
	{
		// Join the next and previous tasks together
		if (toRemove->previous != NULL)
			toRemove->previous->next = toRemove->next;
		
		if (toRemove->next != NULL)
			toRemove->next->previous = toRemove->previous;
		
		// If the task to remove is at the start of the queue then update the queue head 
		if (scheduler.queues[queueIndex] == toRemove) 
			scheduler.queues[queueIndex] = scheduler.queues[queueIndex]->next;
		
		// Now the task is not a member of any queues
		toRemove->next = toRemove->previous = NULL;
	}
	__end_critical();
	
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_frame_init
* Purpose:    	Initialise the stak frame of the task selected
* Arguments: 	
*		toInit - pointer to the task to have its stack frame initialised
*		startAddr - address of the first instruction of the task to initialise
*		isPrivileged - task's priviliged access level flag
* Returns: 
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_frame_init(Task* toInit, void* startAddr, uint32_t isPrivileged) {
	
	// Helper pointer for specifying initial register values in the stack frame
	uint32_t* taskFramePtr; 

	// Set the initial PC to the starting address of task's code
	taskFramePtr = (uint32_t*) (toInit->sp + (STACK_FRAME_PC << WORD_ACCESS_SHIFT));	
	*taskFramePtr = (uint32_t) startAddr;
	
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

