/*******************************************************************************
* File:     	task.c
* Brief:    	
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/10/2016
* Last mod: 	21/10/2016
*
* Note: 		
*
*	Registers saved by the software (PendSV_Handler):
*	+------------+
*	| EXC_RETURN | <- Saved SP (orig. SP - 72 bytes), sp in Task Control Block 
*	|  Control   |    initially points here
*	|     R8     | 
*	|     R9     |
*	|     R10    |
*	|     R11    |
*	|     R4     |
*	|     R5     |
*	|     R6     |
*	|     R7     |
*	+------------+
*
*	Exception frame saved by the NVIC hardware onto stack:
*	+------+
*	|  R0  | <- SP after entering interrupt (orig. SP - 32 bytes)
*	|  R1  |
*	|  R2  |
*	|  R3  |
*	|  R12 |
*	|  LR  |
*	|  PC  |
*	| xPSR |
*	|      | <- SP before interrupt (orig. SP), bottom of task's stack
*	+------+
*
*******************************************************************************/
#include "common.h"
#include "KrisOS.h"
#include "system.h"
#include "task_scheduling.h"


/*-------------------------------------------------------------------------------
* Macro for double-word access
*------------------------------------------------------------------------------*/
#define WORD_ACCESS(ADR) (*((volatile uint64_t*) (ADR)))
#define STACK_ALIGNMENT 8


/*-------------------------------------------------------------------------------
* Stack frame size and register positions, and initial register values
*------------------------------------------------------------------------------*/
#define STACK_FRAME_SIZE 18
#define STACK_FRAME_xPSR 17
#define STACK_FRAME_LR 16
#define STACK_FRAME_PC 16
#define STACK_FRAME_R0 10
#define STACK_FRAME_CONTROL 1
#define STACK_FRAME_EXC_RETURN 0
#define INIT_xPSR 0x01000000				// Only Thumb mode bit set


/*-------------------------------------------------------------------------------
* Exception return
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
uint32_t svc_exc_return;


/*-------------------------------------------------------------------------------
* Task status
*------------------------------------------------------------------------------*/
typedef enum {
	RUNNING = 0,				// task is currently executed
	READY 	= 1, 				// task is ready to run
	BLOCKED = 2					// task is currently blocked
} TaskStatus;


/*-------------------------------------------------------------------------------
* Task definition
*------------------------------------------------------------------------------*/
typedef struct Task {
	uint32_t sp; 				// Stack pointer value
	struct Task *next; 			// Next task to run
	TaskStatus status; 			// Current task's status
	uint32_t id; 				// Task unique identifier
	uint32_t priority; 			// Task priority
	uint32_t sleep_time;		// Remaining time to sleep (in clock ticks)
} Task;

Task* runPtr; 					// Pointer to the task that is currently running


/*-------------------------------------------------------------------------------
* Task control block
*------------------------------------------------------------------------------*/
typedef struct {
	Task* tasks[MAX_TASKS];		// List of pointers to tasks
	uint32_t noOfTasks;			// Current number of tasks 
} TCB;

TCB readyQueue; 				// Ready and Blocked task queues
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
}


