/*******************************************************************************
* File:     	retarget.c
* Brief:    	Redefinition of low-level I/O routines from stdio.h
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	06/10/2016
* Last mod: 	06/10/2016
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"
#include "uart.h"
#include "nokia5110.h"
#include <stdio.h>
#include <rt_misc.h>



/*-------------------------------------------------------------------------------
* Semihosting is not supported by MDK-ARM. Semihosting is a mechanism that enables 
* code running on an ARM target to communicate and use the Input/Output facilities 
* on a host computer that is running a debugger.
--------------------------------------------------------------------------------*/
#pragma import(__use_no_semihosting_swi)
		


/*-------------------------------------------------------------------------------
* Standard output and input streams
--------------------------------------------------------------------------------*/
FILE __stdout;
FILE __stdin;



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
	
	if (file == &nokia5110)
		nokia5110_send_char(character);
		
	#ifdef USE_UART
		if (file == &uart)
			uart_send_char(character); 
	#endif	
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
	
#ifdef USE_UART
	if (file == &uart)
		return uart_get_char();
#endif
	return 0;
}



/*-------------------------------------------------------------------------------
* Function:    	ferror
* Purpose:    	Test the error indicator for the given stream
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
int ferror(FILE *file) {
	return EOF;
}



/*-------------------------------------------------------------------------------
* Function:    	_ttywrch
* Purpose:    	Write a character to the console.
* Arguments:	
*		character - character to write
* Returns: 		-
--------------------------------------------------------------------------------*/
void _ttywrch(int character) {
#ifdef USE_UART
	uart_send_char(character);
#endif
}



/*-------------------------------------------------------------------------------
* Function:    	_sys_exit
* Purpose:    	Library exit function, must not return. 
* Arguments:	
*		return_code - advisory
* Returns: 		-
--------------------------------------------------------------------------------*/
void _sys_exit(int return_code) {
#ifdef USE_UART
	fprintf(&uart, "Terminating KrisOS...");
#endif
	while(1);
}
