/*******************************************************************************
* File:     	fifo.h
* Brief:    	Generic FIFO implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	28/09/2016
* Last mod: 	28/09/2016
*
* Note: 		FIFO build macro allowing specification of FIFOs with
* 				different names, datatypes and sizes. Also, interface to the 
* 				dynamically allocated fifos
*******************************************************************************/
#include "special_reg_access.h"


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_static_fifo
* Purpose:    	Creates a new FIFO together with methods for initialisation and
* 				access at compile-time.
*					1. <NAME>Fifo[SIZE]
*					2. void <NAME>_fifo_init(void);
*					3. int32_t <NAME>_fifo_put(TYPE item); (0 - failure, 1 - success)
*					4. int32_t <NAME>_fifo_get(TYPE* item); (0 - failure, 1- success)
* Arguments:	
*		TYPE - datatype of elements stored in the FIFO
*		SIZE - maximum numer of elements the fifo can store
*		NAME - prefix for creating FIFO and methods with unique names
* Returns: 		-	
--------------------------------------------------------------------------------*/
#define KrisOS_static_fifo(TYPE, SIZE, NAME) 			\
														\
	/* Fifo elements */									\
	TYPE NAME ## Fifo[SIZE];							\
	uint32_t NAME ## Head;								\
	uint32_t NAME ## Tail;								\
														\
	/* Fifo initialisation function */					\
	void NAME ## _fifo_init(void) {						\
		NAME ## Head = NAME ## Tail = 0;				\
	}													\
														\
	/* Put element into fifo */							\
	int32_t NAME ## _fifo_put(TYPE item) {				\
		if ((NAME ## Head + 1) % SIZE == NAME ## Tail)	\
			return 0;									\
		else {											\
			__start_critical();							\
			NAME ## Fifo[NAME ## Head] = item;			\
			NAME ## Head = (NAME ## Head + 1) % SIZE;	\
			__end_critical();							\
			return 1;									\
		}												\
	}													\
														\
	/* Get element from fifo */							\
	int32_t NAME ## _fifo_get(TYPE* item) {				\
	if (NAME ## Tail == NAME ## Head )					\
		return 0;										\
	else {												\
		__start_critical();								\
		*item = NAME ## Fifo[NAME ## Tail];				\
		NAME ## Tail = (NAME ## Tail + 1) % SIZE;		\
		__end_critical();								\
		return 1;										\
	}													\
}										
	
