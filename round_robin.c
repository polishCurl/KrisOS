/*******************************************************************************
* File:     	round_robin.c
* Brief:    	Round-Robin preemptive circular scheduler
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/10/2016
* Last mod: 	21/10/2016
*
* Note: 		
* All task except the idle one (created at system initialisation) are chained 
* into a circle using the *next pointer. The scheduler only runs if the state
* of tasks in the readyQueue is changed. 
*
* There are two queues maintained by the scheduler. 
*	1. readyQueue   - contains all the tasks that are ready to be run. Among these
*	   				  one task has RUNNING state and is currently executed
*	2. blockedQueue - contains all the tasks that can't be picked to run for any
*					  reason (for example, the task is suspended or is delayed
*					  by some time
*
* runPtr - points to the metadata of the task that is currently running
*
*******************************************************************************/
#include "common.h"
#include "KrisOS.h"
#include "system.h"
#include "scheduler.h"
#include "os.h"


/*-------------------------------------------------------------------------------
* Store the value of exception return for SVC calls
--------------------------------------------------------------------------------*/
uint32_t svc_exc_return;

/*-------------------------------------------------------------------------------
* Pointer to the task that is currently running
--------------------------------------------------------------------------------*/
Task* runPtr; 					

/*-------------------------------------------------------------------------------
* Ready and Blocked task queues
--------------------------------------------------------------------------------*/
TCB readyQueue; 			
TCB blockedQueue;


