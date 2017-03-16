/*******************************************************************************
* File:     	heap_manager.c
* Brief:    	KrisOS heap manager implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	28/09/2016
* Last mod: 	14/03/2017
*
* Note: 		
*	The heap manager is implemented as a set of blocks (used/free) Each separate 
*	block of heap memory has an overhead of 8 bytes of metadata (the HeapBlock 
*	struct), which is used for describing a continuous subset of (used/free)
*	heap memory
*		1. Block size - size of the memory area that HeapBlock is describing
*		2. Next pointer - pointer to the next block of heap memory
*
*	Only the free heap blocks are chained together in a linked list. To do that
*	two extra (meta)blocks are used which mark the start and end of the list of
*	free heap block list. The start block is declared separately from the heap
*	memory space, but the end block occupies the last 8 bytes of heap memory.
*	So, after initialisation the heap usage is equal to 8 bytes already (instead
*	of 0). Initially, there exist only a single free heap block which occupies
*	the entire (almost) heap memory and links together the start and end blocks.
*
*	To avoid external memory fragmentation each new free heap block, either freed
*	or created from existing one, is re-inserted into the list and if any of its
*	neighbours are directly adjacent, the free blocks are merged together,
*
* 	This heap manager implementation overrides the <stdlib.h> malloc and free
*	function declarations. Malloc terminates the OS if there is insufficient free
*	heap memory left.
*******************************************************************************/
#include "KrisOS.h"
#include "kernel.h"
#include "system.h"
#ifdef USE_HEAP



/*-------------------------------------------------------------------------------
* Allocate memory for the heap manager at compile time
--------------------------------------------------------------------------------*/
HeapManager heap;



/*-------------------------------------------------------------------------------
* Function:    	heap_init
* Purpose:    	Heap initialisation function
* Arguments:	- 	
* Returns: 		-	
--------------------------------------------------------------------------------*/
void heap_init(void){

	// Pointer to the first free block, which is not startBlock or endBlock (metablock) 
	HeapBlock* firstBlock;
	
	// Helper pointer for end HeapBlock address computation
	uint8_t* endBlockAdr;
	
	// Reset the heap usage counter. The end block is located within the heap memory
	// so the heap usage is reset to a non-zero value
	heap.heapBytesUsed = sizeof(HeapBlock);					
	
	// Initialise the startBlock and endBlock which mark the beginning and end of
	// the list of free heap blocks (they don't describe an actual susbset of 
	// heap-designated memory).
	// Make the startBlock unallocatable by setting its size to 0 and making
	// it point next to the start of the heap memory space (firstBlock)
	heap.startBlock.blockSize = 0;
	heap.startBlock.next = (void*) heap.heapMem;
	
	// Initialise the endBlock and place it inside the heap memory area right
	// at the end of it (last 8 bytes)
	endBlockAdr = heap.heapMem + ALIGNED_HEAP_SIZE - sizeof(HeapBlock);
	heap.endBlock = (void*) endBlockAdr;
	heap.endBlock->blockSize = ALIGNED_HEAP_SIZE;
	heap.endBlock->next = NULL;
	
	// First free block used is placed right at the beginning of heap memory area
	// It occupies the whole heap memory and points to the endBlock
	firstBlock = (void*) heap.heapMem;
	firstBlock->blockSize = ALIGNED_HEAP_SIZE - sizeof(HeapBlock);
	firstBlock->next = heap.endBlock;
	
	// Initialise the heap mutex
	#ifdef USE_MUTEX
		mutex_init(&heap.heapMutex);
	#endif
}



/*-------------------------------------------------------------------------------
* Function:    	malloc
* Purpose:    	Dynamically allocate bytesToAlloc bytes of memory
* Arguments:	
*		bytesToAlloc - number of bytes to allocate on heap
* Returns: 
* 		pointer to the memory block allocated. Doesn't return if unsuccessful
--------------------------------------------------------------------------------*/
void* malloc(size_t bytesToAlloc) {
	
	// Pointer to the memory allocated
	void* allocatedMemory;
	
	// Block pointers used for traversing the free heap block list
	HeapBlock *previousBlock, *iterator;
	
	// Pointer to a new free heap block that can be created (to avoid internal 
	// fragmentation) from the block allocated in this request
	HeapBlock *subBlock;
	
	// Check if the request is valid
	TEST_INVALID_SIZE(bytesToAlloc)
	
	// In addition to the memory to serve the request, extra 8 bytes need to
	// be allocated for the HeapBlock which will be describing it 
	bytesToAlloc += sizeof(HeapBlock);
	bytesToAlloc = heap_align_byte_number(bytesToAlloc);
	
	// If the heap is big enough to meet the request...
	if (bytesToAlloc < ALIGNED_HEAP_SIZE) {
		
		#ifdef USE_MUTEX
			mutex_lock(&heap.heapMutex);
		#else
			__start_critical();
		#endif
		{
			// Go through all the existing free blocks until a large enough is found
			// or we reached the endBlock 
			previousBlock = &heap.startBlock;
			iterator = heap.startBlock.next;
			while ((iterator->blockSize < bytesToAlloc) && (iterator->next != NULL)) {
				previousBlock = iterator;
				iterator = iterator->next;
			}
			
			// If a large enough block exists, remove it from free block list by 
			// joining its neighbours together
			if (iterator != heap.endBlock) {
				allocatedMemory = (void*) (((uint8_t*) previousBlock->next) + sizeof(HeapBlock));
				previousBlock->next = iterator->next;
				
				// If the size difference between the requested memory and the free 
				// heap block found is too big, split the block found into two 
				// subblocks and insert the unallocated part back to the free heap
				// block list
				if (iterator->blockSize - bytesToAlloc > MIN_BLOCK_SIZE) {
					subBlock = (void*) (((uint8_t*) iterator) + bytesToAlloc);
					subBlock->blockSize = iterator->blockSize - bytesToAlloc;
					iterator->blockSize = bytesToAlloc;
					heap_insert_free_block(subBlock);
				}	
				heap.heapBytesUsed += iterator->blockSize;
				
				#ifdef USE_MUTEX
					mutex_unlock(&heap.heapMutex);
				#else
					__end_critical();
				#endif
				return allocatedMemory;	
			}	
		}
		#ifdef USE_MUTEX
			mutex_lock(&heap.heapMutex);
		#else
			__start_critical();
		#endif		
	}
		
	// If there is insufficient free heap memory left to serve this request
	// terminate the OS prematurely 
	exit(EXIT_HEAP_TOO_SMALL);
	return NULL;
}



