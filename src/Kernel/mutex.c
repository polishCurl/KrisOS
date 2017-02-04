/*******************************************************************************
* File:     	mutex.c
* Brief:    	Mutual exclusion lock (mutex) implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/12/2016
* Last mod: 	21/12/2016
*
* Note: 		
*******************************************************************************/
#include "kernel.h"
#include "system.h"

#ifdef USE_MUTEX



/*-------------------------------------------------------------------------------
* Function:    	mutex_init
* Purpose:    	Initialise the mutex given
* Arguments:	
* 		toInit - mutex to initialise
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t mutex_init(Mutex* toInit) {
	
	// Set the initial values of member variables of the mutex to initialise
	TEST_NULL_POINTER(toInit)
	toInit->owner = toInit->waitingQueue = NULL;
	toInit->next = NULL;
	
	// Update the total number of mutexes declared
	#ifdef SHOW_DIAGNOSTIC_DATA
		KrisOS.totalMutexNo++;
	#endif	
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	mutex_create
* Purpose:    	Create a mutex using dynamic memory
* Arguments:	-
* Returns: 		
*		Pointer to the mutex created
--------------------------------------------------------------------------------*/
Mutex* mutex_create(void) {
	
	// Allocate memory for a new mutex and initialise it
	Mutex* toCreate = malloc(sizeof(Mutex));
	mutex_init(toCreate);	
	return toCreate;
}



/*-------------------------------------------------------------------------------
* Function:    	mutex_try_lock
* Purpose:    	Attempts to lock the mutex specified. If mutex is already owned then
*				the calling function doesn't wait until it's released.
* Arguments:	
*		toLock - mutex to lock
* Returns: 		
*		exit status (if lock already acquired - EXIT_FAILURE)
--------------------------------------------------------------------------------*/
uint32_t mutex_try_lock(Mutex* toLock) {
	
	uint32_t exitStatus;
	TEST_NULL_POINTER(toLock)
	
	__start_critical();
	{
		// The lock specified is ready to be taken. The calling tasks becomes the owner
		// immediately so both the lock is attached to the task and vice-versa
		if (toLock->owner == NULL) {
			toLock->owner = scheduler.runPtr;
			toLock->next = scheduler.runPtr->mutexHeld;
			scheduler.runPtr->mutexHeld = toLock;
			exitStatus = EXIT_SUCCESS;
			
			// Record the time the mutex has been taken
			#ifdef SHOW_DIAGNOSTIC_DATA
				toLock->timeTaken = KrisOS.ticks;
			#endif
		}
		else {
			exitStatus = toLock->owner == scheduler.runPtr ? EXIT_SUCCESS : EXIT_FAILURE;
		}
	}
	__end_critical();
	return exitStatus;
}



/*-------------------------------------------------------------------------------
* Function:    	mutex_lock
* Purpose:    	Take the mutex given. If already taken, place the calling task in
* 				the waiting queue
* Arguments:	
*		toLock - mutex to lock
* Returns: 		
*		exit status (if lock already acquired - EXIT_FAILURE)
--------------------------------------------------------------------------------*/
uint32_t mutex_lock(Mutex* toLock) {
	
	// Exit code on return from the function
	uint32_t exitStatus;
	
	// Iterator through the chain of locks (tasks waiting on locks owned by other tasks,
	// which in turn, wait for other locks
	Task* iterator;
	
	// Mutex for which the current task is waiting
	Mutex* mutexWaiting;
	TEST_NULL_POINTER(toLock)
	
	__start_critical();
	{
		// If the lock can't be obtained immediately. The enqueue it while applying the priority
		// inheritance algorithm
		if (mutex_try_lock(toLock) == EXIT_FAILURE) {
			
			// Iterate until the last task in the chain of locks is found, which needs
			// to have its priority temporarily boosted in order to avoid priority inversion.
			iterator = toLock->owner;
			while (iterator->priority > scheduler.runPtr->priority) {
				iterator->priority = scheduler.runPtr->priority;
				
				switch(iterator->status) {
					
					// The task owning mutex given is waiting for its turn in the 
					// ready queue so re-insert it into ready queue with priority increased
					case READY: 	
						task_remove(&scheduler.ready, iterator);
						task_add(&scheduler.ready, iterator);
						break;
					
					// The task owning mutex is waiting for another mutex
					case MTX_WAIT:
						mutexWaiting = iterator->waitingObj;
						task_remove(&mutexWaiting->waitingQueue, iterator);
						task_add(&mutexWaiting->waitingQueue, iterator);
						iterator = mutexWaiting->owner;
						break;
					
					// The task owning mutex is waiting for another mutex
					case SEM_WAIT:
						mutexWaiting = iterator->waitingObj;
						task_remove(&mutexWaiting->waitingQueue, iterator);
						task_add(&mutexWaiting->waitingQueue, iterator);
						break;
					default: break;
				}
			}
			// Insert the task into the waiting list for the mutex in descending priority
			// order
			task_remove(&scheduler.ready, scheduler.runPtr);
			scheduler_run();
			task_add(&toLock->waitingQueue, scheduler.runPtr);
			scheduler.runPtr->waitingObj = toLock;
			scheduler.runPtr->status = MTX_WAIT;
			exitStatus = EXIT_SUCCESS;	
		}
	}
	__end_critical();
	return exitStatus;
}