/*-------------------------------------------------------------------------------
* Function:    	os_start
* Purpose:    	Start the operating system
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_start(void) {
	
	// Helper pointer for specifying register address within the task's stack frame
	uint32_t* taskFramePtr; 	
	
	// Index of the first task to run
	runPtr = readyQueue.tasks[0];
	
	// Set the initial value of svc_exc_return, CONTROL register as well as PSP.
	// PSP should be pointing to the position of PC in the stack frame of the first task
	// to run.
	__set_psp(runPtr->sp + (STACK_FRAME_R0 << WORD_ACCESS_SHIFT)); 
	taskFramePtr = (uint32_t*) (runPtr->sp + (STACK_FRAME_CONTROL << WORD_ACCESS_SHIFT));
	__set_control(*taskFramePtr);
	taskFramePtr = (uint32_t*) runPtr->sp;
	svc_exc_return = *taskFramePtr;

	// Schedule the tasks
	run_scheduler();
	
	// Set up periodic interrupts
	systick_config(TIME_SLICE);	
}


/*-------------------------------------------------------------------------------
* Function:    	run_scheduler
* Purpose:    	Run the scheduler
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void run_scheduler(void) {

// Round-robin scheduler	
#if TASK_SCHEDULER == 1
	
	// Chain all the ready task into a circle
	uint32_t i;
	for (i = 0; i < readyQueue.noOfTasks; i++) {
		readyQueue.tasks[i]->next = readyQueue.tasks[(i + 1) % readyQueue.noOfTasks];
	}
#endif
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
uint32_t create_task(void* startAddr, uint32_t id, uint32_t priority, 
					 size_t stackSize, uint32_t privileged) {
	
	// Helper pointer for specifying initial register values in the stack frame
	uint32_t* taskFramePtr; 					 
						 
	// Test if the arguments are valid or the imit of tasks is reached
	if (priority > MIN_TASK_PRIO || readyQueue.noOfTasks >= MAX_TASKS)
		return EXIT_FAILURE;
	TEST_NULL_POINTER(startAddr)
	
	// Adjust the stack size to comply with the stack alignment
	if (stackSize % STACK_ALIGNMENT)
		stackSize = stackSize + (STACK_ALIGNMENT - stackSize % STACK_ALIGNMENT);
	
	__start_critical();
	
	// Allocate memory for the task metadata and its stack memory
	readyQueue.tasks[readyQueue.noOfTasks] = (Task*) malloc(sizeof(Task));
	TEST_NULL_POINTER(startAddr)
	readyQueue.tasks[readyQueue.noOfTasks]->sp = (uint32_t) malloc(stackSize);
	TEST_NULL_POINTER(readyQueue.tasks[readyQueue.noOfTasks]->sp)
	
	// Assign the task's priority, status and id
	readyQueue.tasks[readyQueue.noOfTasks]->priority = priority;
	readyQueue.tasks[readyQueue.noOfTasks]->id = id;
	readyQueue.tasks[readyQueue.noOfTasks]->status = READY;
	
	// Set the initial stack pointer
	readyQueue.tasks[readyQueue.noOfTasks]->sp = readyQueue.tasks[readyQueue.noOfTasks]->sp + 
												 stackSize - (STACK_FRAME_SIZE << WORD_ACCESS_SHIFT);
	
	// Set the initial PC to the starting address of task's code
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp + 
				   (STACK_FRAME_PC << WORD_ACCESS_SHIFT));	
	*taskFramePtr = (uint32_t) startAddr;
	
	// Set the initial value of xPSR
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp + 
				   (STACK_FRAME_xPSR << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = INIT_xPSR;
	
	// Set the initial value of LR, make LR point to a handler for removing the
	// tasks that terminate (don't have infinite while loop)
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp + 
				   (STACK_FRAME_LR << WORD_ACCESS_SHIFT));				   
	//*taskFramePtr = INIT_xPSR;
	
	// Set the initial value of EXC_RETURN, which specifies the use of stack pointer PSP or MSP and floating 
	// point unit before and after return from this exception
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp +
				   (STACK_FRAME_EXC_RETURN << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = EXC_RETURN_2;
	
	// Set the initial value of control register according to privilege level. This will
	// specify access level of the task that is being declared
	taskFramePtr = (uint32_t*) (readyQueue.tasks[readyQueue.noOfTasks]->sp +
		           (STACK_FRAME_CONTROL << WORD_ACCESS_SHIFT));				   
	*taskFramePtr = privileged ? 0x2 : 0x3;
	
	// Update the task count and run the scheduler
	readyQueue.noOfTasks++;
	run_scheduler();
	
	__end_critical();

	return EXIT_SUCCESS;
}
					 

/*-------------------------------------------------------------------------------
* Function:    	create_task
* Purpose:    	Create a user-defined task and add it to the scheduler
* Arguments:	
*		toSuspendID - unique task identifier
*		priority - the higher the number the lower the priority
*		stack_size - stack size (in bytes) required by the task
*		privileged - 1 if privileged access level (system task), 0 otherwise
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
void suspend_task(uint32_t toSuspendID) {
	
	
}

					 





