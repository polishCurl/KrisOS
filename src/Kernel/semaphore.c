/*******************************************************************************
* File:     	semaphore.c
* Brief:    	Semaphore implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	02/02/2016
* Last mod: 	02/02/2016
*
* Note: 		
*******************************************************************************/
#include "system.h"
#include "kernel.h"



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
uint32_t sem_init(Semaphore* toInit, uint32_t startVal) {
	
	// Check if the Semaphore pointer is valid
	TEST_NULL_POINTER(toInit)
	
	toInit->waitingQueue = NULL;
	toInit->counter = startVal;
	
	// Update the total number of semaphores declared
	#ifdef SHOW_DIAGNOSTIC_DATA
		KrisOS.totalSemNo++;
	#endif	
	
	return EXIT_SUCCESS;
}



#ifdef USE_HEAP
/*-------------------------------------------------------------------------------
* Function:    	sem_create
* Purpose:    	Create a semaphore using heap and initialise it
* Arguments:	
* 		startVal - semaphore counter start value
* Returns: 		
*		pointer to the semaphore created
--------------------------------------------------------------------------------*/
Semaphore* sem_create(uint32_t startVal) {
	
	Semaphore* semCreated = malloc(sizeof(Semaphore));
	sem_init(semCreated, startVal);
	return semCreated;
}
#endif



/*-------------------------------------------------------------------------------
* Function:    	sem_delete
* Purpose:    	Remove the semaphore given
* Arguments:	
* 		toDelete - semaphore to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t sem_delete(Semaphore* toDelete) {
	
	// Validate the input argument
	TEST_NULL_POINTER(toDelete)
	
	__start_critical();
	{
		// Check if the semaphore isn't currently waited on
		if (toDelete->waitingQueue != NULL) {
			__end_critical();
			return EXIT_FAILURE;
		}
		
		// Update the total number of semaphores declared		
		#ifdef SHOW_DIAGNOSTIC_DATA
			KrisOS.totalSemNo--;	
		#endif		
		
		// Free the heap memory occupied (if allocated on heap)
		#ifdef USE_HEAP
			free(toDelete);
		#endif
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	sem_try_acquire
* Purpose:    	Attempt to decrement the semaphore without waiting if unsucessful
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: 		
*		exit status. If semaphore can't be taken exit status = EXIT_FAILURE
--------------------------------------------------------------------------------*/
uint32_t sem_try_acquire(Semaphore* toAcquire) {
	
	uint32_t exitStatus;
	TEST_NULL_POINTER(toAcquire)
	
	// Test if semaphore can be acquired immediately and return the outcome.
	__start_critical();
	{
		if (toAcquire->counter) {
			toAcquire->counter--;
			exitStatus = EXIT_SUCCESS;
		}
		else {
			exitStatus = EXIT_FAILURE;
		}
	}
	__end_critical();
	return exitStatus;
}



/*-------------------------------------------------------------------------------
* Function:    	sem_acquire
* Purpose:    	Decrement the semaphore. Wait on the semaphore if 
*				unsuccessful.
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: -
--------------------------------------------------------------------------------*/
uint32_t sem_acquire(Semaphore* toAcquire) {

	// Validate the semaphore pointer
	TEST_NULL_POINTER(toAcquire)
	__start_critical();
	{		
		// Check if the semaphore can be acquired immidietaly, otherwise ...
		if (sem_try_acquire(toAcquire) == EXIT_FAILURE) {
			
			// Link the semaphore with the calling task
			scheduler.runPtr->waitingObj = toAcquire;
			
			// Remove the calling task from the ready ones and reschedule the
			// remaining tasks
			task_remove(&scheduler.ready, scheduler.runPtr);
			scheduler.runPtr->status = SEM_WAIT;
			scheduler_run();
			
			// Add the calling task to the semaphore waiting queue
			task_add(&toAcquire->waitingQueue, scheduler.runPtr);
		}	
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	sem_release
* Purpose:    	Release the semaphore specified
* Arguments:	
*		toRelease - semaphore to release
* Returns: 		
*		exit status 
--------------------------------------------------------------------------------*/
uint32_t sem_release(Semaphore* toRelease) {
	
	Task* nextToAcquire;
	TEST_NULL_POINTER(toRelease)
	
	__start_critical();
	{	
		// If there is at least one task waiting on the semaphore then make it ready 
		if (toRelease->waitingQueue != NULL) {
			nextToAcquire = toRelease->waitingQueue;
			task_remove(&toRelease->waitingQueue, nextToAcquire);
			nextToAcquire->waitingObj = NULL;
			task_add(&scheduler.ready, nextToAcquire);
			scheduler_run();
		}
		// Otherwise increment the counter
		else {
			toRelease->counter++;
		}
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_release_ISR
* Purpose:    	Release the semaphore specified by an interrupt service routine
* Arguments:	
*		toRelease - semaphore to release		
* Returns: 		
*		exit status 
--------------------------------------------------------------------------------*/
uint32_t KrisOS_sem_release_ISR(Semaphore* toRelease) {
	return sem_release(toRelease);
}



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_acquire_ISR
* Purpose:    	Take the semaphore by an interrupt service routine. Don't wait if
*				unsuccessful
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: 		
*		exit status 
--------------------------------------------------------------------------------*/
uint32_t KrisOS_sem_acquire_ISR(Semaphore* toAcquire) {
	return sem_try_acquire(toAcquire);
}



#endif
