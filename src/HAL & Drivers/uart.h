/*******************************************************************************
* File:     	uart.h
* Brief:    	Header file for uart.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	11/03/2017
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"
#ifdef USE_UART



/*-------------------------------------------------------------------------------
* Function:    	uart_init
* Purpose:    	Initialisation function for the UART0 interface (Serial monitor 
*				over USB).
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void uart_init(void);



/*-------------------------------------------------------------------------------
* Function:    	sendChar
* Purpose:    	Send character over UART
* Arguments:
*		character - character to send
* Returns: 		-	
--------------------------------------------------------------------------------*/
void uart_send_char(uint8_t character);



/*-------------------------------------------------------------------------------
* Function:    	getChar
* Purpose:    	Get character from UART
* Arguments: 	-
* Returns: 		
*		character received
--------------------------------------------------------------------------------*/
uint8_t uart_get_char(void);

#endif