/*-------------------------------------------------------------------------------
* Function:    	tcb_init
* Purpose:    	Initialise the Task Control Block
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void tcb_init(void) {
	
	// Reset the task counters in both ready and blocked queues
	readyQueue.noOfTasks = 0;	
	blockedQueue.noOfTasks = 0; 
	
	// Create the first system task. It will be run only if there are no other tasks
	// to run 
	create_task(os_sleep, -1, 7, 100, 1);
	readyQueue.tasks[0]->next = readyQueue.tasks[0];
}


/*-------------------------------------------------------------------------------
* Function:    	run_scheduler
* Purpose:    	Run the scheduler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void run_scheduler(void) {

	uint32_t i;
	
	// If there is only one task READY left (the idle one) then make the last task
	// that used to be running point to the idle one. Otherwise Chain all the ready 
	// tasks (except the idle one) into a circle
	if (readyQueue.noOfTasks == 1) 
		readyQueue.tasks[1]->next = readyQueue.tasks[0];
	else {
		for (i = 1; i < readyQueue.noOfTasks; i++)
			readyQueue.tasks[i]->next = i < readyQueue.noOfTasks - 1 ?
										readyQueue.tasks[i + 1] : readyQueue.tasks[1];
	}
}


/*-------------------------------------------------------------------------------
* Function:    	update_counters
* Purpose:    	Update counter counters of all the tasks in BLOCKED queue
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void update_counters(void) {
	
	// Go through all the tasks in BLOCKED queue. If it is delayed one (not 
	// permanently suspended) then decrement its sleep time counter. If the task's
	// counter reaches 0 the task should be resumed
	uint32_t i;
	for (i = 0; i < blockedQueue.noOfTasks; i++)
		if (blockedQueue.tasks[i]->counter > 0)
			blockedQueue.tasks[i]->counter--;
		else if (blockedQueue.tasks[i]->counter == 0)
			resume_task(blockedQueue.tasks[i]->id);
}


/*-------------------------------------------------------------------------------
* Function:    	create_task
* Purpose:    	Create a user-defined task and add it to the scheduler
* Arguments:	
*		start_address - starting address of the task to add
*		id - unique task identifier
*		priority - the higher the number the lower the priority
*		stack_size - stack size (in bytes) required by the task
*		privileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t create_task(void* startAddr, int32_t id, uint32_t priority, 
					 size_t stackSize, uint32_t privileged) {
	
	// Helper pointer for specifying initial register values in the stack frame
	uint32_t* taskFramePtr; 					 
						 
	// Test if the arguments are valid or the imit of tasks is reached
	if (priority > MIN_TASK_PRIO || readyQueue.noOfTasks >= MAX_TASKS)
		return EXIT_FAILURE;
	TEST_NULL_POINTER(startAddr)
	
	// User tasks have positive IDs while system ones (privileged) have
	// negative
	if ((privileged && id >= 0) || (!privileged && id < 0))
		return EXIT_FAILURE;
	
	// Adjust the stack size to comply with the stack alignment
	if (stackSize % STACK_ALIGNMENT)
		stackSize = stackSize + (STACK_ALIGNMENT - stackSize % STACK_ALIGNMENT);
	
	__start_critical();
	
	// Allocate memory for the task metadata and its stack memory
	readyQueue.tasks[readyQueue.noOfTasks] = (Task*) malloc(sizeof(Task));
	TEST_NULL_POINTER(startAddr)
	readyQueue.tasks[readyQueue.noOfTasks]->stackBase = malloc(stackSize);
	TEST_NULL_POINTER(readyQueue.tasks[readyQueue.noOfTasks]->stackBase)
	
	// Assign the task's priority, status, id and sleep time
	readyQueue.tasks[readyQueue.noOfTasks]->priority = priority;
	readyQueue.tasks[readyQueue.noOfTasks]->id = id;
	readyQueue.tasks[readyQueue.noOfTasks]->status = READY;
	readyQueue.tasks[readyQueue.noOfTasks]->counter = 0;
	
	// Set the initial stack pointer
	readyQueue.tasks[readyQueue.noOfTasks]->sp = ((uint32_t) readyQueue.tasks[readyQueue.noOfTasks]->stackBase) + 
												 stackSize - (STACK_FRAME_SIZE << WORD_ACCESS_SHIFT);
	
	// Set the initial PC to the starting address of task's code
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp + 
				   (STACK_FRAME_PC << WORD_ACCESS_SHIFT));	
	*taskFramePtr = (uint32_t) startAddr;
	
	// Set the initial value of xPSR, only the Thumb bit is set
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp + 
				   (STACK_FRAME_xPSR << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = INIT_xPSR;
	
	// Set the initial value of LR, make LR point to a function for 
	// self-termination and removal of the task that has no more processing
	// to do
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp + 
				   (STACK_FRAME_LR << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = (uint32_t) task_complete_handler;
	
	// Set the initial value of EXC_RETURN, which specifies the use of stack pointer PSP or MSP and floating 
	// point unit before and after return from this exception
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp +
				   (STACK_FRAME_EXC_RETURN << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = EXC_RETURN_2;
	
	// Set the initial value of control register according to privilege level. This will
	// specify access level of the task
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp +
		           (STACK_FRAME_CONTROL << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = privileged ? 0x2 : 0x3;
	
	// Update the task count and run the scheduler if the OS is already running
	readyQueue.noOfTasks++;
	if (OS_RUNNING)
		run_scheduler();
	
	__end_critical();
	return EXIT_SUCCESS;
}
					 

/*-------------------------------------------------------------------------------
* Function:    	suspend_task
* Purpose:    	Suspend the given task
* Arguments:	
*		toSuspendID - ID of the task to suspend
* 		ticks - number of OS clock ticks task should be suspended for (if 0, task 
*				is suspended until explicitly resumed
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t suspend_task(int32_t toSuspendID, int32_t ticks) {
	
	// Used to iterate through the Task Control Block
	int32_t i;
	Task* taskToSuspend;
	
	__start_critical();
	
	// Search for the task in the READY queue
	i = find_task(toSuspendID, &readyQueue); 
	
	// if the task was found
	if (i != NOT_FOUND) {
		
		// Update both queues
		taskToSuspend = remove_from_queue(i, &readyQueue);
		add_to_queue(taskToSuspend, &blockedQueue);
		
		// If there is time to wait specified, then set the counter to it.
		// Otherwise set the sleep time to -1 to indicate that the function can only
		// be resumed by calling task_resume
		taskToSuspend->counter = ticks == NULL ? -1 : ticks;
		
		// Reschedule tasks if the OS is already running
		if (OS_RUNNING)
			run_scheduler();
		
		// if the task to suspend is the currently running one, context switch must be
		// immediate, so the PendSV_IRQ is set
		if (taskToSuspend->status == RUNNING)
			SCB->ICSR |= (1 << PENDSV_Pos);	
		
		__end_critical();
		return EXIT_SUCCESS;
	}
	__end_critical();
	return EXIT_FAILURE;
}


/*-------------------------------------------------------------------------------
* Function:    	resume_task
* Purpose:    	Resume the execution of given task
* Arguments:	
*		toResumeID - ID of the task to resume
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t resume_task(int32_t toResumeID) {
	
	// Used to iterate through the Task Control Block
	int32_t i;
	__start_critical();
	
	// Search for the task in the BLOCKED queue
	i = find_task(toResumeID, &blockedQueue);

	// if the task was found
	if (i != NOT_FOUND) {
	
		// Update both queues
		add_to_queue(remove_from_queue(i, &blockedQueue), &readyQueue);
		
		// Reschedule tasks if the OS is already running
		if (OS_RUNNING)
			run_scheduler();

		__end_critical();
		return EXIT_SUCCESS;
	}
	__end_critical();
	return EXIT_FAILURE;
}



/*-------------------------------------------------------------------------------
* Function:    	delete_task
* Purpose:    	Pernamently remove given task so it is no longer possible to schedule
* Arguments:	
*		toDeleteID - ID of the task to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t delete_task(int32_t toDeleteID) {

	// Used to iterate through the Task Control Block
	int32_t i;
	Task* taskToDelete;
	
	i = 0;
	__start_critical();
	
	// Search for the task in the READY queue
	i = find_task(toDeleteID, &readyQueue); 
	
	// If the task is found
	if (i != NOT_FOUND) {
		// Remove the task from READY queue and free the memory it is occupying (both
		// stack and Task Control Block entry)
		taskToDelete = remove_from_queue(i, &readyQueue);
		free(taskToDelete->stackBase);
		
		// Reschedule tasks if the OS is already running
		if (OS_RUNNING)
			run_scheduler();
		
		// if the task to suspend is the currently running one, context switch must be
		// immediate, so the PendSV_IRQ is set
		if (taskToDelete->status == RUNNING)
			SCB->ICSR |= (1 << PENDSV_Pos);	
		
		free(taskToDelete);
		__end_critical();
		return EXIT_SUCCESS;
	}

	// Search for the task in the BLOCKED queue
	i = find_task(toDeleteID, &blockedQueue); 

	// If the task is found 
	if (i != NOT_FOUND)  {
		// Remove the task from BLOCKED queue and free the memory it is occupying (both
		// stack and Task Control Block entry)
		taskToDelete = remove_from_queue(i, &blockedQueue);
		free(taskToDelete->stackBase);
		free(taskToDelete);
		__end_critical();
		return EXIT_SUCCESS;
	}
	
	__end_critical();
	return EXIT_FAILURE;
}


/*-------------------------------------------------------------------------------
* Function:    	add_to_queue
* Purpose:    	Add the given task to the specified queue
* Arguments:	
*		toAdd - task to add
*		queue - queue to update
* Returns: 		-
--------------------------------------------------------------------------------*/
void add_to_queue(Task* toAdd, TCB* queue) {
	queue->tasks[queue->noOfTasks] = toAdd;
	queue->noOfTasks++;
}


