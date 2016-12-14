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
#include "KrisOS.h"



/*-------------------------------------------------------------------------------
* Task's private stack definitions
*------------------------------------------------------------------------------*/
#define STACK_ALIGNMENT 8 				// Stack alignment
#define STACK_FRAME_SIZE 18 			// Stack frame size (to be saved and restored)
#define STACK_FRAME_xPSR 17				// Offsets for selected registers in the stack
#define STACK_FRAME_PC 16 				// frame
#define STACK_FRAME_LR 15
#define STACK_FRAME_R0 10
#define STACK_FRAME_CONTROL 1
#define STACK_FRAME_EXC_RETURN 0
#define INIT_xPSR 0x01000000			// Initial value of xPSR (only Thumb mode bit set)



/*-------------------------------------------------------------------------------
* Exception return possible values
*------------------------------------------------------------------------------*/
#define	EXC_RETURN_0 0xFFFFFFF1U		// Return to Handler mode, exception return uses non-floating-point 
										// state from the MSP and execution uses MSP after return.
#define	EXC_RETURN_1  0xFFFFFFF9U		// Return to Thread mode, exception return uses non-floating-point state
										// from MSP and execution uses MSP after return.
#define	EXC_RETURN_2  0xFFFFFFFDU 		// Return to Thread mode, exception return uses non-floating-point state 
										// from the PSP and execution uses PSP after return.
#define	EXC_RETURN_3  0xFFFFFFE1U 		// Return to Handler mode, exception return uses floating-point-state 
										// from MSP and execution uses MSP after return.
#define	EXC_RETURN_4  0xFFFFFFE9U		// Return to Thread mode, exception return uses floating-point state from 
										// MSP and execution uses MSP after return.
#define	EXC_RETURN_5  0xFFFFFFEDU 		// Return to Thread mode, exception return uses floating-point state from
										// PSP and execution uses PSP after return.
										
// Store the value of exception return for SVC calls
extern uint32_t svc_exc_return;



/*-------------------------------------------------------------------------------
* Scheduler
*------------------------------------------------------------------------------*/
// Time sliced preemption enable bit in scheduler status field. If 1, time-slice 
// preemption should be performed at the end of the time slice.
#define TIME_PREEMPT_Pos 0 					
											
// Number of queues other than ready queues for each priority
#define NON_READY_QUEUES 2

// Total number of task queues in the scheduler. Ready queues for each priority 
// (+1 because of lowest priority idle task) and  queues for delayed, blocked etc. tasks
#define TOTAL_QUEUES_NO (TASK_PRIO_LVL_NO + 1 + NON_READY_QUEUES)

// Scheduler definition
typedef struct {
	Task* runPtr; 							// Task currently running
	Task* topPrioTask; 						// Task with currently highest priority
	Task* queues[TOTAL_QUEUES_NO]; 			// Task queues
	int32_t lastIDUsed; 					// Last task ID assigned
	uint32_t status; 						// Scheduler status bits
} Scheduler; 								

extern Scheduler scheduler;




/*-------------------------------------------------------------------------------
* Function:    	scheduler_init
* Purpose:    	Initialise the scheduler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_init(void);



/*-------------------------------------------------------------------------------
* Function:    	scheduler_run
* Purpose:    	Run the scheduler to determine the next task to run
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_run(void);



/*-------------------------------------------------------------------------------
* Function:    	scheduler_wake_tasks
* Purpose:    	Wake the task from the delayed queue that are now ready
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_wake_tasks(void);



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
					uint32_t isPrivileged);



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
					  int32_t priority, uint32_t isPrivileged);



/*-------------------------------------------------------------------------------
* Function:    	task_delay
* Purpose:    	Delay the currently running task by 'delay' number of OS ticks
* Arguments: 	
*		delay - number of OS 'ticks' do suspend execution of the task by
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_delay(uint64_t delay);



/*-------------------------------------------------------------------------------
* Function:    	task_suspend
* Purpose:    	Suspend the task given
* Arguments:	
*		toSuspend - pointer to the task to suspend. If NULL the calling task suspends itself
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_suspend(Task* toSuspend);



/*-------------------------------------------------------------------------------
* Function:    	task_resume
* Purpose:    	Resume the task given
* Arguments:	
*		toSuspend - pointer to the task to resume.
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_resume(Task* toResume);



/*-------------------------------------------------------------------------------
* Function:    	task_insert_at_queue_start
* Purpose:    	Insert the task given at the beginning of the queue specified
* Arguments: 	
*		queue - queue to update
* 		toInsert - task to insert
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_insert_at_queue_start(Task** queue, Task* toInsert);



/*-------------------------------------------------------------------------------
* Function:    	task_remove_from_queue
* Purpose:    	Remove the task given from the queue it belongs to
* Arguments: 
* 		toRemove - task to remove
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_remove_from_queue(Task* toRemove);



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
uint32_t task_frame_init(Task* toInit, void* startAddr, uint32_t isPrivileged);
