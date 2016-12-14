/*******************************************************************************
* File:     	KrisOS.h
* Brief:    	Interface to the KrisOS embedded operating system
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		User interface to the KrisOS and KrisOS setup parameters
*******************************************************************************/
#include "common.h"



/*******************************************************************************
* CONFIGURATION
*******************************************************************************/

/*-------------------------------------------------------------------------------
* System timing setup
*------------------------------------------------------------------------------*/
/* Clock source: 
	0 - 16MHz external crystal oscilator 
	1 - 16MHz internal oscilator
	2 - 4MHz internal oscilator (can't drive the Phase-Lock Loop)
	3 - 30kHz internal oscilator (can't drive the PLL)	*/
#define CLOCK_SOURCE 0

// System clock divider (Derived frequency = (200MHz / SYSCLOCK_DIVIDER)
#define SYSCLOCK_DIVIDER 5 	

// OS clock frequency (in Hz)
#define OS_CLOCK_FREQ 1000

// Time quantum size for multitasking (in OS 'ticks')
#define TIME_SLICE 50



/*-------------------------------------------------------------------------------
* Interrupt controller setup
*------------------------------------------------------------------------------*/
#define MAX_IRQ_PRIO 1
#define MIN_IRQ_PRIO 6



/*-------------------------------------------------------------------------------
* Task scheduler setup
*------------------------------------------------------------------------------*/
// Number of different priority levels which are allowed for user task. The priorities
// are from 0 to (TASK_PRIO_LVL_NO - 1)
#define TASK_PRIO_LVL_NO 4 	

// Possible task states
typedef enum {
	RUNNING = TASK_PRIO_LVL_NO + 4,
	READY = TASK_PRIO_LVL_NO + 3,
	DELAYED = TASK_PRIO_LVL_NO + 1,
	BLOCKED = TASK_PRIO_LVL_NO + 2,
} TaskState;

// Task control block
typedef struct Task {
	uint32_t sp; 					// Stack pointer value
	struct Task* next; 				// Pointer to the next task in a queue
	struct Task* previous; 			// Pointer to the previous task in a queue 
	int32_t id; 					// Task unique identifier
	uint8_t priority; 				// Task priority
	TaskState status; 				// Current task status
	uint64_t waitCounter;			// Remaining time to sleep (in OS 'ticks')
	void* stackBase; 				// Pointer to the base of process' stack
} Task;



/*-------------------------------------------------------------------------------
* Heap Manager setup
*------------------------------------------------------------------------------*/
// Heap size (in bytes)
#define HEAP_SIZE 10000		

// Minimum heap free block size that can still be divided into smaller ones
#define MIN_BLOCK_SIZE (4 * sizeof(HeapBlock))



/*-------------------------------------------------------------------------------
* Serial Monitor setup (UART0 used)
*------------------------------------------------------------------------------*/
// UART0 baud rate
#define SERIAL_MONITOR_BAUD_RATE 9600

/* UART0 word length
	0 - 5bits
	1 - 6bits
	2 - 7bits
	3 - 8bits	*/
#define SERIAL_MONITOR_WORD_LEN 3	

/* UART0 parity checking
 	0 - no parity checking
 	1 - parity bit added (even or odd) 	*/
#define SERIAL_MONITOR_D0_PARITY_CHECK 0	

/* UART0 parity
 	0 - odd
 	1 - even	*/
#define SERIAL_MONITOR_PARITY 0	

/* UART0 number of stop bits
 	0 - one stop bit
 	1 - two stop bits	*/
#define SERIAL_MONITOR_STOP_BITS 0	



/*******************************************************************************
* SVC NUMBERS
*******************************************************************************/
#define SVC_OS_INIT 0				// Initialise the operating system
#define SVC_OS_START 1 				// Start the operating system
#define SVC_CREATE_TASK 2 			// Create a task using heap
#define SVC_DECLARE_TASK 3 			// Create a task using static memory
#define SVC_DELAY_TASK 4 			// Delay given task by a multiple of OS 'ticks'
#define SVC_SUSPEND_TASK 5 			// Suspend the task with given ID
#define SVC_RESUME_TASK 6 			// Resume the task with given ID



/*******************************************************************************
* KRISOS INTERFACE
*******************************************************************************/

/*-------------------------------------------------------------------------------
* Function:    	KrisOS_init
* Purpose:    	Initialise the operating system
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void __svc(SVC_OS_INIT) KrisOS_init(void);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_start
* Purpose:    	Start the operating system
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void __svc(SVC_OS_START) KrisOS_start(void);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_create_task
* Purpose:    	Dynamically create a user task and add it to the ready queue
* Arguments:	
*		startAddr - starting address of the task to add
*		stackSize - stack size (in bytes) required by the task
*		priority - task priority
* Returns: 		
*		pointer to the task declared
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_CREATE_TASK) KrisOS_create_task(void* startAddr,size_t stackSize, 
												  uint32_t priority);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_declare_task
* Purpose:    	Register a statically allocated task at the scheduler
* Arguments:	
* 		toDeclare - pointer to the pre-allocated task control block of the task to declare
*		startAddr - starting address of the task to add
*		stackBase - pointer to the private stack area to be used by the task to declare 
*		priority - task priority
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_DECLARE_TASK) KrisOS_declare_task(Task* toDeclare, void* startAddr,
											         void* stackBase, int32_t priority);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_delay_task
* Purpose:    	Delay the currently running task by 'ms10' 1/100sec
* Arguments:	
*		ms10 - number of tens of milliseconds to suspend the execution of calling task
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_DELAY_TASK) KrisOS_delay_task(uint32_t ms10);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_suspend_task
* Purpose:    	Suspend the task given
* Arguments:	
*		toSuspend - pointer to the task to suspend. If NULL the calling task suspends itself
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_SUSPEND_TASK) KrisOS_suspend_task(Task* toSuspend);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_resume_task
* Purpose:    	Resume the task given
* Arguments:	
*		toSuspend - pointer to the task to resume.
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_RESUME_TASK) KrisOS_resume_task(Task* toResume);

