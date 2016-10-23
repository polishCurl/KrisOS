/*******************************************************************************
* File:     	svc.c
* Brief:    	C part of the Supervisor Call handler
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	16/10/2016
* Last mod: 	16/10/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"
#include "svc.h"
#include "os.h"
#include "system.h"


/*-------------------------------------------------------------------------------
* Function:    	SVC_Handler_C
* Purpose:    	Part of the SVC_Handler written in C
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void SVC_Handler_C(uint32_t* svcArgs) {
	
	// Extract the SVC number and use it to run the right subroutine
	uint8_t svcNumber = ((uint8_t*) svcArgs[6])[-2];
	switch(svcNumber) {
		case SVC_OS_START: os_init(); break;
		case SVC_UART_SEND: uart_send_char(svcArgs[0]); break;
		case SVC_UART_GET: svcArgs[0] = uart_get_char(); break;
		default: break;
	}
	return;
}
