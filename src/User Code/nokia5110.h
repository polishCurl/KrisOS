/*******************************************************************************
* File:     	nokia5110.h
* Brief:    	Header file for nokia5110.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	15/02/2017
* Last mod: 	21/02/2017
*
* Note: 
* 	Tiva C Pin pins used for connecting to the nokia 5110 LCD screen. The maximum 
* 	allowed baud rate is 4Mb/s (4Mhz) and in this driver software the SSI frequency 
* 	is derived from the system clock. Use of Tiva C GPIO pins is the following
* 		PA2 - CLK
* 		PA3 - CE
* 		PA5 - DIN
* 		PA6 - DC (Data/Command)
* 		PA7 - RST
*
* 	The cursor is set to move horizontally. The display consists of 6 rows and 
* 	84 columns. Each write affects the 8 pixels in a vertical line at the current
* 	cursor position. The LCD screen uses PCD8544 controller from PHILLIPS
*******************************************************************************/
#include "KrisOS.h"



/*-----------------------------------------------------------------------------
* SSI transmit frequency
------------------------------------------------------------------------------*/
#define SSI0_CLK_FREQ_MHZ 4



/*-----------------------------------------------------------------------------
* File pointer for redirecting output stream
------------------------------------------------------------------------------*/
extern __FILE nokia5110;
	


/*-----------------------------------------------------------------------------
* Type of data transfer to the LCD module
------------------------------------------------------------------------------*/
typedef enum {
	DATA,
	COMMAND,
} TransferType;



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_init
* Purpose:    	Initialisation of the SSI interface for communication with the 
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
* Purpose:    	Set cursor to the specified position on screen
* Arguments:	
*		x - new X cursor position
* 		y - new Y cursor position
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


