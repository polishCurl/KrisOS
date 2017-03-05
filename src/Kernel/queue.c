/*******************************************************************************
* File:     	queue.c
* Brief:    	Generic queue implementation to be used between tasks and 
*				task-interrupt communication.
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	02/03/2016
* Last mod: 	02/03/2017
*
* Note: 		
*******************************************************************************/
#include "kernel.h"
#include "system.h"



#ifdef USE_QUEUE
/*-------------------------------------------------------------------------------
* Function:    	queue_init
* Purpose:    	Initialise the queue given
* Arguments:	
* 		toInit - queue to initialise
*		bufferMemory - pointer to the memory area to be used as data buffer
*		capacity - buffer memory size (in number of items it can fit)
*		itemSize - size of a single item
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_init(Queue* toInit, void* bufferMemory, size_t capacity,
					size_t itemSize) {
	
	// Validate the input parameters
	TEST_NULL_POINTER(toInit)
	TEST_NULL_POINTER(bufferMemory)
	TEST_INVALID_SIZE(capacity)
	TEST_INVALID_SIZE(itemSize)
				
	// Set the initial values of member variables of the queue to initialise
	toInit->buffer = bufferMemory;
	toInit->bufferSize = capacity * itemSize;
	toInit->itemSize = itemSize;
	toInit->head = toInit->tail = 0;
	toInit->waitingToRead = toInit->waitingToWrite = NULL;
						
	// Update the total number of queues declared
	#ifdef SHOW_DIAGNOSTIC_DATA
		KrisOS.totalQueueNo++;
	#endif	
	
	return EXIT_SUCCESS;
}
					


/*-------------------------------------------------------------------------------
* Function:    	queue_create
* Purpose:    	Create and initialise a queue of given item size and capacity using
*				dynamic memory allocation.
* Arguments:	
*		capacity - buffer memory size (in number of items it can fit)
*		itemSize - size of a single item
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
Queue* queue_create(size_t capacity, size_t itemSize) {
	
	// Pointer to the queue created
	Queue* queueCreated;
	
	// Validate the input parameters
	TEST_INVALID_SIZE(capacity)
	TEST_INVALID_SIZE(itemSize)
	
	// Allocate memory for both the Queue struct and the data buffer
	queueCreated = malloc(sizeof(Queue));
	queueCreated->buffer = malloc(capacity * itemSize);
				
	// Initialise the queue parameters
	queue_init(queueCreated, queueCreated->buffer, capacity, itemSize);
	return queueCreated;
}



/*-------------------------------------------------------------------------------
* Function:    	queue_delete
* Purpose:    	Remove the queue given
* Arguments:	
* 		toDelete - queue to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_delete(Queue* toDelete) {
	
	// Check if the queue pointer is valid
	TEST_NULL_POINTER(toDelete)
	
	__start_critical();
	{
		// Check if there are tasks waiting to read/write from the queue given
		if (toDelete->waitingToRead != NULL || toDelete->waitingToWrite != NULL) {
			__end_critical();
			return EXIT_FAILURE;
		}
		
		// Reset some variables to indicate that the queue is no longer in use.
		// This is useful for spotting 'deleted' statically allocated data structures
		// which can't actually be physically removed from memory.
		toDelete->bufferSize = toDelete->itemSize = 0;
		
		// Free the heap memory occupied by the queue and its buffer (if allocated 
		// on heap) 
		free(toDelete->buffer);
		free(toDelete);
	}
	__end_critical();

	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	queue_try_write
* Purpose:    	Put the item given in the queue specified. Don't wait if the queue
*				is full.
* Arguments:	
* 		toWrite - queue to write to
*		item - pointer to the item to be put in the queue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_try_write(Queue* toWrite, const void* item) {
	
	// Check if the queue pointer is valid
	TEST_NULL_POINTER(toWrite)
	
	__start_critical();
	{
		// Test if the queue is full and the write operation can't be completed
		if ((toWrite->head + toWrite->itemSize) % toWrite->bufferSize == toWrite->tail) {
			__end_critical();
			return EXIT_FAILURE;
		}
		
		// Copy the item to put in the queue by value and update the head pointer
		memcpy(&toWrite->buffer[toWrite->head], item, toWrite->itemSize);
		toWrite->head = (toWrite->head + toWrite->itemSize) % toWrite->bufferSize;
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	queue_try_read
* Purpose:    	Read the next item from the queue specified. Don't wait if the
*				queue is empty.
* Arguments:	
* 		toRead - queue to read from
*		item - item read from the queue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_try_read(Queue* toRead, void* item) {
	
	// Check if the queue pointer is valid
	TEST_NULL_POINTER(toRead)
	
	__start_critical();
	{
		// Test if the queue is empty and the read operation can't be completed
		if (toRead->tail == toRead->head) {
			__end_critical();
			return EXIT_FAILURE;
		}
		
		// Copy the item to read from the queue to the variable to be returned and 
		// update the tail pointer
		memcpy(item, &toRead->buffer[toRead->tail], toRead->itemSize);
		toRead->tail = (toRead->tail + toRead->itemSize) % toRead->bufferSize;
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	queue_try_write
* Purpose:    	Put the item given in the queue specified. Wait if the queue
*				is full.
* Arguments:	
* 		toWrite - queue to write to
*		item - pointer to the item to be put in the queue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_write(Queue* toWrite, const void* item) {
	
	// Check if the queue pointer is valid
	TEST_NULL_POINTER(toWrite)
	
	__start_critical();
	{
		// Check if the semaphore can be acquired immidietaly, otherwise ...
		if (queue_try_write(toWrite, item) == EXIT_FAILURE) {
			
			// The running task is now waiting for the queue to become non-full
			scheduler.runPtr->waitingObj = toWrite;
			
			// Remove the calling task from the ready ones and reschedule the
			// remaining tasks
			task_remove(&scheduler.ready, scheduler.runPtr);
			scheduler.runPtr->status = QUEUE_WRITE_WAIT;
			scheduler_run();
			
			// Add the calling task to the 'write' waiting queue
			task_add(&toWrite->waitingToWrite, scheduler.runPtr);
		}	
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	queue_read
* Purpose:    	Read the next item from the queue specified. Wait if the
*				queue is empty.
* Arguments:	
* 		toRead - queue to read from
*		item - item read from the queue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_read(Queue* toRead, void* item) {
	
	// Check if the queue pointer is valid
	TEST_NULL_POINTER(toRead)
	
	__start_critical();
	{
		// Check if the semaphore can be acquired immidietaly, otherwise ...
		if (queue_try_read(toRead, item) == EXIT_FAILURE) {
			
			// The running task is now waiting for the queue to become non-full
			scheduler.runPtr->waitingObj = toRead;
			
			// Remove the calling task from the ready ones and reschedule the
			// remaining tasks
			task_remove(&scheduler.ready, scheduler.runPtr);
			scheduler.runPtr->status = QUEUE_READ_WAIT;
			scheduler_run();
			
			// Add the calling task to the 'write' waiting queue
			task_add(&toRead->waitingToWrite, scheduler.runPtr);
		}	
	}
	return EXIT_SUCCESS;
}




#endif
