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
#ifdef USE_UART



/*-------------------------------------------------------------------------------
* Uart I/O stream 'as file' declarations 
--------------------------------------------------------------------------------*/
__FILE UART_FILE;
__FILE *uart;



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
	
	SYSCTL->RCGCUART |= (1 << RCGCUART_UART0_Pos);		// Activate UART0
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTA_Pos); 		// UART0 on port A
	GPIOA->AFSEL |= (1 << PIN0_Pos) | (1 << PIN1_Pos);	// Set GPIOA pins 1-0 to alternative function
	GPIOA->DEN |= (1 << PIN0_Pos) | (1 << PIN1_Pos);	// Enable digitial I/O on pins 1-0 of GPIOA
					
	// Enable port A pins 1 and 0
	GPIOA->PCTL |= (1 << PCTL_PMC1_Pos) | (1 << PCTL_PMC0_Pos); 
					
	UART0->CTL &= ~(1 << CTL_UARTEN_Pos); 				// Disable UART device for time of configuration
	UART0->IBRD = dividerInt; 							// divider, integer part
	UART0->FBRD = (int32_t) dividerFrac; 				// divider, fractional part 	
	
	// Set the parameters of serial communication
	UART0->LCRH = (parityUsed << LCHR_SPS_Pos) | (uartWordLen << LCHR_WLEN_Pos) | 
				  (stopBits << LCHR_STP2_Pos) | (oddEven << LCHR_EPS_Pos) | 
				   (parityUsed << LCHR_PEN_Pos);  
				   
	// Enable the receive, transmitter and the whole UART module
	UART0->CTL |= (1 << CTL_RXE_Pos) | (1 << CTL_TXE_Pos) | (1 << CTL_UARTEN_Pos);	

	// Create a file pointer for redirecting I/O stream
	uart = &UART_FILE;
}
				


/*-------------------------------------------------------------------------------
* Function:    	sendChar
* Purpose:    	Send character over UART
* Arguments:
*		character - character to send
* Returns: 		-	
--------------------------------------------------------------------------------*/
void uart_send_char(uint8_t character) {
	
	// Wait for TX buffer to be not full and then insert the value to FIFO
	while (UART0->FR & (1 << TXFF_Pos)); 	
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
	
	// Wait for RX buffer to be not empty and return the value from FIFO
	while (UART0->FR & (1 << RXFE_Pos)); 	
	return (uint8_t) UART0->DR & 0xFF;
}


#endif
