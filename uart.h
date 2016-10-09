/*******************************************************************************
* File:     	uart.h
* Brief:    	Header file for uart.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"


/*-------------------------------------------------------------------------------
* Function:    	uart_init
* Purpose:    	Initialisation of UART interface.
* Arguments:
*		baudRate - controls the number of bits sent per second
*		uartWordLen - UART word length: 5bits (0), 6bits (1), 7bits (2) or 8bits (3)
*		parityUsed - no parity checking/generation (0) or parity used (1)
*		oddEven - parity: odd (0), even (1)
*		stopBits - number of stop bits: one stop bit (0), two stop bits (1) 
* Returns: 		-	
--------------------------------------------------------------------------------*/
void uart_init(uint32_t baudRate, uint32_t uartWordLen, uint32_t parityUsed, 
			    uint32_t oddEven, uint32_t stopBits);


/*-------------------------------------------------------------------------------
* Function:    	sendChar
* Purpose:    	Send character over UART
* Arguments:
*		character - character to send
* Returns: 		-	
--------------------------------------------------------------------------------*/
void send_char(uint8_t character);


/*-------------------------------------------------------------------------------
* Function:    	getChar
* Purpose:    	Get character from UART
* Arguments: 	-
* Returns: 		
*		character received
--------------------------------------------------------------------------------*/
uint8_t get_char(void);
