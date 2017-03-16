/*******************************************************************************
* File:     	mutex.c
* Brief:    	Mutual exclusion lock (mutex) implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/12/2016
* Last mod: 	16/03/2017
*
* Note: 		
*	Mutual exclusion locks are used for preventing simultanous access of more than
*	a single task to a shared resource. So, only one task can enter a critical section
*	protected by a mutex (lock the mutex) and it has to be the same task which
*	later unlocks it. This is a big difference between mutexes, which have owners,
*	and binary semaphores.
*
*	This implies that MUTEXES ARE NOT ALLOWED TO BE USED INSIDE INTERRUPT HANDLERS! 
*	However, from user perspective, an SVC call has to be made to lock or unlock a 
*	mutex, so if the SVC call is made inside an ISR, then a HardFault exception will 
*	be generated. This is an implicit protection against using mutexes inside ISRs.
*
*	Another crucial property of the KrisOS mutexes is the implementation of priority
*	inheritance mechanism which stops the situations in which a low-priority
*	task owning a mutexes is delaying a high-priority task waiting to lock the 
* 	same mutex. This is the so-called 'priority inversion' problem.
*
*	Recursive mutex locking is not supported but if a task requesting a lock, already
*	owns the mutex specified then, no error condition is trigerred.
*	The mutex waiting queue is arranged in descending priority order. Deadlocks
*	are avoided by not allowing the same task to own two or more different mutexes.
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
	
	// Validate the input argument
	TEST_NULL_POINTER(toInit)
	
	// Set the initial values of member variables of the mutex to initialise
	toInit->owner = toInit->waitingQueue = NULL;
	
	// Update the total number of mutexes declared
	#ifdef SHOW_DIAGNOSTIC_DATA
		KrisOS.totalMutexNo++;
	#endif	
	return EXIT_SUCCESS;
}



#ifdef USE_HEAP
/*-------------------------------------------------------------------------------
* Function:    	mutex_create
* Purpose:    	Create a mutex using dynamic memory allocation.
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
* Function:    	mutex_delete
* Purpose:    	Delete the mutex specified 
* Arguments:	
*		toDelete - mutex to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t mutex_delete(Mutex* toDelete) {
	
	// Check if the argument is valid
	TEST_NULL_POINTER(toDelete)
	
	__start_critical();
	{			
		// Only remove mutexes that are not taken by any task and don't have tasks waiting on them
		if (toDelete->owner != NULL || toDelete->waitingQueue != NULL) {
			__end_critical();
			return EXIT_FAILURE;
		}
		
		// Update the total number of mutexes declared		
		#ifdef SHOW_DIAGNOSTIC_DATA
			KrisOS.totalMutexNo--;	
		#endif		
		
		// Release the heap memory this mutex occupies (if allocated dynamically)
		#ifdef USE_HEAP
			free(toDelete);
		#endif
	}
	__end_critical();
	return EXIT_SUCCESS;
}
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
uint32_t mutex_try_lock(Mutex* toLock) {
	
	uint32_t exitStatus;
	
	// Validate the input argument
	TEST_NULL_POINTER(toLock)
	
	__start_critical();
	{
		// The mutex specified is free to be locked. Link the task and the mutex
		// together
		if (toLock->owner == NULL && scheduler.runPtr->mutexHeld == NULL) {
			toLock->owner = scheduler.runPtr;
			scheduler.runPtr->mutexHeld = toLock;
			exitStatus = EXIT_SUCCESS;
			
			// Record the time the mutex has been taken. Later, the mutex release
			// time is recorded to measure the amount of time the mutex has been
			// locked (mutex critical section length)
			#ifdef SHOW_DIAGNOSTIC_DATA
				toLock->timeTaken = KrisOS.ticks;
			#endif
		}
		// If the mutex is already locked, check if it's the calling task that 
		// actually owns it.
		else {
			exitStatus = toLock->owner == scheduler.runPtr ? EXIT_SUCCESS : EXIT_FAILURE;
		}
	}
	__end_critical();
	return exitStatus;
}



/*-------------------------------------------------------------------------------
* Function:    	mutex_lock
* Purpose:    	Take the mutex given. Wait if the mutex is already taken.
* Arguments:	
*		toLock - mutex to lock
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t mutex_lock(Mutex* toLock) {
	
	// Iterator through the chain of 'waiting-for' dependencies inside the priority
	// inheritance algorithm
	Task* iterator;
	
	// Objects for which the iterator task can be waiting, a semaphore or a mutex
	Mutex* mutexWaiting;
	#ifdef USE_SEMAPHORE
		Semaphore* semWaiting;
	#endif
	
	// Check if the argument is valid
	TEST_NULL_POINTER(toLock)

	__start_critical();
	{
		// If the lock can't be obtained immediately...
		if (mutex_try_lock(toLock) == EXIT_FAILURE) {
			
			// Priority inheritance algorithm:
			// Iterate until the last task in the chain of dependencies is found, which needs
			// to have its priority temporarily boosted in order to avoid priority inversion.
			iterator = toLock->owner;
			while (iterator->priority > scheduler.runPtr->priority) {
				iterator->priority = scheduler.runPtr->priority;
				
				switch(iterator->status) {
					
					// The task owning mutex given is waiting for its turn in the 
					// ready queue. So re-insert it into ready queue with priority increased
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
					
					// The task owning mutex is waiting on a semaphore
					#ifdef USE_SEMAPHORE
					case SEM_WAIT:
						semWaiting = iterator->waitingObj;
						task_remove(&semWaiting->waitingQueue, iterator);
						task_add(&semWaiting->waitingQueue, iterator);
						break;
					#endif
					
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
		}
	}
	__end_critical();
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	mutex_unlock
* Purpose:    	Unlock the mutex specified
* Arguments:	
*		toUnlock - mutex to unlock
* Returns: 		
*		exit status, EXIT_FAILURE if the calling task doesn't own the mutex specified 
--------------------------------------------------------------------------------*/
uint32_t mutex_unlock(Mutex* toUnlock) {
	
	// Test if the calling task actually owns the mutex 
	if (toUnlock == NULL || toUnlock->owner != scheduler.runPtr)
		return EXIT_FAILURE;
	
	__start_critical();
	{
		// Now the calling task no longer owns the mutex
		scheduler.runPtr->mutexHeld = NULL;
		
		// Check if the time elapsed from the moment the mutex was locked to the moment
		// it is released exceeds the current maximum mutex-protected critical 
		// section length. If so, update the maximum mutex lock-time statistic
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
		
		// If there are tasks waiting on this lock, wake the top priority task 
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


#endif
