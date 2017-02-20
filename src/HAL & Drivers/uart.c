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
* UART Receiver and Transmitter software FIFOs
--------------------------------------------------------------------------------*/
#if UART_INTERFACE_TYPE == 1
add_fifo(uart_rx, uint8_t, UART_FIFO_SIZE)
add_fifo(uart_tx, uint8_t, UART_FIFO_SIZE)
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
	
	#if UART_INTERFACE_TYPE == 1
		UART0->LCRH |= (1 << LCHR_FEN);
	
		// Configure the RX and TX FIFO level interrupt. TX FIFO <= 1/8 full. RX FIFO >= 7/8 full.
		UART0->IFLS &= ~0x3F; 
		UART0->IFLS |= (4 << TXIFSEL) | (4 << RXIFSEL);

		// Enable the TX and RX FIFO interrupts and RX FIFO Time-Out interrupt
		UART0->IM |= (1 << TXIM) | (1 << RXIM) | (1 << RTIM);
		
		// Clear the already pending UART interrupts
		UART0->ICR = ~0x0;	
		
		// Enable UART0 interrupts at NVIC controller and set their priority
		nvic_set_priority(UART0_IRQn, 1);
		nvic_enable_irq(UART0_IRQn);
		
		// Initialise the software RX and TX FIFOs
		uart_rx_fifo_init();
		uart_tx_fifo_init();
	#endif	
	
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
	
	#if UART_INTERFACE_TYPE == 0
		// Wait for the transmitter to be ready to accept next character
		while (UART0->FR & (1 << FR_TXFF)); 
		UART0->DR = character;
	#else
		// Add the character given to the software TX FIFO and then attempt to push it 
		// further to the hardware one, disabling the interrupt in the meantime
		while(uart_tx_fifo_put(character) == EXIT_FAILURE);
		UART0->IM &= ~(1 << TXIM);
		uart_software_to_hardware_fifo();
		UART0->IM |= (1 << TXIM);
	#endif
}




/*-------------------------------------------------------------------------------
* Function:    	getChar
* Purpose:    	Get character from UART
* Arguments: 	-
* Returns: 		
*		character received
--------------------------------------------------------------------------------*/
uint8_t uart_get_char(void) {
	
	uint8_t character;
	#if UART_INTERFACE_TYPE == 0
		// Wait for the next character to arrive at UART receiver and collect it
		while (UART0->FR & (1 << FR_RXFE)); 
	#else
		// Get the next character from software RX FIFO
		while(uart_rx_fifo_get(&character) == EXIT_FAILURE);
	#endif
	
	return character;
}



#if UART_INTERFACE_TYPE == 1
/*-------------------------------------------------------------------------------
* Function:    	UART0_Handler
* Purpose:    	UART0 Interrupt handler
* Arguments: 	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void UART0_Handler(void) {
	
	// Find the interrupt source. UART TX FIFO nearly empty. Acknowledge the interrupt 
	// and send new characters from software FIFO to the hardware one. If the software
	// TX FIFO becomes empty then disable the TX FIFO Interrupts
	if (UART0->RIS & (1 << TXRIS)) { 
		UART0->ICR |= (1 << TXIC);
		uart_software_to_hardware_fifo();
		if (uart_rx_fifo_is_empty())
			UART0->IM &= ~(1 << TXIM);
	}
	
	// UART RX FIFO is almost full. Acknowledge the interrupt and send new characters
	// from hardware FIFO to the software one.
	if (UART0->RIS & (1 << RXRIS)) {
		UART0->ICR |= (1 << RXIC);
		uart_hardware_to_software_fifo();
	}
	
	// UART RX FIFO has timed out (some data is still waiting to be collected). 
	// Acknowledge the interrupt and send the remaining characters from hardware 
	// FIFO to the software one. 
	if (UART0->RIS & (1 << RTRIS)) {
		UART0->ICR |= (1 << RTIC);
		uart_hardware_to_software_fifo();
	}
}



/*-------------------------------------------------------------------------------
* Function:    	uart_hardware_to_software_fifo
* Purpose:    	Copy data from UART receiver hardware FIFO to the software one
* Arguments: 	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void uart_hardware_to_software_fifo(void) {
	uint8_t character;
	while ((UART0->FR & (1 << FR_RXFE)) == 0 && !uart_rx_fifo_is_full()) {
		character = UART0->DR;
		uart_rx_fifo_put(character);
	}
}



/*-------------------------------------------------------------------------------
* Function:    	uart_software_to_hardware_fifo
* Purpose:    	Copy data from transmitter software FIFO to the UART hardware one
* Arguments: 	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void uart_software_to_hardware_fifo(void) {
	uint8_t character;
	while ((UART0->FR & (1 << FR_TXFF)) == 0 && !uart_tx_fifo_is_empty()) {
		uart_tx_fifo_get(&character);
		UART0->DR = character;
	}
}


#endif
#endif
