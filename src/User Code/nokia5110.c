/*******************************************************************************
* File:     	nokia5110.c
* Brief:    	Nokia 5110 LCD screen driver
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	11/02/2017
* Last mod: 	11/02/2017
*
* Note: 
*******************************************************************************/
#include "KrisOS.h"
#include "nokia5110.h"



/*-----------------------------------------------------------------------------
* 5x7 pixel ASCII font
------------------------------------------------------------------------------*/
const uint8_t font_5x7[] = {
	
    0x00, 0x00, 0x00, 0x00, 0x00,	// (space)
	0x00, 0x00, 0x5F, 0x00, 0x00,	// !
	0x00, 0x07, 0x00, 0x07, 0x00,	// "
	0x14, 0x7F, 0x14, 0x7F, 0x14,	// #
	0x24, 0x2A, 0x7F, 0x2A, 0x12,	// $
	0x23, 0x13, 0x08, 0x64, 0x62,	// %
	0x36, 0x49, 0x55, 0x22, 0x50,	// &
	0x00, 0x05, 0x03, 0x00, 0x00,	// '
	0x00, 0x1C, 0x22, 0x41, 0x00,	// (
	0x00, 0x41, 0x22, 0x1C, 0x00,	// )
	0x08, 0x2A, 0x1C, 0x2A, 0x08,	// *
	0x08, 0x08, 0x3E, 0x08, 0x08,	// +
	0x00, 0x50, 0x30, 0x00, 0x00,	// ,
	0x08, 0x08, 0x08, 0x08, 0x08,	// -
	0x00, 0x60, 0x60, 0x00, 0x00,	// .
	0x20, 0x10, 0x08, 0x04, 0x02,	// /
	0x3E, 0x51, 0x49, 0x45, 0x3E,	// 0
	0x00, 0x42, 0x7F, 0x40, 0x00,	// 1
	0x42, 0x61, 0x51, 0x49, 0x46,	// 2
	0x21, 0x41, 0x45, 0x4B, 0x31,	// 3
	0x18, 0x14, 0x12, 0x7F, 0x10,	// 4
	0x27, 0x45, 0x45, 0x45, 0x39,	// 5
	0x3C, 0x4A, 0x49, 0x49, 0x30,	// 6
	0x01, 0x71, 0x09, 0x05, 0x03,	// 7
	0x36, 0x49, 0x49, 0x49, 0x36,	// 8
	0x06, 0x49, 0x49, 0x29, 0x1E,	// 9
	0x00, 0x36, 0x36, 0x00, 0x00,	// :
	0x00, 0x56, 0x36, 0x00, 0x00,	// ;
	0x00, 0x08, 0x14, 0x22, 0x41,	// <
	0x14, 0x14, 0x14, 0x14, 0x14,	// =
	0x41, 0x22, 0x14, 0x08, 0x00,	// >
	0x02, 0x01, 0x51, 0x09, 0x06,	// ?
	0x32, 0x49, 0x79, 0x41, 0x3E,	// @
	0x7E, 0x11, 0x11, 0x11, 0x7E,	// A
	0x7F, 0x49, 0x49, 0x49, 0x36,	// B
	0x3E, 0x41, 0x41, 0x41, 0x22,	// C
	0x7F, 0x41, 0x41, 0x22, 0x1C,	// D
	0x7F, 0x49, 0x49, 0x49, 0x41,	// E
	0x7F, 0x09, 0x09, 0x01, 0x01,	// F
	0x3E, 0x41, 0x41, 0x51, 0x32,	// G
	0x7F, 0x08, 0x08, 0x08, 0x7F,	// H
	0x00, 0x41, 0x7F, 0x41, 0x00,	// I
	0x20, 0x40, 0x41, 0x3F, 0x01,	// J
	0x7F, 0x08, 0x14, 0x22, 0x41,	// K
	0x7F, 0x40, 0x40, 0x40, 0x40,	// L
	0x7F, 0x02, 0x04, 0x02, 0x7F,	// M
	0x7F, 0x04, 0x08, 0x10, 0x7F,	// N
	0x3E, 0x41, 0x41, 0x41, 0x3E,	// O
	0x7F, 0x09, 0x09, 0x09, 0x06,	// P
	0x3E, 0x41, 0x51, 0x21, 0x5E,	// Q
	0x7F, 0x09, 0x19, 0x29, 0x46,	// R
	0x46, 0x49, 0x49, 0x49, 0x31,	// S
	0x01, 0x01, 0x7F, 0x01, 0x01,	// T
	0x3F, 0x40, 0x40, 0x40, 0x3F,	// U
	0x1F, 0x20, 0x40, 0x20, 0x1F,	// V
	0x7F, 0x20, 0x18, 0x20, 0x7F,	// W
	0x63, 0x14, 0x08, 0x14, 0x63,	// X
	0x03, 0x04, 0x78, 0x04, 0x03,	// Y
	0x61, 0x51, 0x49, 0x45, 0x43,	// Z
	0x00, 0x00, 0x7F, 0x41, 0x41,	// [
	0x02, 0x04, 0x08, 0x10, 0x20,	// "\"
	0x41, 0x41, 0x7F, 0x00, 0x00,	// ]
	0x04, 0x02, 0x01, 0x02, 0x04,	// ^
	0x40, 0x40, 0x40, 0x40, 0x40,	// _
	0x00, 0x01, 0x02, 0x04, 0x00,	// `
	0x20, 0x54, 0x54, 0x54, 0x78,	// a
	0x7F, 0x48, 0x44, 0x44, 0x38,	// b
	0x38, 0x44, 0x44, 0x44, 0x20,	// c
	0x38, 0x44, 0x44, 0x48, 0x7F,	// d
	0x38, 0x54, 0x54, 0x54, 0x18,	// e
	0x08, 0x7E, 0x09, 0x01, 0x02,	// f
	0x08, 0x14, 0x54, 0x54, 0x3C,	// g
	0x7F, 0x08, 0x04, 0x04, 0x78,	// h
	0x00, 0x44, 0x7D, 0x40, 0x00,	// i
	0x20, 0x40, 0x44, 0x3D, 0x00,	// j
	0x00, 0x7F, 0x10, 0x28, 0x44,	// k
	0x00, 0x41, 0x7F, 0x40, 0x00,	// l
	0x7C, 0x04, 0x18, 0x04, 0x78,	// m
	0x7C, 0x08, 0x04, 0x04, 0x78,	// n
	0x38, 0x44, 0x44, 0x44, 0x38,	// o
	0x7C, 0x14, 0x14, 0x14, 0x08,	// p
	0x08, 0x14, 0x14, 0x18, 0x7C,	// q
	0x7C, 0x08, 0x04, 0x04, 0x08,	// r
	0x48, 0x54, 0x54, 0x54, 0x20,	// s
	0x04, 0x3F, 0x44, 0x40, 0x20,	// t
	0x3C, 0x40, 0x40, 0x20, 0x7C,	// u
	0x1C, 0x20, 0x40, 0x20, 0x1C,	// v
	0x3C, 0x40, 0x30, 0x40, 0x3C,	// w
	0x44, 0x28, 0x10, 0x28, 0x44,	// x
	0x0C, 0x50, 0x50, 0x50, 0x3C,	// y
	0x44, 0x64, 0x54, 0x4C, 0x44,	// z
	0x00, 0x08, 0x36, 0x41, 0x00,	// {
	0x00, 0x00, 0x7F, 0x00, 0x00,	// |
	0x00, 0x41, 0x36, 0x08, 0x00,	// }
	0x08, 0x08, 0x2A, 0x1C, 0x08,	// ->
	0x08, 0x1C, 0x2A, 0x08, 0x08 	// <-
};