/*-------------------------------------------------------------------------------
* Function:    	remove_from_queue
* Purpose:    	Remove the task from the specified queue
* Arguments:	
*		index - index of the task to remove in the queue
*		queue - queue from which the task should be removed
* Returns: 		
*		pointer to the task that is removed
--------------------------------------------------------------------------------*/
Task* remove_from_queue(uint32_t index, TCB* queue) {
	
	// Plase the last task in the queue in the position of the task to be removed
	// and update the task counter
	Task* toRemove = queue->tasks[index];
	queue->tasks[index] = queue->tasks[queue->noOfTasks - 1];
	queue->noOfTasks--;
	return toRemove;
}


/*-------------------------------------------------------------------------------
* Function:    	find_task
* Purpose:    	Try to find the task with specified ID in the given queue
* Arguments:	
*		id - ID of the task to find
*		queue - queue in which the task should be searched for
* Returns: 		
*		index of the task in the queue, or -1 (if not ffound)
--------------------------------------------------------------------------------*/
int32_t find_task(int32_t id, TCB* queue) {
	
	int32_t i;
	Task* toFind;
	
	// Iterate through the queue until the task with specified ID is found or
	// we've searched through all the tasks with no success 
	i = 0;
	toFind = queue->tasks[i];
	while(toFind->id != id && i < queue->noOfTasks)
		toFind = queue->tasks[++i];
	
	// If the task couln't be found then return -1
	if (i == queue->noOfTasks)
		return -1;
	
	return i;
}


/*-------------------------------------------------------------------------------
* Function:    	task_complete_handler
* Purpose:    	Routine performed once a task's execution is complete
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void task_complete_handler(void) {
	// Pernamently remove the currently executed task
	if (__get_control() & 0x1)
		KrisOS_delete_task(runPtr->id);
	else
		delete_task(runPtr->id);
}

