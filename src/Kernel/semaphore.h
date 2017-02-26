/*******************************************************************************
* File:     	semaphore.h
* Brief:    	Header file for semaphore.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	02/02/2016
* Last mod: 	02/02/2016
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"



#ifdef USE_SEMAPHORE
/*-------------------------------------------------------------------------------
* Function:    	sem_init
* Purpose:    	Initialise the semaphore given
* Arguments:	
* 		toInit - semaphore to initialise
* 		startVal - semaphore counter start value
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t sem_init(Semaphore* toInit, uint32_t startVal);



/*-------------------------------------------------------------------------------
* Function:    	sem_create
* Purpose:    	Create a semaphore using heap and initialise it
* Arguments:	
* 		startVal - semaphore counter start value
* Returns: 		
*		pointer to the semaphore created
--------------------------------------------------------------------------------*/
Semaphore* sem_create(uint32_t startVal);



/*-------------------------------------------------------------------------------
* Function:    	sem_delete
* Purpose:    	Remove the semaphore given
* Arguments:	
* 		toDelete - semaphore to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t sem_delete(Semaphore* toDelete);



/*-------------------------------------------------------------------------------
* Function:    	sem_try_acquire
* Purpose:    	Attempt to decrement the semaphore without waiting if unsucessful
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t sem_try_acquire(Semaphore* toAcquire);



/*-------------------------------------------------------------------------------
* Function:    	sem_acquire
* Purpose:    	Attempt to decrement the semaphore. Wait if unsuccessful for maximum
* 				of 'timout' milliseconds
* Arguments:	
* 		toAcquire - semaphore to acquire
* 		timout - semaphore timeout (timeout = 0 causes infinite wait on semaphore)
* Returns: -
--------------------------------------------------------------------------------*/
uint32_t sem_acquire(Semaphore* toAcquire, uint32_t timout);



/*-------------------------------------------------------------------------------
* Function:    	sem_release
* Purpose:    	Release the semaphore specified
* Arguments:	
*		toRelease - semaphore to release
* Returns: 		
*		exit status (if lock already acquired - EXIT_FAILURE)
--------------------------------------------------------------------------------*/
uint32_t sem_release(Semaphore* toRelease);

#endif
