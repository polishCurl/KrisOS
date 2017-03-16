/*******************************************************************************
* File:     	scheduler.h
* Brief:    	Header file for scheduler.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	04/11/2016
* Last mod: 	14/03/2017
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"


/*-------------------------------------------------------------------------------
* Task's private stack byte alignment
*------------------------------------------------------------------------------*/
#define STACK_ALIGNMENT 8 



/*-------------------------------------------------------------------------------
* Word address offsets from the task's stack base where saved values of some
* important registers are stored. These register values have to be specified during
* task creation
*------------------------------------------------------------------------------*/
#define STACK_FRAME_SIZE 18 			
#define STACK_FRAME_xPSR 17				
#define STACK_FRAME_PC 16 				
#define STACK_FRAME_LR 15
#define STACK_FRAME_R0 10
#define STACK_FRAME_CONTROL 1
#define STACK_FRAME_EXC_RETURN 0



/*-------------------------------------------------------------------------------
* Initial value of xPSR register set for each task (only Thumb mode bit set)
*------------------------------------------------------------------------------*/
#define INIT_xPSR 0x01000000			



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
* Scheduler definition
*------------------------------------------------------------------------------*/
typedef struct {
	Task* runPtr; 							// Task currently running
	Task* topPrioTask; 						// Current top priority task (next to run)
	uint32_t svcExcReturn;					// Temporary store for the SVC call return value
	Task* ready; 							// Ready queue 
	Task* blocked; 							// Blocked tasks queue
	int32_t lastIDUsed; 					// Last task ID assigned (used for unique ID assignment)
	uint8_t preemptFlag; 					// Time sliced preemption flag. 1 if 
#ifdef SHOW_DIAGNOSTIC_DATA 				// preemption should be performed.
	uint32_t idleTime; 						// Number of OS 'ticks' the idle task has been running for
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
* Purpose:    	Wake all the tasks from the blocked queue which are now ready
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void scheduler_wake_tasks(void);



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
						  uint8_t isPrivileged);
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
					        uint8_t priority, uint8_t isPrivileged);



/*-------------------------------------------------------------------------------
* Function:    	task_sleep
* Purpose:    	Suspend the execution of the currently running task for 
* 				specified amount of OS ticks.
* Arguments: 	
*		delay - number of OS 'ticks' do suspend execution of the task by
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t task_sleep(uint64_t delay);



/*-------------------------------------------------------------------------------
* Function:    	task_delete
* Purpose:    	Remove the currently running (calling) task from the scheduler and
*				delete it, if allocated dynamically.
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
*		toInit - pointer to the task struct to have the stack frame initialised
*		startAddr - address of the first instruction of the task to initialise
*		isPrivileged - 1 if the task to initialise should be priviliged, 0 otherwise
*		priority - priority of the task to initialise
* Returns: 
*		exit status
--------------------------------------------------------------------------------*/
uint32_t task_init(Task* toInit, void* startAddr, uint8_t isPrivileged, uint8_t priority);



/*-------------------------------------------------------------------------------
* Function:    	task_complete_handler
* Purpose:    	Code to be executed should a task return
* Arguments: 	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void task_complete_handler(void);
