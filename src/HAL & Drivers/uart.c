/*******************************************************************************
* File:     	uart.c
* Brief:    	A simple UART driver
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	11/03/2016
*
* Note: 		
*	The UART module driver is the only device driver that is part of the operating
*	system, however it is optional. In embedded operating systems the device drivers
*	should be written by the user as these tend to be quite specific and packing all
*	possible drivers inside the OS doesn't make sense in embedded (memory restricted)
*	applications.
*	
*	The driver uses GPIO pins PA0 and PA1 which are directly connected to the USB.
*	This is the main motivation for having the driver on-board. This allows for
*	useful debug data to be displayed on a serial monitor on a host machine 
*	without relying on the user to write a driver for at least one output stream.
*
*	The driver is polling-based and allows modification of the transmission baud
*	rate, provided it is sensible.
*******************************************************************************/
#include "KrisOS.h"
#include "system.h"
#include "kernel.h"
#ifdef USE_UART



/*-------------------------------------------------------------------------------
* Declaration of UART interface as a file for output stream redirection
--------------------------------------------------------------------------------*/
__FILE uart;



/*-------------------------------------------------------------------------------
* Mutex on UART. This mutex is optional but is maintained by the kernel code
* as the UART interface forms part of the KrisOS (is used for KrisOS debugging)
--------------------------------------------------------------------------------*/
#ifdef USE_MUTEX
	Mutex uartMtx;
#endif
	


/*-------------------------------------------------------------------------------
* Function:    	uart_init
* Purpose:    	Initialisation function for the UART0 interface (Serial monitor 
*				over USB).
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void uart_init(void) {
				   					
	// Integer and fractional part of the system clock divider for generating
	// the desired baudRate
	int32_t dividerInt; 				
	float32_t dividerFrac;	

	// Test if the baud rate is within the allowable range 9600 to 115200
	if (UART_BAUD_RATE < 9600 || UART_BAUD_RATE > 115200)
		exit(EXIT_UART_INVALID_BAUD_RATE);
	
	// Activate UART0 on port A and disable UART device for time of configuration
	SYSCTL->RCGCUART |= (1 << RCGC_UART0);		
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTA); 
	UART0->CTL &= ~(1 << CTL_UARTEN); 							   
				   
	// Calculate the divider, separate the integer and fractional parts and write
	// these to the Integer Baud-Rate Divisor and Fractional Baud-Rate Divisor 
	// registers
	dividerFrac = (float32_t) SYSTEM_CLOCK_FREQ / 16 / UART_BAUD_RATE;
	dividerInt = (int32_t) dividerFrac;
	dividerFrac -= dividerInt;
	dividerFrac = dividerFrac * 64 + 0.5f;
	UART0->IBRD = dividerInt; 							
	UART0->FBRD = (int32_t) dividerFrac; 
	
	// Enable port A pins 1 and 0. Set GPIOA pins 1-0 to alternative function (UART0)
	// and enable digitial I/O on pins 1-0 of GPIOA	
	GPIOA->PCTL &= ~((0xFU << PCTL_PMC1) | (0xFU << PCTL_PMC0));
	GPIOA->PCTL |= (1 << PCTL_PMC1) | (1 << PCTL_PMC0); 
	GPIOA->AFSEL |= (1 << PIN0) | (1 << PIN1);	 
	GPIOA->DEN |= (1 << PIN0) | (1 << PIN1);				   
				   
	// Set the parameters of serial communication. No parity checks, single stop bit,
	// UART FIFO disabled (polling mode), 8 bit word length
	UART0->LCRH = 0x3 << LCHR_WLEN;
	
	// Enable the UART receiver, the transmitter and the whole UART module
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

	// Wait for the transmitter to be ready (not full) to accept next character.
	// Then write to the UART data register
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
