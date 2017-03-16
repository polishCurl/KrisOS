/*******************************************************************************
* File:     	semaphore.c
* Brief:    	Semaphore implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	02/02/2016
* Last mod: 	16/03/2017
*
* Note: 		
*	Semaphores are a standard task synchronisation primitive. Contrary to mutual
*	exclusion locks, they don't have owners, so can be released from interrupt 
*	service routines, for example, as a way to resume task execution due to external 
*	event. Semaphores can be also released by a different task than the ones which took
*	it. The semaphore waiting queue is arranged in descending priority order.
*
*	BLOCKING SEMAPHORE ACQUISITION IS NOT ALLOWED INSIDE INTERRUPT HANDLERS! 
*	SVC calls made by the user will generate HardFault when these calls
*	occur inside interrupt handlers, which already run in Handler processor mode.
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
	
	// Initialise the semaphore parameters according to the input arguments
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



/*-------------------------------------------------------------------------------
* Function:    	sem_delete
* Purpose:    	Delete the semaphore given
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
#endif



/*-------------------------------------------------------------------------------
* Function:    	sem_try_acquire
* Purpose:    	Attempt to decrement the semaphore. Don't wait if unsucessful.
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: 		
*		exit status. EXIT_FAILURE if semaphore can't be taken
--------------------------------------------------------------------------------*/
uint32_t sem_try_acquire(Semaphore* toAcquire) {
	
	uint32_t exitStatus;
	TEST_NULL_POINTER(toAcquire)
	
	// Test if semaphore can be acquired immediately. If so, decrement
	// its counter value. Otherwise carry on without waiting. Notify the user
	// about the final outcome of the operation by returning the exit status
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
* Purpose:    	Decrement the semaphore. Wait if unsuccessful.
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: -
--------------------------------------------------------------------------------*/
uint32_t sem_acquire(Semaphore* toAcquire) {

	// Validate the semaphore pointer
	TEST_NULL_POINTER(toAcquire)
	__start_critical();
	{		
		// Try non-blicking acquisition of the semaphore. If it fails force the 
		// calling task to wait on this semaphore
		if (sem_try_acquire(toAcquire) == EXIT_FAILURE) {
			
			// Link the semaphore with the calling task
			scheduler.runPtr->waitingObj = toAcquire;
			
			// Remove the calling task from the ready queue and re-run the 
			// scheduler as the state of the ready queue has changed
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
* Purpose:    	Increment the semaphore.
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
		// If there is at least one task waiting on the semaphore then make wake
		// it up without changing the semaphore value
		if (toRelease->waitingQueue != NULL) {
			nextToAcquire = toRelease->waitingQueue;
			task_remove(&toRelease->waitingQueue, nextToAcquire);
			nextToAcquire->waitingObj = NULL;
			task_add(&scheduler.ready, nextToAcquire);
			scheduler_run();
		}
		// Otherwise increment the semaphore counter
		else {
			toRelease->counter++;
		}
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_release_ISR
* Purpose:    	Release the semaphore specified inside an interrupt service routine
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
* Purpose:    	Take the semaphore inside an interrupt service routine. Don't wait if
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
