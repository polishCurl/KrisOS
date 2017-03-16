/*******************************************************************************
* File:     	os.h
* Brief:    	Header file for os.c.
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	13/03/2017
*
* Note: 		
*******************************************************************************/
#include "common.h"



/*-------------------------------------------------------------------------------
* Kernel control block
* Contains useful statistics concerning the OS usage and data internal to the OS
* that is essential for the correct operation of the OS
--------------------------------------------------------------------------------*/
typedef struct {
	// Flag indicating if the OS is running
	uint8_t isRunning; 					
	// OS ticks, counter incremented on each OS timer interrupt. Used for 
	// time-keeping by KrisOS
	uint64_t ticks; 				
	// The time length of the longest mutex lock time recorded (useful performance
	// figure) and the total number of mutexes in use
#if defined SHOW_DIAGNOSTIC_DATA && defined USE_MUTEX
	uint32_t maxMtxCriticalSection; 	
	uint32_t totalMutexNo; 				
#endif
	// Total number of semaphores in use
#if defined SHOW_DIAGNOSTIC_DATA && defined USE_SEMAPHORE
	uint32_t totalSemNo; 				
#endif
	// Total number of queues in use
#if defined SHOW_DIAGNOSTIC_DATA && defined USE_QUEUE
	uint32_t totalQueueNo;				
#endif	
} Kernel; 

extern Kernel KrisOS;



/*-------------------------------------------------------------------------------
* Function:    	os_init
* Purpose:    	Initialise the operating system
* Arguments:	-
* Returns: 
* 		exit status		
--------------------------------------------------------------------------------*/
uint32_t os_init(void);



/*-------------------------------------------------------------------------------
* Function:    	os_start
* Purpose:    	Start the operating system by redirecting the execution to the top
*				priority ready task in the scheduler
* Arguments:	-
* Returns: 
* 		exit status	
--------------------------------------------------------------------------------*/
uint32_t os_start(void);

