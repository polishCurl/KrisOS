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
#include <stdio.h>
#include <rt_misc.h>
#include "svc.h"

/*-------------------------------------------------------------------------------
* Semihosting is not supported by MDK-ARM. Semihosting is a mechanism that enables 
* code running on an ARM target to communicate and use the Input/Output facilities 
* on a host computer that is running a debugger.
--------------------------------------------------------------------------------*/
#pragma import(__use_no_semihosting_swi)
		


/*-------------------------------------------------------------------------------
* File representation
--------------------------------------------------------------------------------*/
struct __FILE { 
	int handle;
};
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
	send_char(character);		// send the desired character over UART serial port
	return 0;
}


/*-------------------------------------------------------------------------------
* Function:    	fgetc
* Purpose:    	Read a character from the specified input stream 
* Arguments:
*		file - pointer to the input stream (unused) 
* Returns: 		
*		character read
--------------------------------------------------------------------------------*/
int fgetc(FILE *f) {
	return get_char();			// read the character from UART serial port
}


/*-------------------------------------------------------------------------------
* Function:    	ferror
* Purpose:    	Test the error indicator for the given stream
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
int ferror(FILE *f) {
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
	send_char(character);
}


/*-------------------------------------------------------------------------------
* Function:    	_sys_exit
* Purpose:    	Library exit function, must not return. 
* Arguments:	
*		return_code - advisory
* Returns: 		-
--------------------------------------------------------------------------------*/
void _sys_exit(int return_code) {
	printf("Exiting...");
	while(1);
}
