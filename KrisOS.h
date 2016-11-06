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
#include "svc.h"


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

// Clock frequency in Hz
extern uint32_t OS_CLOCK_FREQ; 

// Quantum (time slice) length in number of clock cycles
#define TIME_SLICE (OS_CLOCK_FREQ / 10)



/*-------------------------------------------------------------------------------
* Task scheduler setup
*------------------------------------------------------------------------------*/
// Maximum number of tasks that can be run concurrently
#define MAX_TASKS 10

// Minimum and maximum task priority
#define MIN_TASK_PRIO 7
#define MAX_TASK_PRIO 0

// Difference of time slice size between two tasks with priorities differing by 1
#define TIME_SLICE_MUL_PRIORITY 1


/*-------------------------------------------------------------------------------
* Heap Manager setup
*------------------------------------------------------------------------------*/
/* Heap manager used:
	1. Simplest, array-based no heap freeing
 	2. Linked list of free blocks, allows freeing but can lead to fragmentation */
#define HEAP_MANAGER 2

// Heap size (in bytes)
#define HEAP_SIZE 20000		



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
* Purpose:    	Create a user-defined task and add it to the scheduler
* Arguments:	
*		start_address - starting address of the task to add
*		task_id - unique task identifier
*		task_prio - the higher the number the lower the priority
*		stack_size - stack size (in bytes) required by the task
* Returns: 		-
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_ADD_USER_TASK) KrisOS_create_task(void* start_address, int32_t task_id, 
													 int32_t task_prio, size_t stack_size);


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_suspend_task
* Purpose:    	Suspend the given task
* Arguments:	
*		toSuspendID - ID of the task to suspend
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_SUSPEND_USER_TASK) KrisOS_suspend_task(uint32_t toSuspendID);


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_resume_task
* Purpose:    	Resume the execution of given task
* Arguments:	
*		toResumeID - ID of the task to resume
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_RESUME_USER_TASK) KrisOS_resume_task(uint32_t toResumeID);


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_delete_task
* Purpose:    	Pernamently remove given task so it is no longer possible to schedule
* Arguments:	
*		toDeleteID - ID of the task to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_DELETE_USER_TASK) KrisOS_delete_task(uint32_t toDeleteID);


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_delay_task
* Purpose:    	Delay the execution of given task
* Arguments:	
*		toDelayID - ID of the task to resume
*		ticks - Number of OS ticks to delay by
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_DELAY_USER_TASK) KrisOS_delay_task(uint32_t toDelayID, int32_t ticks);
