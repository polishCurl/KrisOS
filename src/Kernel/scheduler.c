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

// Idle task parameters
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
	for (queueIndex = 0; queueIndex <= TASK_PRIO_LVL_NO; queueIndex++)
		scheduler.readyTasks[queueIndex] = NULL;
	scheduler.delayedTasks = NULL;
	
	// No tasks to wake, so set the time of the soonest task to be made ready to 
	// 'infinity'
	scheduler.nextToWake = UINT64_MAX;
	
	// Start assigning task IDs from 1. (Actually +-1 as system tasks have negative 
	// IDs and user tasks have positive IDs
	scheduler.lastIDUsed = 1;
	
	// Create the background idle task (priviliged, lowest possible task priority)
	task_declare(&idleTask, idle_task, &idleTaskStack[idleTaskStackSize], TASK_PRIO_LVL_NO, 1);

	// Initialise the run ptr to the idle task
	scheduler.runPtr = scheduler.readyTasks[TASK_PRIO_LVL_NO];
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
	for (queueIndex = 0; queueIndex <= TASK_PRIO_LVL_NO; queueIndex++) {
		if (scheduler.readyTasks[queueIndex] != NULL) {
			
			// If currently running task has the same priority as the top priority one, then
			// run the next task the current task points to (round-robin).
			if (scheduler.runPtr->waitCounter == 0 && scheduler.runPtr->priority == queueIndex)
				scheduler.topPrioTask = scheduler.runPtr->next;
			// Otherwise, pick the first task in specified queue to be next to run
			else
				scheduler.topPrioTask = scheduler.readyTasks[queueIndex];
			
			__end_critical();
			return;		
		}
	}
}



/*-------------------------------------------------------------------------------
* Function:    	scheduler_wake_tasks
* Purpose:    	Check which tasks are ready, and wake them up
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_wake_tasks(void) {
	
	// Iterator through the list of delayed tasks and a pointer to the task to wake
	Task* iterator;
	Task* toWake;
	
	// The new earliest task to wake, after the now-ready tasksa are woken up
	uint64_t nextEarliestTask = UINT64_MAX;
	
	// Iterate through the list of delayed tasks
	__start_critical();
	iterator = scheduler.delayedTasks;
	while (iterator->next != scheduler.delayedTasks) {
		
		// If the current task is to be woken up, remove it from the delayed queue and 
		// add it back to the ready queue of tasks with priority specified. Iterator
		// needs to be updated before the task is put in the ready queue, as in the meantime
		// the ->next pointer is modified. Also reset the waitCounter
		if (iterator->waitCounter == scheduler.nextToWake) {
			toWake = remove_from_queue(&scheduler.delayedTasks, iterator->id);
			iterator = iterator->next;
			toWake->waitCounter = 0;
			add_to_queue(&scheduler.readyTasks[iterator->priority], toWake);
		}
		// Otherwise, if a task is not yet ready, but will be ready sooner than nextEarliestTask,
		// then update the nextEarliestTask
		else {
			nextEarliestTask = iterator->waitCounter < nextEarliestTask ? iterator->waitCounter : nextEarliestTask;
			iterator = iterator->next;
		}
	}
	
	// When the loop terminates there is still one more task to check
	if (iterator->waitCounter == scheduler.nextToWake) {
		toWake = remove_from_queue(&scheduler.delayedTasks, iterator->id);
		toWake->waitCounter = 0;
		add_to_queue(&scheduler.readyTasks[iterator->priority], toWake);
	}
	else if (iterator->waitCounter < nextEarliestTask) {
		nextEarliestTask = iterator->waitCounter;
	}
			
	// Update the cache for the soonest task to be ready and run the scheduler
	scheduler.nextToWake = nextEarliestTask;
	__end_critical();
	scheduler_run();
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
*		unique nonzero ID number of the task created, 0 otherwise
--------------------------------------------------------------------------------*/
int32_t task_create(void* startAddr, size_t stackSize, uint32_t priority,
					uint32_t isPrivileged) {
	
	// Pointer to the task to create
	Task* toCreate;				 
						 
	// Test if priority is matching one of the allowed priority levels
	if (priority > TASK_PRIO_LVL_NO)
		return 0;
	
	// Check if the starting address of the task code is valid
	TEST_NULL_POINTER(startAddr)
	
	// Adjust the stack size to comply with the stack alignment
	if (stackSize % STACK_ALIGNMENT)
		stackSize = stackSize + (STACK_ALIGNMENT - stackSize % STACK_ALIGNMENT);
	
	__start_critical();
	
	// Allocate memory for the task control block. 
	toCreate = malloc(sizeof(Task));
	TEST_NULL_POINTER(toCreate)
	
	// Allocate memory for the task's private stack
	toCreate->stackBase = malloc(stackSize);
	TEST_NULL_POINTER(toCreate->stackBase)
	
	// Assign the task's priority and id and sleep time. System tasks have negative
	// IDs while user ones have positive IDs. Reset the waitCounter. Finally, set the 
	// initial stack pointer value
	toCreate->priority = priority;
	toCreate->id = isPrivileged ? -scheduler.lastIDUsed : scheduler.lastIDUsed;
	scheduler.lastIDUsed++;
	toCreate->waitCounter = 0;
	toCreate->sp = ((uint32_t) toCreate->stackBase) + stackSize - (STACK_FRAME_SIZE << WORD_ACCESS_SHIFT);
	
	// Initialise the stack frame and add the newly created task to the ready 
	// queue with specified priority
	task_frame_init(toCreate, startAddr, isPrivileged);
	add_to_queue(&scheduler.readyTasks[priority], toCreate);
	__end_critical();
	
	// Return the ID of the task created
	return toCreate->id;
}
					 


