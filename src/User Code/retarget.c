/*******************************************************************************
* File:     	retarget.c
* Brief:    	File for redirection of output and input streams. Can be 
*				modified by the user.
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	06/10/2016
* Last mod: 	09/03/2017
*
* Note: 		
*******************************************************************************/
// Include the header files for input/output streams implemented
#include "uart.h"
#include "nokia5110.h"



/*-------------------------------------------------------------------------------
* Function:    	fputc
* Purpose:    	Writes a character to the specified output stream 
* Arguments:
*		character - character to write
*		file - pointer to the output stream
* Returns: 		
*		0 upon sucessful completion
--------------------------------------------------------------------------------*/
int fputc(int character, FILE *file) {
	
	// Write to UART0 (serial monitor on the computer connected via USB)
	#ifdef USE_UART
		if (file == &uart)
			uart_send_char(character); 
	#endif	

	// ---- ADD USER-DEFINED OUTPUT STREAMS BELOW: ---
	// Write to the nokia LCD screen
	if (file == &nokia5110)
		nokia5110_send_char(character);
	
	return 0;
}



/*-------------------------------------------------------------------------------
* Function:    	fgetc
* Purpose:    	Read a character from the specified input stream 
* Arguments:
*		file - pointer to the input stream
* Returns: 		
*		character read
--------------------------------------------------------------------------------*/
int fgetc(FILE *file) {
	
	// Read the character sent over UART0
	#ifdef USE_UART
		if (file == &uart)
			return uart_get_char();
	#endif

	// ---- ADD USER-DEFINED INPUT STREAMS BELOW: ---		
		
	return 0;
}

