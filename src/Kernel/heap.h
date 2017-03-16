/*******************************************************************************
* File:     	heap_manager.h
* Brief:    	Header file for heap_manager.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	28/09/2016
* Last mod: 	14/03/2017
*
* Note: 
*******************************************************************************/
#include "mutex.h"
#ifndef HEAP_H				// include barrier
#define HEAP_H
#ifdef USE_HEAP



/*-------------------------------------------------------------------------------
* Heap memory byte alignment
*------------------------------------------------------------------------------*/
#define HEAP_BYTE_ALIGN 8 			



/*-------------------------------------------------------------------------------
* Heap size in bytes (takes into account the HEAP_BYTE_ALIGN)
*------------------------------------------------------------------------------*/		
#define ALIGNED_HEAP_SIZE (HEAP_SIZE % HEAP_BYTE_ALIGN ? 						\
	(HEAP_SIZE + (HEAP_BYTE_ALIGN - HEAP_SIZE % HEAP_BYTE_ALIGN)) :	HEAP_SIZE)
				
				

/*-------------------------------------------------------------------------------
* Heap free block definition
--------------------------------------------------------------------------------*/
typedef struct HeapBlock {
		size_t blockSize; 				// Size of the free heap memory a block describes
		struct HeapBlock* next; 		// Pointer to the next free heap block
} HeapBlock;



/*-------------------------------------------------------------------------------
* Heap manager definition
--------------------------------------------------------------------------------*/
typedef struct HeapManager {
	HeapBlock startBlock; 				// The beginning and the end of list of free blocks
	HeapBlock* endBlock;
	uint32_t heapBytesUsed;				// Number of bytes already allocated on heap
#ifdef USE_MUTEX
	Mutex heapMutex;					// Mutual exclusion lock on the heap
#endif	
	uint8_t heapMem[ALIGNED_HEAP_SIZE];	// Statically alloated heap memory area
} HeapManager;

extern HeapManager heap;



/*-------------------------------------------------------------------------------
* Function:    	heap_init
* Purpose:    	Heap initialisation function
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
* 		pointer to the memory block allocated. Doesn't return if unsuccessful
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
* Function:    	heap_insert_free_block
* Purpose:    	Insert a new block into the list of free blocks in ascending block 
* 				size order
* Arguments:	
*		toInsert - pointer to the block to insert
* Returns: 		-
--------------------------------------------------------------------------------*/
void heap_insert_free_block(HeapBlock* toInsert);



/*-------------------------------------------------------------------------------
* Function:    	heap_align_byte_number
* Purpose:    	Update the number of bytes requested for allocation so that it is 
*				compliant with the heap byte alignment
* Arguments:	
* 		byteNumber - number of bytes requested
* Returns: 		
*		modified byte number to
--------------------------------------------------------------------------------*/
size_t heap_align_byte_number(size_t byteNumber);


#endif
#endif
