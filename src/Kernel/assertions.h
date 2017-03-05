/*******************************************************************************
* File:     	assertions.h
* Brief:    	Commonly used assertions which are defined as multiline macros
*				to avoid method calls (slower)
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	02/03/2016
* Last mod: 	02/03/2016
*
* Note: 		
*******************************************************************************/
#define EXIT_NULL 1
#define EXIT_INVALID_SIZE 2
#define EXIT_INVALID_IRQ_PRIO 3
#define EXIT_HEAP_TOO_SMALL 4



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
