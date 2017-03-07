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
	toInit->head = toInit->tail = toInit->buffer;
	sem_init(&toInit->elementsStored, 0);
	sem_init(&toInit->remainingCapacity, capacity);
						
	// Update the total number of queues declared
	#ifdef SHOW_DIAGNOSTIC_DATA
		KrisOS.totalQueueNo++;
	#endif	
	
	return EXIT_SUCCESS;
}
					


#ifdef USE_HEAP
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
#endif



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
		if (toDelete->elementsStored.waitingQueue != NULL || 
			toDelete->remainingCapacity.waitingQueue != NULL) {
			__end_critical();
			return EXIT_FAILURE;
		}
			
		// Free the heap memory occupied by the queue and its buffer (if allocated 
		// on heap) 
		#ifdef USE_HEAP
			free(toDelete->buffer);
			free(toDelete);
		#endif
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
	
	// Test if the queue is full and the write operation can't be completed
	if (sem_try_acquire(&toWrite->remainingCapacity) == EXIT_FAILURE) 
		return EXIT_FAILURE;
	
	// Write to the queue
	queue_enqueue(toWrite, item);
			
	// Update the number of elements stored and notify the tasks that
	// are waiting to read
	sem_release(&toWrite->elementsStored);
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
	
	// Test if the queue is empty and the read operation can't be completed
	if (sem_try_acquire(&toRead->elementsStored) == EXIT_FAILURE)
		return EXIT_FAILURE;
	
	// Read from the queue
	queue_dequeue(toRead, item);
	
	// Update the number of items that can still fit in the buffer and notify
	// the tasks waiting to write.
	sem_release(&toRead->remainingCapacity);
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_queue_write
* Purpose:    	Put the item given in the queue specified. Wait if the queue
*				is full.
* Arguments:	
* 		toWrite - queue to write to
*		item - pointer to the item to be put in the queue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t KrisOS_queue_write(Queue* toWrite, const void* item) {
	
	// Check if the queue pointer is valid
	TEST_NULL_POINTER(toWrite)
	
	// Wait to write if necessary
	KrisOS_sem_acquire(&toWrite->remainingCapacity);
	
	// Write to the queue
	KrisOS_queue_enqueue(toWrite, item);
	
	// Update the number of elements stored and notify the tasks that
	// are waiting to read
	KrisOS_sem_release(&toWrite->elementsStored);
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_queue_read
* Purpose:    	Read the next item from the queue specified. Wait if the
*				queue is empty.
* Arguments:	
* 		toRead - queue to read from
*		item - item read from the queue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t KrisOS_queue_read(Queue* toRead, void* item) {
	
	// Check if the queue pointer is valid
	TEST_NULL_POINTER(toRead)
	
	// Wait to read if necessary
	KrisOS_sem_acquire(&toRead->elementsStored);
	
	// Read from the queue
	KrisOS_queue_dequeue(toRead, item);
	
	// Update the number of items that can still fit in the buffer and notify
	// the tasks waiting to write.
	KrisOS_sem_release(&toRead->remainingCapacity);
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	queue_enqueue
* Purpose:    	Place the item given in the queue specified
* Arguments:	
* 		queue - queue to update
*		item - item to enqueue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_enqueue(Queue* queue, const void* item) {

	__start_critical();
	{
		// Copy-by-value the item to enqueue and update the head pointer
		memcpy(queue->head, item, queue->itemSize);
		queue->head += queue->itemSize;
		if (queue->head == queue->buffer + queue->bufferSize)
			queue->head = queue->buffer;
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	queue_dequeue
* Purpose:    	Remove the next item from the queue and pass it to 'item'.
* Arguments:	
* 		queue - queue to update
*		item - item to enqueue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_dequeue(Queue* queue, void* item) {

	__start_critical();
	{		
		// Read the item from the queue and update the tail pointer 
		memcpy(item, queue->tail, queue->itemSize);
		queue->tail += queue->itemSize;
		if (queue->tail == queue->buffer + queue->bufferSize)
			queue->tail = queue->buffer;
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_queue_write_ISR
* Purpose:    	Put the item given in the queue specified while inside ISR (don't wait)
* Arguments:	
* 		toWrite - queue to write to
*		item - pointer to the item to be put in the queue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t KrisOS_queue_write_ISR(Queue* toWrite, const void* item) {
	return queue_try_write(toWrite, item);
}



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_queue_read_ISR
* Purpose:    	Read the next item from the queue specified while inside ISR (don't wait)
* Arguments:	
* 		toRead - queue to read from
*		item - item read from the queue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t KrisOS_queue_read_ISR(Queue* toRead, void* item) {
	return queue_try_read(toRead, item);
}


#endif
