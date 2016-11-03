/*******************************************************************************
* File:     	task_scheduling.h
* Brief:    	Header file for task_scheduling.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	28/10/2016
* Last mod: 	28/10/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"


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
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t suspend_task(int32_t toSuspendID);


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
* Function:    	test_context_switch
* Purpose:    	Check if context switch should be performed
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void test_context_switch(void);

