/*******************************************************************************
* File:     	common.h
* Brief:    	Common includes 
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		
*******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define test_null_pointer(ptr) 					\
	if (ptr == NULL) {							\
		printf("NULL pointer! ");		\
		exit(1); 								\
	}				
		
	
/*-------------------------------------------------------------------------------
* Floating point data type defnitions
*------------------------------------------------------------------------------*/
typedef float float32_t;		// 32-bit floating-point type definition
typedef double float64_t;		// 64-bit floating-point type definition
