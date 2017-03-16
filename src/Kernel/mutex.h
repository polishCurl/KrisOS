/*******************************************************************************
* File:     	mutex.h
* Brief:    	Header file for mutex.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/12/2016
* Last mod: 	16/03/2017
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
* Purpose:    	Create a mutex using dynamic memory allocation.
* Arguments:	-
* Returns: 		
*		Pointer to the mutex created
--------------------------------------------------------------------------------*/
Mutex* mutex_create(void);



/*-------------------------------------------------------------------------------
* Function:    	mutex_delete
* Purpose:    	Delete the mutex specified 
* Arguments:	
*		toDelete - mutex to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t mutex_delete(Mutex* toDelete);
#endif



/*-------------------------------------------------------------------------------
* Function:    	mutex_try_lock
* Purpose:    	Attempt to lock the mutex specified. Don't wait if the mutex is 
*				already owned by some other task. 
* Arguments:	
*		toLock - mutex to lock
* Returns: 		
*		exit status. EXIT_FAILURE if the mutex can't be locked immediately
--------------------------------------------------------------------------------*/
uint32_t mutex_try_lock(Mutex* toLock);



/*-------------------------------------------------------------------------------
* Function:    	mutex_lock
* Purpose:    	Take the mutex given. Wait if the mutex is already taken.
* Arguments:	
*		toLock - mutex to lock
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t mutex_lock(Mutex* toLock);



/*-------------------------------------------------------------------------------
* Function:    	mutex_unlock
* Purpose:    	Unlock the mutex specified
* Arguments:	
*		toUnlock - mutex to unlock
* Returns: 		
*		exit status, EXIT_FAILURE if the calling task doesn't own the mutex specified 
--------------------------------------------------------------------------------*/
uint32_t mutex_unlock(Mutex* toUnlock);

#endif
