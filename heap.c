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
#include "common.h"
#include "heap.h"


/*-------------------------------------------------------------------------------
* Heap manager used:
*	1. Simplest, array-based no heap freeing
* 	2. Linked list of free blocks, allows freeing but can lead to fragmentation
*------------------------------------------------------------------------------*/
#define HEAP_MANAGER 2


/*-------------------------------------------------------------------------------
* Heap byte alignment
*------------------------------------------------------------------------------*/
#define HEAP_BYTE_ALIGN 8 			


/*-------------------------------------------------------------------------------
* Heap size in bytes, both the desired and aligned one
*------------------------------------------------------------------------------*/
#define HEAP_SIZE 192				
#define ALIGNED_HEAP_SIZE (HEAP_SIZE % HEAP_BYTE_ALIGN ? 									\
							(HEAP_SIZE + (HEAP_BYTE_ALIGN - HEAP_SIZE % HEAP_BYTE_ALIGN)) :	\
							HEAP_SIZE)


/*-------------------------------------------------------------------------------
* Heap-allocated memory
*------------------------------------------------------------------------------*/
static __align(HEAP_BYTE_ALIGN) uint8_t heap[ALIGNED_HEAP_SIZE];	


/*-------------------------------------------------------------------------------
* Number of bytes already allocated
*------------------------------------------------------------------------------*/
static size_t bytesUsed;				


/*-------------------------------------------------------------------------------
* Function:    	align_byte_number
* Purpose:    	Update the number of bytes requested for allocation so that it is 
*				compliant with the current byte alignment
* Arguments:	
* 		byteNumber - number of bytes requested
* Returns: 		
*		modified byte number to
--------------------------------------------------------------------------------*/
size_t align_byte_number(size_t byteNumber) {
	
	// Test if the number of bytes is a multiple of HEAP_BYTE_ALIGN. If not, add 
	// number of bytes necessary to enforce alignment
	size_t bytesToAlign = byteNumber % HEAP_BYTE_ALIGN;
	if (bytesToAlign)
		byteNumber += HEAP_BYTE_ALIGN - bytesToAlign;
	
	return byteNumber;
}


/////////////////////////////////////////////////////////////////////////////////
#if HEAP_MANAGER == 1
/////////////////////////////////////////////////////////////////////////////////


/*-------------------------------------------------------------------------------
* Function:    	heap_init
* Purpose:    	Heap initialisation function
* Arguments:	- 	
* Returns: 		-	
--------------------------------------------------------------------------------*/
void heap_init(void){
	bytesUsed = 0;						// number of bytes already used 
}

		
/*-------------------------------------------------------------------------------
* Function:    	malloc
* Purpose:    	Dynamically allocate bytesToAlloc bytes on the heap
* Arguments:	
*		bytesToAlloc - number of bytes to allocate	
* Returns: 
* 		pointer to the memory block allocated or a NULL pointer if unsuccessful
--------------------------------------------------------------------------------*/
void* malloc(size_t bytesToAlloc) {
	
	// pointer to the memory allocates
	void* pointer;
	
	// Check if enough space available and the number of bytes is valid
	if ((bytesToAlloc > 0) && (bytesToAlloc < ALIGNED_HEAP_SIZE - bytesUsed)) {
		
		// Ensure byte-alignment required
		bytesToAlloc = align_byte_number(bytesToAlloc);
		
		// Update the heap and return the pointer to newly allocated memory
		pointer = &heap[bytesUsed];
		bytesUsed += bytesToAlloc;
	}
	return pointer;
}


/*-------------------------------------------------------------------------------
* Function:    	free
* Purpose:    	Free the allocated block of memory
* Arguments:	
*		toFree - block of heap memory to free
* Returns: 		-
--------------------------------------------------------------------------------*/
void free(void* toFree) {}	



/////////////////////////////////////////////////////////////////////////////////
#elif HEAP_MANAGER == 2
/////////////////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------------------
* Heap free block definition
--------------------------------------------------------------------------------*/
typedef struct FreeBlock {
	size_t blockSize; 				// size in bytes
	struct FreeBlock* next; 		// pointer to the next free heap block
} FreeBlock;


/*-------------------------------------------------------------------------------
* Beginning and end of the list of free blocks. (Can't be assigned data)
--------------------------------------------------------------------------------*/
FreeBlock startBlock, endBlock;


/*-------------------------------------------------------------------------------
* Function:    	insert_free_block
* Purpose:    	Insert a new block into the list of free blocks
* Arguments:	
*		toInsert - pointer to the block to insert
* Returns: 		-
--------------------------------------------------------------------------------*/
void insert_free_block(FreeBlock* toInsert) {
	
	// Iterate through the linked list of blocks until one that has bigger size
	// is next
	FreeBlock* iterator = &startBlock;
	while (iterator->blockSize < iterator->next->blockSize) 
		iterator = iterator->next;
	
	// Insert the block
	toInsert->next = iterator->next;
	iterator->next = toInsert;
}


/*-------------------------------------------------------------------------------
* Function:    	heap_init
* Purpose:    	Heap initialisation function 
* Arguments:	- 	
* Returns: 		-	
--------------------------------------------------------------------------------*/
void heap_init(void){

	// Pointer to the first free block of data that can be used
	FreeBlock* firstBlock;
	
	// Reset the counter
	bytesUsed = 0;					
	
	// Initialise the two blocks used for management, the starting and ending ones
	startBlock.blockSize = 0;
	startBlock.next = (void*) &heap[0];
	endBlock.blockSize = ALIGNED_HEAP_SIZE;
	endBlock.next = NULL;
	
	// First free block used is placed right at the beginning of heap memory area
	// It occupies the whole heap memory and points to the endBlock
	firstBlock = (void*) &heap[0];
	firstBlock->blockSize = ALIGNED_HEAP_SIZE;
	firstBlock->next = &endBlock;
}


/*-------------------------------------------------------------------------------
* Function:    	malloc
* Purpose:    	Dynamically allocate bytesToAlloc bytes on the heap
* Arguments:	
*		bytesToAlloc - number of bytes to allocate	
* Returns: 
* 		pointer to the memory block allocated or a NULL pointer if unsuccessful
--------------------------------------------------------------------------------*/
void* malloc(size_t bytesToAlloc) {
	
	// Pointer to the memory allocated
	void* allocatedMemory;
	
	// Block pointers used for traversing the list
	FreeBlock *previousBlock, *iterator;
	
	if (bytesToAlloc > 0) {
		bytesToAlloc += sizeof(FreeBlock);
		bytesToAlloc = align_byte_number(bytesToAlloc);
	}
	
	if (bytesToAlloc > 0 && bytesToAlloc < HEAP_BYTE_ALIGN) {
		previousBlock = &startBlock;
		iterator = (void*) &heap[0];
		
		while ((iterator->blockSize < bytesToAlloc) && (iterator->next != NULL)) {
			previousBlock = iterator;
			iterator = iterator->next;
		}
		
		if (iterator != &endBlock) {
			allocatedMemory = ((void*) ((uint8_t*) previousBlock->next) + sizeof(FreeBlock));
			previousBlock->next = iterator->next;
			
			
			
			
			
		}
		
		
		
	}
	
}


#endif
