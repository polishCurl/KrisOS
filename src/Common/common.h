/*******************************************************************************
* File:     	common.h
* Brief:    	Common libraries to include and other definitions
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	04/12/2016
*
* Note: 		
*******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


/*-------------------------------------------------------------------------------
* Shift value for word access
*------------------------------------------------------------------------------*/
#define WORD_ACCESS_SHIFT 2



/*-------------------------------------------------------------------------------
* Macro for expressing that an item wasn't found
*------------------------------------------------------------------------------*/
#define NOT_FOUND -1



/*-------------------------------------------------------------------------------
* Macro:    	TEST_NULL_POINTER
* Purpose:    	Test if given pointer is a NULL one. if so, exit
* Arguments:	
*		ptr - pointer to test
* Returns: 		-	
--------------------------------------------------------------------------------*/
#define TEST_NULL_POINTER(PTR) 					\
	if (PTR == NULL) 							\
		exit(1); 								
	
	

/*-------------------------------------------------------------------------------
* Floating point data types defnition
*------------------------------------------------------------------------------*/
typedef float float32_t;		// 32-bit floating-point type definition
typedef double float64_t;		// 64-bit floating-point type definition
