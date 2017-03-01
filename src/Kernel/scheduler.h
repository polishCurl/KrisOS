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



/*-------------------------------------------------------------------------------
* Scheduler
*------------------------------------------------------------------------------*/
// Scheduler definition
typedef struct {
	Task* runPtr; 							// Task currently running
	Task* topPrioTask; 						// Current top priority task
	uint32_t svcExcReturn;					// SVC call exception return value
	Task* ready; 							// Ready queue 
	Task* blocked; 							// Blocked tasks queue
	int32_t lastIDUsed; 					// Last task ID assigned
	uint8_t preemptFlag; 					// Time sliced preemption flag. 1 if 
#ifdef SHOW_DIAGNOSTIC_DATA 				// preemption should be performed.
	uint32_t idleTime; 						// Idle time counter (ms)
	uint32_t contextSwitchNo; 				// Context switch counter
	uint32_t totalTaskNo; 					// Total number of tasks declared
	Task* taskRegistry[TASK_REGISTRY_SIZE]; // Size of the task registry 
#endif
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
* Purpose:    	Run the scheduler to determine the next task to run.
* Arguments:	-
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t scheduler_run(void);



/*-------------------------------------------------------------------------------
* Function:    	scheduler_wake_tasks
* Purpose:    	Wake the task from the delayed queue that are now ready
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_wake_tasks(void);



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
Task* task_create_dynamic(void* startAddr, size_t stackSize, uint32_t priority,
						  uint32_t isPrivileged);
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
					        uint32_t priority, uint32_t isPrivileged);



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
uint32_t task_sleep(uint64_t delay, TaskState state);



/*-------------------------------------------------------------------------------
* Function:    	task_delete
* Purpose:    	Permanently remove the calling task.
* Arguments: 	-
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_delete(void);



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
uint32_t task_add(Task** queue, Task* toInsert);



/*-------------------------------------------------------------------------------
* Function:    	task_remove
* Purpose:    	Remove the task given from the queue specified
* Arguments: 
*		queue - queue to update
* 		toRemove - task to remove
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_remove(Task** queue, Task* toRemove);



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
uint32_t task_init(Task* toInit, void* startAddr, uint32_t isPrivileged, uint32_t priority);



/*-------------------------------------------------------------------------------
* Function:    	task_complete_handler
* Purpose:    	Code to be executed when the running task returns. 
* Arguments: 	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void task_complete_handler(void);