/*-----------------------------------------------------------------------------
* DC pin address and possible values
------------------------------------------------------------------------------*/
#define DC (*((volatile uint32_t*) 0x40004100))
#define DC_DATA 0x40
#define DC_COMMAND 0x0



/*-----------------------------------------------------------------------------
* RST pin address
------------------------------------------------------------------------------*/
#define RST (*((volatile uint32_t*) 0x40004200))
#define RST_HIGH 0x80
#define RST_LOW 0x0



/*-------------------------------------------------------------------------------
* Nokia 5110 LCD screen mutual exclusion lock
--------------------------------------------------------------------------------*/
Mutex* nokiaMtx;



/*-----------------------------------------------------------------------------
* Current cursor Y coordinate
------------------------------------------------------------------------------*/
uint8_t cursorRow;



/*-----------------------------------------------------------------------------
* File pointer for redirecting output stream
------------------------------------------------------------------------------*/
__FILE nokia5110;



/*******************************************************************************
* Task: 	nokiaLCDSetup
* Purpose: 	One-off LCD screen setup task run first and then removed from memory
*			(allocated on heap)
*******************************************************************************/
void nokiaLCDSetup(void) {
	
	int16_t a, b;
	Queue* testQ = KrisOS_queue_create(5, sizeof(int16_t));
	
	for (a = 0; a < 6; a++)
		KrisOS_queue_try_write(testQ, &a);
	
	for (a = 0; a < 6; a++)
		KrisOS_queue_try_read(testQ, &b);
	
	a = 231;
	KrisOS_queue_try_write(testQ, &a);	
	KrisOS_queue_try_read(testQ, &a);	

	// Initialise the screen together with a mutex on it
	nokiaMtx = (Mutex*) KrisOS_mutex_create();
	nokia5110_init();
	
	// 'Say my name baby'
	nokia5110_set_cursor(25, 5);
	fprintf(&nokia5110, "KrisOS"); 	
}



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_init
* Purpose:    	Initialise the SSI interface for communication with the 
*				controller on the Nokia 5110 LCD screen.
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_init(void) {
	
	// Variable for adding a short delay during module reset
	uint32_t delay;
	
	// Activate the SSI0 module and enable clock on port A. Add a small delay.
	SYSCTL->RCGCSSI |= 1 << RCGC_SSI0;
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTA); 
	while ((SYSCTL->RCGCGPIO & (1 << RCGCGPIO_PORTA)) == 0);
	
	// Set GPIO PA6 and PA7 to output and enable digital I/O on PA2-PA7
	// (excluding PA4)
	GPIOA->DIR |= (1 << PIN6) |  (1 << PIN7);
	GPIOA->DEN |= (1 << PIN2) | (1 << PIN3) | (1 << PIN5) | 
				  (1 << PIN6) | (1 << PIN7);
	
	// Set the pins PA2, PA3 and PA5 alternative function - SSI and PA6, PA7 to 
	// GPIO
	GPIOA->AFSEL |= (1 << PIN2) | (1 << PIN3) | (1 << PIN5);
	GPIOA->AFSEL &= ~((1 << PIN6) | (1 << PIN7));
	GPIOA->PCTL &= ~((0xFU << PCTL_PMC6) | (0xFU << PCTL_PMC7) |
				     (0xFU << PCTL_PMC2) | (0xFU << PCTL_PMC3) | 
				     (0xFU << PCTL_PMC5)); 
	GPIOA->PCTL |= (2 << PCTL_PMC2) | (2 << PCTL_PMC3) | 
				   (2 << PCTL_PMC5); 
	
	// Disable SSI0 for setup, and set it to master mode
	SSI0->CR1 &= ~(1 << CR1_SSE);
	SSI0->CR1 &= ~(1 << CR1_MS);
	
	// SSI0 clock is derived from system clock:			
	// SSInCLK = SysClk / ( CPSDVSR * (1 + SCR))
	SSI0->CC &= ~0xF;
	SSI0->CPSR = SYSTEM_CLOCK_FREQ / 1000000 / SSI0_CLK_FREQ_MHZ;
	SSI0->CR0 &= ~(0xFF << CR0_SCR);
	
	// Freescale format, 8-bit data. Also set clock phase and polarity
	SSI0->CR0 &= ~((1 << CR0_SPH) | (1 << CR0_SPO) | (0x3 << CR0_FRF));
	SSI0->CR0 &= ~(0xF << CR0_DSS);
	SSI0->CR0 |= 0x7 << CR0_DSS;
	
	// Re-enable SSI0
	SSI0->CR1 |= 1 << CR1_SSE;
	
	// Reset the LCD screen controller
	RST = RST_LOW;
	for (delay = 0; delay < 10; delay++);
	RST = RST_HIGH;
	
	// Power up, set horizontal addressing and extended instruction set (for setup)
	nokia5110_send(COMMAND, 0x21);
	
	// Set the operating voltage, the temperature coefficient and LCD bias voltage
	// level
	nokia5110_send(COMMAND, 0xB1);
	nokia5110_send(COMMAND, 0x04);
	nokia5110_send(COMMAND, 0x14);
	
	// Come back to basic instruction set, and set the normal display mode
	nokia5110_send(COMMAND, 0x20);
	nokia5110_send(COMMAND, 0x0C);
	
	// Clear the screen
	nokia5110_clear();
}



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_send
* Purpose:    	Send a message to the nokia 5110 LCD controller 
* Arguments:	
*		type - message type (DATA/COMMAND)
*		data - message content
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_send(TransferType type, uint8_t data) {
	
	// Set the DC pin according to whether a command or data is being sent
	DC = type == DATA ? DC_DATA : DC_COMMAND;
	
	// Wait until the TX FIFO is not full send the data
	while ((SSI0->SR & (1 << SR_TFE)) == 0);
	SSI0->DR = data;
	
	// Wait for acknowledge and remove it from the RX FIFO
	while ((SSI0->SR & (1 << SR_RNE)) == 0);
	data = SSI0->DR;
}



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_set_cursor
* Purpose:    	Move the cursor to the new specified position
* Arguments:	
*		x - new X cursor position
* 		y - new Y cursor position
* Returns: 
*		exit_status
--------------------------------------------------------------------------------*/
uint32_t nokia5110_set_cursor(uint8_t x, uint8_t y) {
	
	// Check if arguments are in allowed pixel address range
	if (x >= NOKIA5110_WIDTH || y >= NOKIA5110_HEIGHT)
		return EXIT_FAILURE;

	// Set the new X and Y coordinate
	nokia5110_send(COMMAND, 0x80 | x);
	nokia5110_send(COMMAND, 0x40 | y);
	cursorRow = y;
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_clear
* Purpose:    	Clear the screen 
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_clear(void) {
	
	uint32_t i;
	for (i = 0; i < NOKIA5110_HEIGHT * NOKIA5110_WIDTH; i++)
		nokia5110_send(DATA, 0x00);
	
	nokia5110_set_cursor(0, 0);
}



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_send_char
* Purpose:    	Display the character given on screen at the current cursor 
*				position.
* Arguments:	
*		toSend - ASCII character to send
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_send_char(uint8_t toSend) {
	
	// Helper variable for displaying character as a set of 5 byte columns of pixels 
	uint32_t column;
	
	// Character to send is one of the newline symbols. Move the cursor to the
	// beginning of the new line
	if (toSend == 0xA || toSend == 0xC || toSend == 0x1B)
		nokia5110_set_cursor(0, (cursorRow + 1) % NOKIA5110_HEIGHT);
	
	// Character to send is one of the printable ASCII symbols. Output the 
	// character as 5 vertical bytes of data followed by a delimiter
	// for spacing out the characters
	else if (toSend >= 0x20 && toSend <= 0x7F) {
		for (column = 0; column < 5; column++)
			nokia5110_send(DATA, font_5x7[(toSend - 0x20) * 5 + column]);	
		
		nokia5110_send(DATA, 0x00);
	}
}
