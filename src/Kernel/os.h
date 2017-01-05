/*******************************************************************************
* File:     	os.h
* Brief:    	os.c header file
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		Method declarations visible to OS users for accessing its 
*				functionality
*******************************************************************************/
#include "common.h"



/*-------------------------------------------------------------------------------
* Kernel status struct
--------------------------------------------------------------------------------*/
typedef struct {
	uint8_t isRunning; 					// Flag indicating if the OS is running
	uint64_t ticks; 					// OS ticks, incremented on every timer interrupt
#ifdef SHOW_DIAGNOSTIC_DATA
	uint32_t idleTime; 					// Idle time counter (ms)
	uint32_t contextSwitchNo; 			// Context switch counter
	uint32_t maxMtxCriticalSection; 	// Longest critical section implemented using mutexes
	uint32_t totalTaskNo; 				// Total number of tasks declared
	uint32_t totalMutexNo; 				// Total number of mutexes in use
#endif
} Kernel; 

extern Kernel KrisOS;



/*-------------------------------------------------------------------------------
* Idle task
--------------------------------------------------------------------------------*/
extern Task idleTask;



/*-------------------------------------------------------------------------------
* Function:    	os_init
* Purpose:    	KrisOS initialisation method
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
extern void os_init(void);



/*-------------------------------------------------------------------------------
* Function:    	os_start
* Purpose:    	Start the operating system
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_start(void);



/*-------------------------------------------------------------------------------
* Function:		os_sleep
* Purpose:    	Put the operating system into power-saving mode
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void os_sleep(void);



/*-------------------------------------------------------------------------------
* Function:		idle_task
* Purpose:    	Idle task. Lowest priority task for power saving.
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void idle_task(void);