/*-------------------------------------------------------------------------------
* Function:    	free
* Purpose:    	Free the allocated block of memory
* Arguments:	
*		toFree - block of heap memory to free
* Returns: 		-
--------------------------------------------------------------------------------*/
void free(void* toFree) {
	
	// Byte array and actual heap block to free
	uint8_t* bytesToFree;
	HeapBlock* blockToFree;
	
	// Validate the input argument
	TEST_NULL_POINTER(toFree)
	
	// Test if the memory to free actually belongs to heap
	if (toFree < (void*) &heap.heapMem[0] || toFree >= (void*) &heap.heapMem[ALIGNED_HEAP_SIZE])
		return;
	
	// Extract the HeapBlock metadata from the input argument pointer and re-insert 
	// the block back to the list of free heap blocks
	bytesToFree = (uint8_t*) toFree;
	bytesToFree -= sizeof(HeapBlock);
	blockToFree = (HeapBlock*) bytesToFree;
	
	#ifdef USE_MUTEX
		mutex_lock(&heap.heapMutex);
	#else
		__start_critical();
	#endif
	{
		heap.heapBytesUsed -= blockToFree->blockSize;
		heap_insert_free_block(blockToFree);	
	}
	#ifdef USE_MUTEX
		mutex_unlock(&heap.heapMutex);
	#else
		__end_critical();
	#endif
}	



/*-------------------------------------------------------------------------------
* Function:    	heap_insert_free_block
* Purpose:    	Insert a new block into the list of free blocks in ascending block 
* 				size order
* Arguments:	
*		toInsert - pointer to the block to insert
* Returns: 		-
--------------------------------------------------------------------------------*/
void heap_insert_free_block(HeapBlock* toInsert) {
	
	// Iterator through the list of free blocks and helper pointer for finding 
	// neighbouring free heap blocks
	HeapBlock* iterator;
	uint8_t* neighbourLocator; 
	
	// Iterate through the linked list of blocks until one with higher address is found
	iterator = &heap.startBlock;
	while (iterator->next < toInsert) 
		iterator = iterator->next;
	
	// Test if the block preceding the one to insert is adjacent (they form contiguous
	// block of memory). If so, merge them.
	neighbourLocator = (uint8_t*) iterator;
	if (neighbourLocator + iterator->blockSize == (uint8_t*) toInsert) {
		iterator->blockSize += toInsert->blockSize ;
		toInsert = iterator;
	}
	
	// Test if the block following the one to insert is adjacent (they form contiguous
	// block of memory). If so, merge them.
	neighbourLocator = (uint8_t*) toInsert;
	if (neighbourLocator + toInsert->blockSize == (uint8_t*) iterator->next) {
		
		// The block is inserted right at the end of heap memory area
		if (iterator->next == heap.endBlock) {
			toInsert->next = heap.endBlock;
		}
		// Remaining cases
		else {
			toInsert->blockSize += iterator->next->blockSize;
			toInsert->next = iterator->next->next;
		}
	}
	else {
		toInsert->next = iterator->next;
	}
	
	// If both the preceding and following blocks were adjacent to the one to insert
	// then the next pointer needs to be corrected
	if (iterator != toInsert)
		iterator->next = toInsert;

}



/*-------------------------------------------------------------------------------
* Function:    	heap_align_byte_number
* Purpose:    	Update the number of bytes requested for allocation so that it is 
*				compliant with the current byte alignment
* Arguments:	
* 		byteNumber - number of bytes requested
* Returns: 		
*		modified byte number to
--------------------------------------------------------------------------------*/
size_t heap_align_byte_number(size_t byteNumber) {
	
	// Test if the number of bytes is a multiple of HEAP_BYTE_ALIGN. If not, add 
	// number of bytes necessary to enforce alignment
	size_t bytesToAlign = byteNumber % HEAP_BYTE_ALIGN;
	if (bytesToAlign)
		byteNumber += HEAP_BYTE_ALIGN - bytesToAlign;
	
	return byteNumber;
}

#endif
