/*******************************************************************************
* File:     	svc.c
* Brief:    	Supervisor call number definitions
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	16/10/2016
* Last mod: 	16/10/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"


/*-------------------------------------------------------------------------------
* SVC numbers
--------------------------------------------------------------------------------*/
#define SVC_OS_INIT 0				// start the operating system
#define SVC_UART_SEND 1 			// send a character over uart
#define SVC_UART_GET 2 				// receive a character from uart
#define SVC_ADD_USER_TASK 3 		// add a user-defined task to the scheduler
#define SVC_OS_START 4 				// start the operating system
#define SVC_SUSPEND_USER_TASK 5 	// suspend a user-defined task
#define SVC_RESUME_USER_TASK 6		// resume execution of a user defined task
#define SVC_DELETE_USER_TASK 7		// delete a use-defined task
#define SVC_DELAY_USER_TASK 8		// delay a use-defined task


/*-------------------------------------------------------------------------------
* SVC calls renaming for ease of use
--------------------------------------------------------------------------------*/
void __svc(SVC_UART_SEND) send_char(uint8_t character);
uint8_t __svc(SVC_UART_GET) get_char(void);
