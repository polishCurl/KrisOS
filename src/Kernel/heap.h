/*******************************************************************************
* File:     	heap_manager.h
* Brief:    	Header file for heap_manager.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	28/09/2016
* Last mod: 	28/09/2016
*
* Note: 		malloc and free method headers are already declared in stdlib.h
*******************************************************************************/
#include "mutex.h"
#ifndef HEAP_H
#define HEAP_H
#ifdef USE_HEAP



/*-------------------------------------------------------------------------------
* Heap byte alignment
*------------------------------------------------------------------------------*/
#define HEAP_BYTE_ALIGN 8 			



/*-------------------------------------------------------------------------------
* Heap size in bytes (including the currently set word alignment)
*------------------------------------------------------------------------------*/		
#define ALIGNED_HEAP_SIZE (HEAP_SIZE % HEAP_BYTE_ALIGN ? 									\
							(HEAP_SIZE + (HEAP_BYTE_ALIGN - HEAP_SIZE % HEAP_BYTE_ALIGN)) :	\
							HEAP_SIZE)
				
				

/*-------------------------------------------------------------------------------
* Heap free block definition
--------------------------------------------------------------------------------*/
typedef struct HeapBlock {
		size_t blockSize; 				// size in bytes
		struct HeapBlock* next; 		// pointer to the next free heap block
} HeapBlock;



/*-------------------------------------------------------------------------------
* Heap manager definition
--------------------------------------------------------------------------------*/
typedef struct HeapManager {
	HeapBlock startBlock; 				// Beginning and the end of list of free blocks
	HeapBlock endBlock;
	uint8_t heapMem[ALIGNED_HEAP_SIZE];	// Heap memory
	uint32_t heapBytesUsed;				// Number of bytes already allocated on heap
#ifdef USE_MUTEX
	Mutex heapMutex;					// Mutual exclusion lock on the heap
#endif						
	
} HeapManager;

extern HeapManager heap;



/*-------------------------------------------------------------------------------
* Function:    	heap_init
* Purpose:    	Heap initialisation function (no init needed in this case)
* Arguments:	- 	
* Returns: 		-	
--------------------------------------------------------------------------------*/
void heap_init(void);



/*-------------------------------------------------------------------------------
* Function:    	malloc
* Purpose:    	Dynamically allocate bytesToAlloc bytes of memory
* Arguments:	
*		bytesToAlloc - number of bytes to allocate on heap
* Returns: 
* 		pointer to the memory block allocated or a NULL pointer if unsuccessful
--------------------------------------------------------------------------------*/
void* malloc(size_t bytesToAlloc);



/*-------------------------------------------------------------------------------
* Function:    	free
* Purpose:    	Free the allocated block of memory
* Arguments:	
*		toFree - block of heap memory to free
* Returns: 		-
--------------------------------------------------------------------------------*/
void free(void* toFree);



/*-------------------------------------------------------------------------------
* Function:    	align_byte_number
* Purpose:    	Update the number of bytes requested for allocation so that it is 
*				compliant with the current byte alignment
* Arguments:	
* 		byteNumber - number of bytes requested
* Returns: 		
*		modified byte number to
--------------------------------------------------------------------------------*/
size_t align_byte_number(size_t byteNumber);



/*-------------------------------------------------------------------------------
* Function:    	heap_insert_free_block
* Purpose:    	Insert a new block into the list of free blocks in ascending size
* 				order
* Arguments:	
*		toInsert - pointer to the block to insert
* Returns: 		-
--------------------------------------------------------------------------------*/
void heap_insert_free_block(HeapBlock* toInsert);

#endif
#endif
