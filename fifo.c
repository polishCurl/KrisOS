/*******************************************************************************
* File:     	fifo.c
* Brief:    	Dynamic FIFO implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	18/10/2016
* Last mod: 	18/10/2016
*
* Note: 		The FIFO is circular and uses two indexes to keep track of the
* 				the first and last element in the FIFO. The FIFO is represented 
*				as a struct within which there is a pointer to an array of void
* 				pointers which allows storing any type of data (pointers actually)
* 				The fifo array is one word length greater then desired as
*******************************************************************************/
#include "common.h"
#include "fifo.h"
#include "special_reg_access.h"


/*-------------------------------------------------------------------------------
* Function:    	request_fifo
* Purpose:    	Creates and initialises a FIFO 
* Arguments:	
*		size - maximum number of elements FIFO should store
* Returns: 		
* 		pointer to the dynamically allocated FIFO
--------------------------------------------------------------------------------*/
FIFO* request_fifo(size_t size) {
	
	// Allocate memory for the FIFO struct
	FIFO* fifoPtr = malloc(sizeof(FIFO)); 
	TEST_NULL_POINTER(fifoPtr)
	
	// Initialise the indexes of the first and last element in the fifo
	// and the size of the fifo (+1 because of the gap entry)
	fifoPtr->tail = fifoPtr->head = 0;
	fifoPtr->size = size + 1;
	
	// Allocate memory for the actual data being an array of *void pointers
	fifoPtr->data = malloc(sizeof(fifoPtr->data) * (size + 1));
	TEST_NULL_POINTER(fifoPtr->data)
	
	return fifoPtr;
}


/*-------------------------------------------------------------------------------
* Function:    	fifo_put
* Purpose:    	Put an element into given FIFO
* Arguments:	
*		item - pointer to the element to enqueue in the FIFO
*		fifo - pointer to the FIFO that should be updated
* Returns: 		
* 		exit status
--------------------------------------------------------------------------------*/
int32_t fifo_put(void *item, FIFO* fifo) {
	
	// Test if FIFO is full, otherwise insert the item and update head
	if ((fifo->head + 1) % fifo->size == fifo->tail) {
		return EXIT_FAILURE;
	}
	else {
		__start_critical();
		fifo->data[fifo->head] = item;
		fifo->head = (fifo->head + 1) % fifo->size;
		__end_critical();
		return EXIT_SUCCESS;
	}
}


/*-------------------------------------------------------------------------------
* Function:    	fifo_get
* Purpose:    	Get the first element from the FIFO
* Arguments:	
*		fifo - pointer to the FIFO that should be updated
* Returns: 		
* 		pointer to the last item or NULL if FIFO is empty
--------------------------------------------------------------------------------*/
void* fifo_get(FIFO* fifo) {
	
	// Test if FIFO is empty, if not, return the first element from it
	void* item;
	if (fifo->tail == fifo->head) {
		return NULL;
	}
	else {
		__start_critical();
		item = fifo->data[fifo->tail];
		fifo->tail = (fifo->tail + 1) % fifo->size;
		__end_critical();
		return item;
	}
}


/*-------------------------------------------------------------------------------
* Function:    	remove_fifo
* Purpose:    	Remove the given FIFO from memory
* Arguments:	
*		fifo - pointer to the FIFO that should be removed
* Returns:		-
--------------------------------------------------------------------------------*/
void remove_fifo(FIFO* fifo) {
	free(fifo->data);
	free(fifo);
}


