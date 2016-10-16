/*******************************************************************************
* File:     	KrisOS.h
* Brief:    	Interface to the embedded operating system
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		Method declarations visible to OS users for accessing its 
*				functionality
*******************************************************************************/
#include "svc.h"
#include "fifo.h"


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_init
* Purpose:    	Initialise the operating system
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void __svc(SVC_OS_START) KrisOS_init(void);


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_malloc
* Purpose:    	Request dynamic memory allocation
* Arguments:	
* 		bytesToAlloc - number of bytes to allocate
* Returns: 		
*		pointer to the allocated memory or NULL if unsuccessful
--------------------------------------------------------------------------------*/
void* __svc(SVC_HEAP_ALLOC) KrisOS_malloc(size_t bytesToAlloc);


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_free
* Purpose:    	Free the previously allocated memory
* Arguments:	
*		toFree - pointer to the block of memory to free
* Returns: 		-
--------------------------------------------------------------------------------*/
void __svc(SVC_HEAP_FREE) KrisOS_free(void* toFree);

