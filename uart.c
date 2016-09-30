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
#include "system.h"
#include "tm4c123gh6pm.h"


/*-------------------------------------------------------------------------------
* Function:    	uart_init
* Purpose:    	Initialisation of UART interface.
* Arguments:
*		baudRate - controls the number of bits sent per second
* Returns: 		-	
--------------------------------------------------------------------------------*/
void uart_init(uint32_t baudRate) {
	
	
	float64_t divider;				// system clock divider
	int32_t dividerInt;				// integer part of clock divider
	
	// calculate the system clock divider for getting the correct baud rate
	divider =  ((float64_t) SYSTEM_CLOCK_FREQ) / 16.0 / baudRate;
	dividerInt = (int32_t) divider;
	
	SYSCTL_RCGUART |= 0x1;			// Activate UART0
	SYSCTL_RCGCGPIO |= 0x1; 		// UART0 on port A
	UART0->CTL &= ~(0x1); 			// Disable UART device for time of configuration
	UART0->IBRD = dividerInt; 		// divider, integer part
	
	
	// UART0_FBRD = (int32_t)round(divider - dividerInt);	// divider, fractional part
	
	
}
