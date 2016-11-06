/*******************************************************************************
* File:     	scheduler.h
* Brief:    	Header file for scheduler.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	04/11/2016
* Last mod: 	04/11/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"
#include "task.h"


/*-------------------------------------------------------------------------------
* Function:    	os_start
* Purpose:    	Start the operating system
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_start(void);


/*-------------------------------------------------------------------------------
* Function:    	run_scheduler
* Purpose:    	Run the scheduler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void run_scheduler(void);


/*-------------------------------------------------------------------------------
* Function:    	update_counters
* Purpose:    	Update counter counters of all the tasks in BLOCKED queue
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void update_counters(void);
	

/*-------------------------------------------------------------------------------
* Function:    	tcb_init
* Purpose:    	Initialise the Task Control Block
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void tcb_init(void);


/*-------------------------------------------------------------------------------
* Function:    	create_task
* Purpose:    	Create a user-defined task and add it to the scheduler
* Arguments:	
*		startAddr - starting address of the task to add
*		id - unique task identifier
*		priority - the higher the number the lower the priority
*		stackSize - stack size (in bytes) required by the task
*		privileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t create_task(void* startAddr, int32_t id, uint32_t priority, 
					 size_t stackSize, uint32_t privileged);


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
uint32_t suspend_task(int32_t toSuspendID, int32_t ticks);


/*-------------------------------------------------------------------------------
* Function:    	resume_task
* Purpose:    	Resume the execution of given task
* Arguments:	
*		toResumeID - ID of the task to resume
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t resume_task(int32_t toResumeID);


/*-------------------------------------------------------------------------------
* Function:    	delete_task
* Purpose:    	Pernamently remove given task so it is no longer possible to schedule
* Arguments:	
*		toDeleteID - ID of the task to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t delete_task(int32_t toDeleteID);


/*-------------------------------------------------------------------------------
* Function:    	task_complete_handler
* Purpose:    	Routine performed once a task's execution is complete
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void task_complete_handler(void);


/*-------------------------------------------------------------------------------
* Function:    	add_to_queue
* Purpose:    	Add the given task to the specified queue
* Arguments:	
*		toAdd - task to add
*		queue - queue to update
* Returns: 		-
--------------------------------------------------------------------------------*/
void add_to_queue(Task* toAdd, TCB* queue);


/*-------------------------------------------------------------------------------
* Function:    	remove_from_queue
* Purpose:    	Remove the task from the specified queue
* Arguments:	
*		index - index of the task to remove in the queue
*		queue - queue from which the task should be removed
* Returns: 		
*		pointer to the task that is removed
--------------------------------------------------------------------------------*/
Task* remove_from_queue(uint32_t index, TCB* queue);


/*-------------------------------------------------------------------------------
* Function:    	find_task
* Purpose:    	Try to find the task with specified ID in the given queue
* Arguments:	
*		id - ID of the task to find
*		queue - queue in which the task should be searched for
* Returns: 		
*		index of the task in the queue, or -1 (if not found)
--------------------------------------------------------------------------------*/
int32_t find_task(int32_t id, TCB* queue);
