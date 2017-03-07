/*******************************************************************************
* File:     	mutex.h
* Brief:    	Header file for mutex.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/12/2016
* Last mod: 	21/12/2016
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"



#ifdef USE_MUTEX
/*-------------------------------------------------------------------------------
* Function:    	mutex_init
* Purpose:    	Initialise the mutex given
* Arguments:	
* 		toInit - mutex to initialise
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t mutex_init(Mutex* toInit);



#ifdef USE_HEAP
/*-------------------------------------------------------------------------------
* Function:    	mutex_create
* Purpose:    	Create a mutex using dynamic memory
* Arguments:	-
* Returns: 		
*		Pointer to the mutex created
--------------------------------------------------------------------------------*/
Mutex* mutex_create(void);
#endif



/*-------------------------------------------------------------------------------
* Function:    	mutex_try_lock
* Purpose:    	Attempts to lock the mutex specified. If mutex is already owned then
*				the calling function doesn't wait until it's released.
* Arguments:	
*		toLock - mutex to lock
* Returns: 		
*		exit status (if lock already acquired - EXIT_FAILURE)
--------------------------------------------------------------------------------*/
uint32_t mutex_try_lock(Mutex* toLock);



/*-------------------------------------------------------------------------------
* Function:    	mutex_lock
* Purpose:    	Take the mutex given. If already taken, place the calling task in
* 				the waiting queue
* Arguments:	
*		toLock - mutex to lock
* Returns: 		
*		exit status (if lock already acquired - EXIT_FAILURE)
--------------------------------------------------------------------------------*/
uint32_t mutex_lock(Mutex* toLock);



/*-------------------------------------------------------------------------------
* Function:    	mutex_unlock
* Purpose:    	Release the mutex specified
* Arguments:	
*		toUnlock - mutex to unlock
* Returns: 		
*		exit status (if lock already acquired - EXIT_FAILURE)
--------------------------------------------------------------------------------*/
uint32_t mutex_unlock(Mutex* toUnlock);



/*-------------------------------------------------------------------------------
* Function:    	mutex_delete
* Purpose:    	Delete the mutex specified (if created using dynamic memory)
* Arguments:	
*		toDelete - mutex to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t mutex_delete(Mutex* toDelete);


#endif
