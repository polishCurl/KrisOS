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
	
	TEST_NULL_POINTER(toInit)
	toInit->waitingQueue = NULL;
	toInit->counter = startVal;
	
	// Update the total number of semaphores declared
	#ifdef SHOW_DIAGNOSTIC_DATA
		KrisOS.totalSemNo++;
	#endif	
	
	return EXIT_SUCCESS;
}



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



/*-------------------------------------------------------------------------------
* Function:    	sem_delete
* Purpose:    	Remove the semaphore given
* Arguments:	
* 		toDelete - semaphore to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t sem_delete(Semaphore* toDelete) {
	
	// Reset the member variables to indicate that the semaphore is no longer in use
	TEST_NULL_POINTER(toDelete)
	toDelete->counter = UINT32_MAX;
	toDelete->waitingQueue = NULL;
	
	// Update the total number of semaphores declared		
	#ifdef SHOW_DIAGNOSTIC_DATA
		KrisOS.totalSemNo--;	
	#endif		
	
	// Free the heap memory occupied (if allocated on heap)
	free(toDelete);
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	sem_try_acquire
* Purpose:    	Attempt to decrement the semaphore without waiting if unsucessful
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: 		
*		exit status
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
* Purpose:    	Attempt to decrement the semaphore. Wait if unsuccessful for maximum
* 				of 'timout' milliseconds
* Arguments:	
* 		toAcquire - semaphore to acquire
* 		timout - semaphore timeout
* Returns: -
--------------------------------------------------------------------------------*/
uint32_t sem_acquire(Semaphore* toAcquire, uint32_t timout) {

	TEST_NULL_POINTER(toAcquire)
	__start_critical();
	{
		Task* callingTask = scheduler.runPtr;
		
		// Check if the semaphore can be acquired immidietaly, otherwise ...
		if (sem_try_acquire(toAcquire) == EXIT_FAILURE) {
			
			// Link the semaphore with the calling task
			callingTask->waitingObj = toAcquire;
			
			// If valid timout is specified, then put the calling task to sleep for 
			// the amount of time specified
			if (timout != TIME_INFINITY) {
				task_sleep(timout, SEM_WAIT);
			}
			// If there is no wait timout then simply remove the calling task from
			// the ready ones and run the scheduler
			else {
				task_remove(&scheduler.ready, callingTask);
				callingTask->status = SEM_WAIT;
				scheduler_run();
			}
			
			// Add the calling task to the semaphore waiting queue regardless of the
			// timout
			task_add(&toAcquire->waitingQueue, callingTask);
		}	
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	sem_release
* Purpose:    	Release the semaphore specified
* Arguments:	
*		toRelease - mutex to unlock
* Returns: 		
*		exit status (if lock already acquired - EXIT_FAILURE)
--------------------------------------------------------------------------------*/
uint32_t sem_release(Semaphore* toRelease) {
	
	Task* nextToAcquire;
	TEST_NULL_POINTER(toRelease)
	
	__start_critical();
	{	
		// If there is at least one task waiting on the semaphore then ready it 
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




#endif
