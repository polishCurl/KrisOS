/*******************************************************************************
* File:     	fifo.h
* Brief:    	Static and dynamic generic FIFO implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	28/09/2016
* Last mod: 	28/09/2016
*
* Note: 		FIFO definition, both static (multiline MACRO) and dynamic (using
* 				struct) to be used by the OS (Handler mode). FIFOs are implemented
* 				using an array of size one larger than requested but they keep one
*				spare entry. Thread synchronisation is achieved, thanks to critical
* 				sections
*******************************************************************************/
#include "special_reg_access.h"


/*-------------------------------------------------------------------------------
* Macro:    	add_fifo
* Purpose:    	Adds a static FIFO at compile-time together with methods for 
* 				FIFO initialisation and access.
*					1. <NAME>Fifo[SIZE]
*					2. void <NAME>_fifo_init(void);
*					3. int32_t <NAME>_fifo_put(TYPE item); (0 - failure, 1 - success)
*					4. int32_t <NAME>_fifo_get(TYPE* item); (0 - failure, 1- success)
* Arguments:	
*		NAME - prefix for creating FIFO and methods with unique names
*		TYPE - datatype of elements stored in the FIFO
*		SIZE - maximum numer of elements the fifo can store
* Returns: 		-	
--------------------------------------------------------------------------------*/
#define add_fifo(NAME, TYPE, SIZE) 								\
																\
	/* Fifo elements */											\
	TYPE NAME ## Fifo[SIZE + 1];								\
																\
	/* Indexes of last and first elements in FIFO */			\
	uint32_t NAME ## Head;										\
	uint32_t NAME ## Tail;										\
																\
	/* Fifo initialisation function */							\
	void NAME ## _fifo_init(void) {								\
		NAME ## Head = NAME ## Tail = 0;						\
	}															\
																\
	/* Put element into fifo */									\
	int32_t NAME ## _fifo_put(TYPE item) {						\
		if ((NAME ## Head + 1) % (SIZE + 1) == NAME ## Tail)	\
			return 0;											\
		else {													\
			__start_critical();									\
			NAME ## Fifo[NAME ## Head] = item;					\
			NAME ## Head = (NAME ## Head + 1) % (SIZE + 1);		\
			__end_critical();									\
			return 1;											\
		}														\
	}															\
																\
	/* Get element from fifo */									\
	int32_t NAME ## _fifo_get(TYPE* item) {						\
	if (NAME ## Tail == NAME ## Head )							\
		return 0;												\
	else {														\
		__start_critical();										\
		*item = NAME ## Fifo[NAME ## Tail];						\
		NAME ## Tail = (NAME ## Tail + 1) % (SIZE + 1);				\
		__end_critical();										\
		return 1;												\
	}															\
}										
	


/*-------------------------------------------------------------------------------
* Dynamically allocated FIFO data structure
--------------------------------------------------------------------------------*/
typedef struct {
	uint32_t tail;		// index of the last element in the FIFO
	uint32_t head; 		// index of the first element in the FIFO
	size_t size; 		// number of elements (pointers) stored
	void **data; 		// array of pointers for generic use
} FIFO;



/*-------------------------------------------------------------------------------
* Function:    	request_fifo
* Purpose:    	Creates and initialises a FIFO 
* Arguments:	
*		size - maximum number of elements FIFO should store
* Returns: 		
* 		pointer to the dynamically allocated FIFO
--------------------------------------------------------------------------------*/
FIFO* request_fifo(size_t size);


/*-------------------------------------------------------------------------------
* Function:    	fifo_put
* Purpose:    	Put an element into given FIFO
* Arguments:	
*		item - pointer to the element to enqueue in the FIFO
*		fifo - pointer to the FIFO that should be updated
* Returns: 		
* 		0 - failure, 1 - success
--------------------------------------------------------------------------------*/
int32_t fifo_put(void *item, FIFO* fifo);


/*-------------------------------------------------------------------------------
* Function:    	fifo_get
* Purpose:    	Get the first element from the FIFO
* Arguments:	
*		fifo - pointer to the FIFO that should be updated
* Returns: 		
* 		pointer to the last item or NULL if FIFO is empty
--------------------------------------------------------------------------------*/
void* fifo_get(FIFO* fifo);

