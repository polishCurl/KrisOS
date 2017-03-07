/*******************************************************************************
* File:     	queue.h
* Brief:    	Header file for queue.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	03/03/2016
* Last mod: 	03/03/2016
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"



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
					size_t itemSize);
					


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
Queue* queue_create(size_t capacity, size_t itemSize);
#endif



/*-------------------------------------------------------------------------------
* Function:    	queue_delete
* Purpose:    	Remove the queue given
* Arguments:	
* 		toDelete - queue to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_delete(Queue* toDelete);



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
uint32_t queue_try_write(Queue* toWrite, const void* item);



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
uint32_t queue_try_read(Queue* toRead, void* item);



/*-------------------------------------------------------------------------------
* Function:    	queue_enqueue
* Purpose:    	Place the item given in the queue specified.
* Arguments:	
* 		queue - queue to update
*		item - item to enqueue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_enqueue(Queue* queue, const void* item);
 


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_queue_enqueue
* Purpose:    	Place the item given in the queue specified. SVC call.
* Arguments:	
* 		queue - queue to update
*		item - item to enqueue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_QUEUE_ENQUEUE) KrisOS_queue_enqueue(Queue* queue, const void* item);



/*-------------------------------------------------------------------------------
* Function:    	queue_dequeue
* Purpose:    	Remove the next item from the queue and pass it to 'item'.
* Arguments:	
* 		queue - queue to update
*		item - item to enqueue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t queue_dequeue(Queue* queue, void* item);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_queue_dequeue
* Purpose:    	Remove the next item from the queue and pass it to 'item'. SVC Call.
* Arguments:	
* 		queue - queue to update
*		item - item to enqueue
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_QUEUE_DEQUEUE) KrisOS_queue_dequeue(Queue* queue, void* item);

#endif 
