/*******************************************************************************
* File:     	uart.c
* Brief:    	UART driver for serial asynchronous communication
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	04/12/2016
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"
#include "system.h"
#include "kernel.h"
#ifdef USE_UART



/*-------------------------------------------------------------------------------
* Uart I/O stream 'as file' declarations 
--------------------------------------------------------------------------------*/
__FILE uart;



/*-------------------------------------------------------------------------------
* Mutex on UART
--------------------------------------------------------------------------------*/
#ifdef USE_MUTEX
	Mutex uartMtx;
#endif
	


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
	
	// Activate UART0 on port A and disable UART device for time of configuration
	SYSCTL->RCGCUART |= (1 << RCGC_UART0);		
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTA); 
	UART0->CTL &= ~(1 << CTL_UARTEN); 							   
				   
	// Calculate the divider, separate the integer and fractional parts
	dividerFrac = (float32_t) SYSTEM_CLOCK_FREQ / 16 / baudRate;
	dividerInt = (int32_t) dividerFrac;
	dividerFrac -= dividerInt;
	dividerFrac = dividerFrac * 64 + 0.5f;
	UART0->IBRD = dividerInt; 							
	UART0->FBRD = (int32_t) dividerFrac; 
	
	// Enable port A pins 1 and 0. Set GPIOA pins 1-0 to alternative function and
	// enable digitial I/O on pins 1-0 of GPIOA	
	GPIOA->PCTL &= ~((0xFU << PCTL_PMC1) | (0xFU << PCTL_PMC0));
	GPIOA->PCTL |= (1 << PCTL_PMC1) | (1 << PCTL_PMC0); 
	GPIOA->AFSEL |= (1 << PIN0) | (1 << PIN1);	 
	GPIOA->DEN |= (1 << PIN0) | (1 << PIN1);				   
				   
	// Set the parameters of serial communication. Parity, stop bit, word length etc...
	UART0->LCRH = (parityUsed << LCHR_SPS) | (uartWordLen << LCHR_WLEN) | 
				  (stopBits << LCHR_STP2) | (oddEven << LCHR_EPS) | 
				  (parityUsed << LCHR_PEN);  
	
	// Enable the receive, transmitter and the whole UART module
	UART0->CTL |= (1 << CTL_RXE) | (1 << CTL_TXE) | (1 << CTL_UARTEN);

	// Initialise the UART mutex
	#ifdef USE_MUTEX	
		mutex_init(&uartMtx);
	#endif
}
				


/*-------------------------------------------------------------------------------
* Function:    	sendChar
* Purpose:    	Send character over UART
* Arguments:
*		character - character to send
* Returns: 		-	
--------------------------------------------------------------------------------*/
void uart_send_char(uint8_t character) {

	// Wait for the transmitter to be ready to accept next character
	while (UART0->FR & (1 << FR_TXFF)); 
	UART0->DR = character;
}




/*-------------------------------------------------------------------------------
* Function:    	getChar
* Purpose:    	Get character from UART
* Arguments: 	-
* Returns: 		
*		character received
--------------------------------------------------------------------------------*/
uint8_t uart_get_char(void) {
	
	// Wait for the next character to arrive at UART receiver and collect it
	while (UART0->FR & (1 << FR_RXFE)); 
	return (UART0->DR & 0xFF);
}




#endif
