/*******************************************************************************
* File:     	heap_manager.c
* Brief:    	heap implementation for dynamic memory allocation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	28/09/2016
* Last mod: 	28/09/2016
*
* Note: 		
*******************************************************************************/


/*-------------------------------------------------------------------------------
* Included headers
*------------------------------------------------------------------------------*/
#include "cortex_m4.h"


/*-------------------------------------------------------------------------------
* Heap manager used:
*	1. Simplest, array-based no heap freeing
* 	....
*------------------------------------------------------------------------------*/
#define HEAP_MANAGER 1

/*-------------------------------------------------------------------------------
* Heap byte alignment
*------------------------------------------------------------------------------*/
#define HEAP_BYTE_ALIGNMENT 8

/*-------------------------------------------------------------------------------
* Heap size in bytes
*------------------------------------------------------------------------------*/
#define HEAP_SIZE 200



// Simplest heap implementation
#if HEAP_MANAGER == 1

static uint8_t heap[HEAP_SIZE];		// the heap - array of bytes
static size_t bytesUsed = 0;		// number of bytes already used 


/*-------------------------------------------------------------------------------
* Function:    	heap_init
* Purpose:    	Heap initialisation function (no init needed in this case)
* Arguments:	- 	
* Returns: 		-	
--------------------------------------------------------------------------------*/
void heap_init(void){}					
	
	
/*-------------------------------------------------------------------------------
* Function:    	heap_allocate
* Purpose:    	Dynamically allocate bytesToAlloc bytes on the heap
* Arguments:	
*		bytesToAlloc - number of bytes to allocate	
* Returns: 
* 		pointer to the memory block allocated or a NULL pointer if unsuccessful
--------------------------------------------------------------------------------*/
void* heap_allocate(size_t bytesToAlloc) {
	
	// pointer to the memory allocates
	void* pointer;
	
	// Ensure byte-alignment required
	size_t bytesToAlign = bytesToAlloc % HEAP_BYTE_ALIGNMENT;
	if (bytesToAlign)
		bytesToAlloc += HEAP_BYTE_ALIGNMENT - bytesToAlign;
	
	// Check if enough space available for the allocation
	if (bytesToAlloc > get_free_heap_size())
		return NULL;
	
	// Update the heap and return the pointer to newly allocated memory
	pointer = &heap[bytesUsed];
	bytesUsed += bytesToAlloc;
	return pointer;
}

/*-------------------------------------------------------------------------------
* Function:    	heap_free
* Purpose:    	Free the allocated block of memory
* Arguments:	
*		toFree - block of heap memory to free
* Returns: 		-
--------------------------------------------------------------------------------*/
void heap_free(void* toFree) {}
	

/*-------------------------------------------------------------------------------
* Function:    	get_free_heap_size
* Purpose:    	Return the number of bytes still available for allocation
* Arguments:	-
* Returns: 
* 		number of bytes still possible for allocation
--------------------------------------------------------------------------------*/
size_t get_free_heap_size(void) {
	return HEAP_SIZE - bytesUsed;
}


#endif
