/*******************************************************************************
* File:     	nokia5110.h
* Brief:    	Header file for nokia5110.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	15/02/2017
* Last mod: 	09/03/2017
*
* Note: 
*
*******************************************************************************/
#include "KrisOS.h"



/*-----------------------------------------------------------------------------
* SSI interface transmit frequency in MHz
------------------------------------------------------------------------------*/
#define SSI0_CLK_FREQ_MHZ 4



/*-----------------------------------------------------------------------------
* Screen pixel dimensions (frame buffer)
------------------------------------------------------------------------------*/
#define NOKIA5110_WIDTH 84
#define NOKIA5110_HEIGHT 48



/*-----------------------------------------------------------------------------
* File pointer to the LCD screen for redirecting output stream
------------------------------------------------------------------------------*/
extern __FILE nokia5110;
	


/*-------------------------------------------------------------------------------
* Nokia 5110 LCD screen mutual exclusion lock
--------------------------------------------------------------------------------*/
extern Mutex* nokiaMtx;



/*-----------------------------------------------------------------------------
* Types of data transfer to the LCD module
------------------------------------------------------------------------------*/
typedef enum {
	DATA,
	COMMAND,
} TransferType;



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_init
* Purpose:    	Initialise the SSI interface for communication with the 
*				controller on the Nokia 5110 LCD screen.
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_init(void);



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_send
* Purpose:    	Send a message to the nokia 5110 LCD controller 
* Arguments:	
*		type - message type (DATA/COMMAND)
*		data - message content
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_send(TransferType type, uint8_t data);



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_set_cursor
* Purpose:    	Move the cursor to position specified
* Arguments:	
*		x - new X cursor position (1 of the 6 'byte rows')
* 		y - new Y cursor position (1 of the 84 possible Y pixel positions)
* Returns: 
*		exit_status
--------------------------------------------------------------------------------*/
uint32_t nokia5110_set_cursor(uint8_t x, uint8_t y);



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_clear
* Purpose:    	Clear the screen
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_clear(void);



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_send_char
* Purpose:    	Display the character given on screen at the current cursor 
*				position.
* Arguments:	
*		toSend - ASCII character to send
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_send_char(uint8_t toSend);
