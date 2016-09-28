/*******************************************************************************
* File:     	heap_manager.h
* Brief:    	Header file for heap_manager.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	28/09/2016
* Last mod: 	28/09/2016
*
* Note: 		
*******************************************************************************/


/*-------------------------------------------------------------------------------
* Function:    	heap_init
* Purpose:    	Heap initialisation function (no init needed in this case)
* Arguments:	- 	
* Returns: 		-	
--------------------------------------------------------------------------------*/
extern void heap_init(void);


/*-------------------------------------------------------------------------------
* Function:    	heap_allocate
* Purpose:    	Dynamically allocate bytesToAlloc bytes on the heap
* Arguments:	
*		bytesToAlloc - number of bytes to allocate	
* Returns: 
* 		pointer to the memory block allocated or a NULL pointer if unsuccessful
--------------------------------------------------------------------------------*/
extern void* heap_allocate(size_t bytesToAlloc);


/*-------------------------------------------------------------------------------
* Function:    	heap_free
* Purpose:    	Free the allocated block of memory
* Arguments:	
*		toFree - block of heap memory to free
* Returns: 		-
--------------------------------------------------------------------------------*/
extern void heap_free(void* toFree);


/*-------------------------------------------------------------------------------
* Function:    	get_free_heap_size
* Purpose:    	Return the number of bytes still available for allocation
* Arguments:	-
* Returns: 
* 		number of bytes still possible for allocation
--------------------------------------------------------------------------------*/
extern size_t get_free_heap_size(void);
