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
* Flag indicating whether the operating system is running
--------------------------------------------------------------------------------*/
extern uint32_t OS_RUNNING;



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
