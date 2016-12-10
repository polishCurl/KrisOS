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
typedef struct {
	Task* runPtr; 							// Task currently running
	Task* topPrioTask; 						// Task with currently highest priorityu
	Task* readyTasks[TASK_PRIO_LVL_NO+1]; 	// Ready queues for each priority level
	Task* delayedTasks; 					// Blocked queue for delayed tasks
	uint64_t nextToWake; 					// Time of the soonest task from delayedTasks 
											// to be ready
	Task* suspendedTasks; 					// Suspended tasks
	int32_t lastIDUsed; 					// Last task ID assigned (used for unique 											
} Scheduler; 								// ID generation)

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
*		unique nonzero ID number of the task created, 0 otherwise
--------------------------------------------------------------------------------*/
int32_t task_create(void* startAddr, size_t stackSize, uint32_t priority,
					uint32_t isPrivileged);



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
					 uint32_t isPrivileged);



/*-------------------------------------------------------------------------------
* Function:    	task_delay
* Purpose:    	Delay given task by specified amount of OS 'ticks'
* Arguments: 	
*		delay - number of OS 'ticks' do suspend execution of the task by
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_delay(uint64_t delay);



/*-------------------------------------------------------------------------------
* Function:    	task_frame_init
* Purpose:    	Initialise the stak frame of the task selected
* Arguments: 	
*		toInit - pointer to the task to have its stack frame initialised
*		startAddr - starting address of the task to have its stack frame initialised
*		privileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		-
--------------------------------------------------------------------------------*/
void task_frame_init(Task* toInit, void* startAddr, uint32_t privileged);



/*-------------------------------------------------------------------------------
* Function:    	add_to_queue
* Purpose:    	Add given task to the queue specified
* Arguments: 	
*		queue - queue to add the task to
* 		toAdd - task to add
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t add_to_queue(Task** queue, Task* toAdd);



/*-------------------------------------------------------------------------------
* Function:    	remove_from_queue
* Purpose:    	Remove the task with given ID from the queue specified.
* Arguments: 	
*		queue - address of the pointer to the queue to remove the task from
* 		id - ID of the task to remove
* Returns: 
* 		pointer to the task removed
--------------------------------------------------------------------------------*/
Task* remove_from_queue(Task** queue, int32_t id);