/*-------------------------------------------------------------------------------
* Function:    	mutex_unlock
* Purpose:    	Release the mutex specified
* Arguments:	
*		toUnlock - mutex to unlock
* Returns: 		
*		exit status (if lock already acquired - EXIT_FAILURE)
--------------------------------------------------------------------------------*/
uint32_t mutex_unlock(Mutex* toUnlock) {
	
	TEST_NULL_POINTER(toUnlock)
	
	// Test if the calling task actually owns the mutex and this is the most recent mutex
	// it has acquired
	if (toUnlock->owner != scheduler.runPtr)
		return EXIT_FAILURE;
	
	__start_critical();
	{
		// Remove the lock from the list of owned locks
		scheduler.runPtr->mutexHeld = scheduler.runPtr->mutexHeld->next;
		
		// Check if the time elapsed from the moment the mutex was taken until it
		// was given has exceeded the current maximum critical section length
		#ifdef SHOW_DIAGNOSTIC_DATA
			if (KrisOS.ticks - toUnlock->timeTaken > KrisOS.maxMtxCriticalSection)
				KrisOS.maxMtxCriticalSection = KrisOS.ticks - toUnlock->timeTaken;
		#endif		
		
		// If the running task had it's priority boosted (priority inheritance) then 
		// re-insert it into the ready queue with its original priority
		if (scheduler.runPtr->priority != scheduler.runPtr->basePrio) {
			task_remove(&scheduler.ready, scheduler.runPtr);
			scheduler.runPtr->priority = scheduler.runPtr->basePrio;
			task_add(&scheduler.ready, scheduler.runPtr);
			scheduler_run();
		}
		
		// If there are tasks waiting on this lock, wake the top priority task (first one
		// because of descending priority order)
		if (toUnlock->waitingQueue != NULL) {
			toUnlock->owner = toUnlock->waitingQueue;
			toUnlock->waitingQueue = toUnlock->waitingQueue->next;
			toUnlock->owner->status = READY;
			toUnlock->owner->mutexHeld = toUnlock;
			task_add(&scheduler.ready, toUnlock->owner);
			
			// Record the time the mutex has been taken
			#ifdef SHOW_DIAGNOSTIC_DATA
				toUnlock->timeTaken = KrisOS.ticks;
			#endif			
			scheduler_run();
		}
		else {
			toUnlock->owner = NULL;
		}
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	mutex_delete
* Purpose:    	Delete the mutex specified (if created using dynamic memory)
* Arguments:	
*		toDelete - mutex to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t mutex_delete(Mutex* toDelete) {
	
	TEST_NULL_POINTER(toDelete)
	
	__start_critical();
	{	
		// Update the total number of mutexes declared		
		#ifdef SHOW_DIAGNOSTIC_DATA
			KrisOS.totalMutexNo--;	
		#endif				
		
		// Only remove mutexes that are not used and don't have tasks waiting on them
		if (toDelete->owner != NULL || toDelete->waitingQueue != NULL) {
			__end_critical();
			return EXIT_FAILURE;
		}
		
		toDelete->next = NULL;
		toDelete->owner = toDelete->waitingQueue = NULL;
		free(toDelete);
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	mutex_release_all
* Purpose:    	Release all the locks that the given task has.
* Arguments:	
*		muxOwner - task owning muxes which we want to unlock
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t mutex_release_all(Task* muxOwner) {
	
	TEST_NULL_POINTER(muxOwner)
	
	__start_critical();
	{
		while (muxOwner->mutexHeld != NULL) 
			mutex_unlock(muxOwner->mutexHeld);
	}
	__end_critical();
	
	return EXIT_SUCCESS;
}


#endif