/*-------------------------------------------------------------------------------
* Function:    	task_declare
* Purpose:    	Register a statically allocated task at the scheduler
* Arguments:	
* 		tcb - pointer to the previously allocated task control block of the task to create
*		startAddr - starting address of the task to add
*		stackBase - pointer to the private stack area to be used by the task to create 
*		priority - task priority
*		isPrivileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		
*		unique nonzero ID number of the task created, 0 otherwise
--------------------------------------------------------------------------------*/
int32_t task_declare(void* tcb,  void* startAddr, void* stackBase, int32_t priority, 
					 uint32_t isPrivileged) {
	
	// Task to create
	Task* toDeclare;					 
						 
	// Test if input arguments are valid
	if (priority > TASK_PRIO_LVL_NO)
		return 0;
	TEST_NULL_POINTER(tcb)
	TEST_NULL_POINTER(stackBase)
	TEST_NULL_POINTER(startAddr)
	
	// Attach the stack to the task control block of the task to declare
	toDeclare = tcb;
	toDeclare->stackBase = stackBase;
	
	// Assign the task's priority and id and sleep time. System tasks have negative
	// IDs while user ones have positive IDs. Reset the waitCounter
	toDeclare->priority = priority;
	toDeclare->id = isPrivileged ? -scheduler.lastIDUsed : scheduler.lastIDUsed;
	scheduler.lastIDUsed++;
	toDeclare->waitCounter = 0;
	toDeclare->sp = ((uint32_t) toDeclare->stackBase) - (STACK_FRAME_SIZE << WORD_ACCESS_SHIFT);
	
	// Initialise the stack frame and add the newly created task to the ready 
	// queue with specified priority
	task_frame_init(tcb, startAddr, isPrivileged);
	__start_critical();
	add_to_queue(&scheduler.readyTasks[priority], toDeclare);
	__end_critical();
	
	// Return the ID of the task created
	return toDeclare->id;
}
					 


/*-------------------------------------------------------------------------------
* Function:    	task_delay
* Purpose:    	Delay given task by specified amount of OS 'ticks'
* Arguments: 	
*		delay - number of OS 'ticks' do suspend execution of the task by
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_delay(uint64_t delay) {
	
	// Remove the calling task (currently running) from the queue of delayed tasks
	Task* toDelay = remove_from_queue(&scheduler.readyTasks[scheduler.runPtr->priority],
									scheduler.runPtr->id);  
	// Test if task found
	if (toDelay == NULL)
		return EXIT_FAILURE;
	
	// Update the wait counter and check if its less than current value of the soonest task.
	// If so, this is now the earliest task to be made ready
	toDelay->waitCounter = OS_TICKS + delay;
	if (toDelay->waitCounter < scheduler.nextToWake)
		scheduler.nextToWake = toDelay->waitCounter;
	
	// Add the task to timer blocked queue, reschedule tasks and force the immediate 
	// context switch
	add_to_queue(&scheduler.delayedTasks, toDelay);
	scheduler_run();
	SCB->ICSR |= (1 << PENDSV_Pos);	
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	task_frame_init
* Purpose:    	Initialise the stak frame of the task selected
* Arguments: 	
*		toInit - pointer to the task to have its stack frame initialised
*		startAddr - starting address of the task to have its stack frame initialised
*		privileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		-
--------------------------------------------------------------------------------*/
void task_frame_init(Task* toInit, void* startAddr, uint32_t privileged) {
	
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
	*taskFramePtr = privileged ? 0x2 : 0x3;
}



/*-------------------------------------------------------------------------------
* Function:    	add_to_queue
* Purpose:    	Add given task to the queue specified
* Arguments: 	
*		queue - queue to add the task to
* 		toAdd - task to add
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t add_to_queue(Task** queue, Task* toAdd) {
	
	// Helper iterator through the queue
	Task* iterator;
	__start_critical();
	
	// If the queue is empty then make the head point to the task to add
	if (*queue == NULL) {
		toAdd->next = toAdd;
		*queue = toAdd;
		__end_critical();
		return EXIT_SUCCESS;
	}
	
	// Otherwise insert the task at the end of the queue and close the list by
	// making it a loop of tasks
	iterator = *queue;
	while(iterator->next != *queue)
		iterator = iterator->next;
	
	toAdd->next = *queue;
	iterator->next = toAdd;
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	remove_from_queue
* Purpose:    	Remove the task with given ID from the queue specified.
* Arguments: 	
*		queue - address of the pointer to the queue to remove the task from
* 		id - ID of the task to remove
* Returns: 
* 		pointer to the task removed
--------------------------------------------------------------------------------*/
Task* remove_from_queue(Task** queue, int32_t id) {
	
	// Pointer to the task to remove and a helper iterator through the queue
	Task* toRemove;
	Task* iterator;
	
	// If the queue is empty then the task is certainly not in it
	if (*queue == NULL)
		return NULL;
	
	// Initialise the pointers
	__start_critical();
	toRemove = NULL;
	iterator = *queue;
	
	// Iterate through the queue until we reached the end of it, or a loop (ready tasks,
	// round-robin) or we have found the task to remove
	while (iterator->next != *queue && iterator->next->id != id)
		iterator = iterator->next;
	
	// If the task is found...
	if (iterator->next->id == id) {
		toRemove = iterator->next;
		
		// This is the only task in the queue so set the pointer to the queue to NULL
		if (iterator->next == iterator) {
			*queue = NULL;
		} 
		// Otherwise, link the task before and the task after. If the readyQueue pointer
		// is affected then update it too.
		else {
			if (iterator->next == *queue)
				*queue = iterator->next->next;
			iterator->next = iterator->next->next;
		}
	}
	__end_critical();
	return toRemove;
}
