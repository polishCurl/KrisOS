#include "common.h"

#ifndef DATA_STRUCT_H					// Include barrier
#define DATA_STRUCT_H

/*******************************************************************************
* TEMPLATES FOR DATA STRUCTURES USED
*******************************************************************************/
typedef struct Task Task;			// Task Control Block
typedef struct Mutex Mutex; 		// Mutex
typedef struct Semaphore Semaphore; // Semaphore


/*******************************************************************************
* Task definition
*******************************************************************************/
// Possible task states
typedef enum {
	RUNNING,
	READY,
	SLEEPING,
	MTX_WAIT,
	SEM_WAIT,
	REMOVED,
} TaskState;

// Memory allocation type
typedef enum {
	STATIC,
	DYNAMIC,
} MemoryAllocation;




// Task control block
typedef struct Task {
	uint32_t sp; 					// Stack pointer value
	Task* next; 					// Pointer to the next task in a queue
	Task* previous; 				// Pointer to the previous task in a queue 
	int32_t id; 					// Task unique identifier
	uint8_t priority; 				// Task priority
	TaskState status; 				// Current task status
	uint64_t waitCounter;			// Remaining time to sleep (in OS 'ticks')
	uint32_t* stackBase; 			// Pointer to the base of process' stack
	void* waitingObj;				// Synchronisation object the task is waiting for
	uint8_t basePrio; 				// Base priority of the task given
#ifdef USE_MUTEX
	Mutex* mutexHeld; 				// List of mutexes held
#endif
#ifdef SHOW_DIAGNOSTIC_DATA
	MemoryAllocation memoryType; 	// Task memory allocation (static or dynamic)
	uint32_t cpuUsage; 				// CPU usage counter
	uint32_t stackSize; 			// Stack memory size
#endif
} Task;



/*******************************************************************************
* Mutex definiton
*******************************************************************************/
#ifdef USE_MUTEX
typedef struct Mutex {
	Task* owner; 					// Task owning the mutex
	Task* waitingQueue; 			// Queue of tasks waiting for the mutex
#ifdef SHOW_DIAGNOSTIC_DATA 		// Last time mutex was taken (for critical
	uint64_t timeTaken; 			// section length calculation)
#endif
} Mutex;

#endif



/*******************************************************************************
* Semaphore definiton
*******************************************************************************/
#ifdef USE_SEMAPHORE
typedef struct Semaphore {
	uint32_t counter; 				// Semaphore counter value
	Task* waitingQueue; 			// Queue of tasks waiting for the semaphore
} Semaphore;
#endif



#endif


