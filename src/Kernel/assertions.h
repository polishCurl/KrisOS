/*******************************************************************************
* File:     	assertions.h
* Brief:    	Commonly used assertions which are defined as multiline macros
*				to avoid method calls 
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	02/03/2016
* Last mod: 	11/03/2016
*
* Note: 		
*	The following multiline macros are used for input argument checking, especially
*	inside the OS calls routines to prevent the user to cause erroneous OS
*	behaviour by speciffying irrational input arguments. This header file contains
*	MACROS which aren't very context specific, these assertions are used throughout
*	multiple files. The types of common incorrect input arguments a used can specify
*	inside an SVC call:
*		1. NULL pointer as a reference to a data structure
*		2. Size argument which is not positive
*
*	There are a lot more traps for invalid arguments, however they are context
*	specific so the code for dealing with them is declared directly inside 
*	these methods. 
*
*	This file also declares a set of EXIT codes for terminating the OS safely
*	should an exception situation occur (no more heap space left, UART baud rate
*	outside allowed range etc)
*******************************************************************************/
#define EXIT_NULL 1
#define EXIT_INVALID_SIZE 2
#define EXIT_INVALID_IRQ_PRIO 3
#define EXIT_HEAP_TOO_SMALL 4
#define EXIT_UART_INVALID_BAUD_RATE 5
#define EXIT_INVALID_OS_CLOCK_FREQ 6



/*-------------------------------------------------------------------------------
* Macro:    	TEST_NULL_POINTER
* Purpose:    	Test if given pointer is a NULL one.
* Arguments:	
*		PTR - pointer to test
* Returns: 		-	
--------------------------------------------------------------------------------*/
#define TEST_NULL_POINTER(PTR) 					\
	if (PTR == NULL) 							\
		exit(EXIT_NULL); 								
	
	
	
/*-------------------------------------------------------------------------------
* Macro:    	TEST_INVALID_SIZE
* Purpose:    	Test if given number is equal to 0. If so, this means that 
*				the size argument is invalid.
* Arguments:	
*		SIZE - size to check
* Returns: 		-	
--------------------------------------------------------------------------------*/
#define TEST_INVALID_SIZE(SIZE) 				\
	if (SIZE == 0) 								\
		exit(EXIT_INVALID_SIZE); 									
