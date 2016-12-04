

#include "common.h"
#include "KrisOS.h"

/*-------------------------------------------------------------------------------
* Stack alignment in bytes
*------------------------------------------------------------------------------*/
#define STACK_ALIGNMENT 8


/*-------------------------------------------------------------------------------
* Stack frame size and register positions, and initial register values
*------------------------------------------------------------------------------*/
#define STACK_FRAME_SIZE 18
#define STACK_FRAME_xPSR 17
#define STACK_FRAME_PC 16
#define STACK_FRAME_LR 15
#define STACK_FRAME_R0 10
#define STACK_FRAME_CONTROL 1
#define STACK_FRAME_EXC_RETURN 0
#define INIT_xPSR 0x01000000			// Only Thumb mode bit set


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
extern uint32_t svc_exc_return;


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
	void* stackBase; 			// Pointer to the base off process' stack
	int32_t id; 				// Task unique identifier
	int32_t priority; 			// Task priority
	int32_t counter;			// Remaining time to sleep and anti-starvation counter
	uint64_t lastRun; 			
} Task;

extern Task* runPtr; 					// Pointer to the task that is currently running


/*-------------------------------------------------------------------------------
* Task control block
*------------------------------------------------------------------------------*/
typedef struct {
	Task* tasks[MAX_TASKS];		// List of pointers to tasks
	uint32_t noOfTasks;			// Current number of tasks 
} TCB;

extern TCB readyQueue; 				// Ready and Blocked task queues
extern TCB blockedQueue;
