/*******************************************************************************
* File:     	uart.c
* Brief:    	UART driver for serial asynchronous communication
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"
#include "time.h"
#include "tm4c123gh6pm.h"


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
			    uint32_t oddEven, uint32_t stopBits) {
					
	// Integer and fractional part of the system clock divider for generating
	// the desired baudRate
	int32_t dividerInt; 				
	float32_t dividerFrac;			
	
	// Calculate the divider, separate the integer and fractional parts
	dividerFrac = (float32_t) SYSTEM_CLOCK_FREQ / 16 / baudRate;
	dividerInt = (int32_t) dividerFrac;
	dividerFrac -= dividerInt;
	dividerFrac = dividerFrac * 64 + 0.5f;
	
	SYSCTL->RCGCUART |= 0x1;		// Activate UART0
	SYSCTL->RCGCGPIO |= 0x1; 		// UART0 on port A
	GPIOA->AFSEL |= 0x3;			// set GPIOA pins 1-0 to alternative function
	GPIOA->DEN |= 0x3;				// enable digitial I/O on pins 1-0 of GPIOA
	GPIOA->PCTL |= (1 << 4) | (1 << 0);
					
	UART0->CTL &= ~(0x1); 			// Disable UART device for time of configuration
	UART0->IBRD = dividerInt; 		// divider, integer part
	UART0->FBRD = (int32_t) dividerFrac; // divider, fractional part 	
	
	// Set the parameters of serial communication
	UART0->LCRH = (parityUsed << 7) | (uartWordLen << 5) | (stopBits << 3) | 
				   (oddEven << 2) | (parityUsed << 1);  
	UART0->CTL |= 0x0301;			// enable RXE, TXE, UARTEN
}
				

/*-------------------------------------------------------------------------------
* Function:    	sendChar
* Purpose:    	Send character over UART
* Arguments:
*		character - character to send
* Returns: 		-	
--------------------------------------------------------------------------------*/
void send_char(uint8_t character) {
	while (UART0->FR & (1 << 5)); 	// wait for TX buffer to be not full
	UART0->DR = character;
}

/*-------------------------------------------------------------------------------
* Function:    	getChar
* Purpose:    	Get character from UART
* Arguments: 	-
* Returns: 		
*		character received
--------------------------------------------------------------------------------*/
uint8_t get_char(void) {
	while (UART0->FR & (1 << 4)); 	// wait for RX buffer to be not empty
	return (uint8_t) UART0->DR & 0xFF;
}
