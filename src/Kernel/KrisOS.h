/*******************************************************************************
* File:     	KrisOS.h
* Brief:    	User interface to the KrisOS embedded operating system
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	29/12/2016
*
* Note: 		User interface to the KrisOS and KrisOS setup parameters
*******************************************************************************/
#include "common.h"
#include "tm4c123gh6pm.h"
#ifndef KRIS_OS_H					// Include barrier
#define KRIS_OS_H



/*******************************************************************************
* OS features enable/disable (comment out to disable features)
*******************************************************************************/
#define USE_MUTEX 					// Use mutexes
#define USE_SEMAPHORE 				// Use semaphores
#define USE_HEAP 					// Use dynamic memory
#define USE_UART 					// Enable UART driver
#define SHOW_DIAGNOSTIC_DATA		// Show OS usage statistics etc.



/*******************************************************************************
* TEMPLATES FOR DATA STRUCTURES USED
*******************************************************************************/
typedef struct Task Task;			// Task Control Block
typedef struct Mutex Mutex; 		// Mutex
typedef struct Semaphore Semaphore; // Semaphore
typedef struct __FILE __FILE;		// File definition (for redirecting output stream)



/*******************************************************************************
* CONFIGURATION
*******************************************************************************/

/*-----------------------------------------------------------------------------
* System timing setup
------------------------------------------------------------------------------*/
// OS clock frequency (in Hz)
#define OS_CLOCK_FREQ 10000

// Definition of infinity (for pernamently suspending tasks)
#define TIME_INFINITY 0

// System clock frequency
extern uint32_t SYSTEM_CLOCK_FREQ;


/*-----------------------------------------------------------------------------
* Scheduler setup
------------------------------------------------------------------------------*/
// Time quantum size for preemptive scheduling (in OS clock 'ticks')
#define TIME_SLICE 500

// Size of the task registry for keeping track of all the tasks (without linked-list)
#define TASK_REGISTRY_SIZE 20


/*-----------------------------------------------------------------------------
* Heap Manager setup
------------------------------------------------------------------------------*/
// Heap size (in bytes)
#define HEAP_SIZE 1000		

// Minimum heap free block size that can still be divided into smaller ones
#define MIN_BLOCK_SIZE (4 * sizeof(HeapBlock))


/*-----------------------------------------------------------------------------
* Serial Monitor setup 
------------------------------------------------------------------------------*/
// UART0 baud rate
#define UART_BAUD_RATE 115200

/* UART0 word length
	0 - 5bits
	1 - 6bits
	2 - 7bits
	3 - 8bits	*/
#define UART_WORD_LEN 3	

/* UART0 parity checking
 	0 - no parity checking
 	1 - parity bit added (even or odd) 	*/
#define UART_D0_PARITY_CHECK 0	

/* UART0 parity
 	0 - odd
 	1 - even	*/
#define UART_PARITY 0	

/* UART0 number of stop bits
 	0 - one stop bit
 	1 - two stop bits	*/
#define UART_STOP_BITS 0	

// For redirecting output stream in stdio library to UART0
extern __FILE uart;

// Mutual exclusion lock on UART
#ifdef USE_UART 
#ifdef USE_MUTEX
extern Mutex uartMtx;
#endif
#endif


/*-----------------------------------------------------------------------------
* Usage statistics task setup
------------------------------------------------------------------------------*/
// Diagnostic data refresh rate (in OS clock 'ticks'). How frequently the KrisOS 
// usage data task is run.
#define DIAG_DATA_RATE 50000

// Statistics task'S priority
#define DIAG_DATA_PRIO (UINT8_MAX - 1)



/*******************************************************************************
* KrisOS data structures
*******************************************************************************/

/*-----------------------------------------------------------------------------
* Task
------------------------------------------------------------------------------*/
// Task states
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


/*-----------------------------------------------------------------------------
* Mutex definiton
------------------------------------------------------------------------------*/
#ifdef USE_MUTEX
typedef struct Mutex {
	Task* owner; 					// Task owning the mutex
	Task* waitingQueue; 			// Queue of tasks waiting for the mutex
#ifdef SHOW_DIAGNOSTIC_DATA 		// Last time mutex was taken (for critical
	uint64_t timeTaken; 			// section length calculation)
#endif
} Mutex;

#endif


/*-----------------------------------------------------------------------------
* Semaphore
------------------------------------------------------------------------------*/
#ifdef USE_SEMAPHORE
typedef struct Semaphore {
	uint32_t counter; 				// Semaphore counter value
	Task* waitingQueue; 			// Queue of tasks waiting for the semaphore
} Semaphore;
#endif


/*-----------------------------------------------------------------------------
* File (input/output stream)
------------------------------------------------------------------------------*/
typedef struct __FILE { 
	int handle;
} __FILE;



/*******************************************************************************
* SVC NUMBERS
*******************************************************************************/
#define SVC_OS_INIT 0				// Initialise the operating system
#define SVC_OS_START 1 				// Start the operating system
#define SVC_IRQ_EN 2 				// Enable an interrupt source
#define SVC_IRQ_DIS 3				// Disable an interrupt source
#define SVC_IRQ_SET_PEND 4 			// Set an interrupt pending
#define SVC_IRQ_CLEAR_PEND 5 		// Clear a pending interrupt
#define SVC_IRQ_READ_ACTIVE 6 		// Check if interrupt specified is active
#define SVC_IRQ_SET_PRIO 7 			// Set a priority for an interrupt
#define SVC_IRQ_GET_PRIO 8 			// Set a priority for an interrupt
#define SVC_TASK_NEW 9 				// Create a task using heap
#define SVC_TASK_NEW_S 10 			// Create a task using static memory
#define SVC_TASK_SLEEP 11 			// Delay given task by a multiple of OS 'ticks'
#define SVC_TASK_YIELD 12 			// Yield the currently running task
#define SVC_TASK_DELETE 13 			// Delete the currently running task
#define SVC_HEAP_ALLOC 14 			// Request dynamic memory allocation
#define SVC_HEAP_FREE 15 			// Free a dynamically allocated memory
#define SVC_MTX_INIT 16 			// Initialise a mutex
#define SVC_MTX_CREATE 17			// Create a mutex using heap
#define SVC_MTX_TRY_LOCK 18			// Attempt to aquire a mutex (doesn't wait)
#define SVC_MTX_LOCK 19				// Take a mutex, if unsuccessful, wait
#define SVC_MTX_UNLOCK 20 			// Release a mutex
#define SVC_MTX_DELETE 21 			// Pernamently remove a mutex
#define SVC_SEM_INIT 22				// Initialise a semaphore
#define SVC_SEM_CREATE 23 			// Create a semaphore
#define SVC_SEM_DELETE 24 			// Delete a semaphore
#define SVC_SEM_TRY_ACQUIRE 25 		// Take a semaphore (don't wait if not available)
#define SVC_SEM_ACQUIRE 26 			// Acquire a semaphore
#define SVC_SEM_ACQUIRE_TIME 27		// Acquire a semaphore (with timout)
#define SVC_SEM_RELEASE 28 			// Release a semaphore



/*******************************************************************************
* KRISOS INTERFACE
*******************************************************************************/
/*-------------------------------------------------------------------------------
* Function:    	KrisOS_init
* Purpose:    	Initialise the operating system
* Arguments:	-
* Returns: 
* 		exit status		
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_OS_INIT) KrisOS_init(void);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_start
* Purpose:    	Start the operating system
* Arguments:	-
* Returns: 
* 		exit status		
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_OS_START) KrisOS_start(void);




/*-------------------------------------------------------------------------------
* Function:    	KrisOS_irq_enable
* Purpose:    	Enable interrupt source
* Arguments: 	
*		irq - interrupt source to enable
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_IRQ_EN) KrisOS_irq_enable(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_irq_disable
* Purpose:    	Disable interrupt source
* Arguments: 	
*		irq - interrupt source to disable
* Returns: 
* 		exit status	
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_IRQ_DIS) KrisOS_irq_disable(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_irq_set_pending
* Purpose:    	Set pending Interrupt
* Arguments: 	
*		irq - interrupt source set pending
* Returns: 
* 		exit status		
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_IRQ_SET_PEND) KrisOS_irq_set_pending(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_irq_clear_pending
* Purpose:    	Clear Pending Interrupt
* Arguments: 	
*		irq - interrupt source clear
* Returns: 
* 		exit status	
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_IRQ_CLEAR_PEND) KrisOS_irq_clear_pending(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_irq_read_active
* Purpose:    	Check if given Interrupt is currently active
* Arguments: 	
*		irq - interrupt to have its status checked
* Returns: 		
*		0 if not active, 1 otherwise. UINT32_MAX if invalid interrupt number.	
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_IRQ_READ_ACTIVE) KrisOS_irq_read_active(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_irq_set_prio
* Purpose:    	Set the priority of given interrupt
* Arguments: 	
*		irq - interrupt number to set the priority of
* 		priority - priority to set, from 0 (highest) to 7 (lowest)
* Returns: 
* 		exit status		
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_IRQ_SET_PRIO) KrisOS_irq_set_prio(IRQn_Type irq, uint32_t priority);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_irq_get_prio
* Purpose:    	Get the priority of given interrupt
* Arguments: 	
*		irq - interrupt number to get the priority of
* Returns: 		
*		priority of given IRQ (0 - highest, 7 - lowest)
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_IRQ_GET_PRIO)  KrisOS_irq_get_prio(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_task_create
* Purpose:    	Create a task using heap and add it to the ready queue
* Arguments:	
*		startAddr - starting address of the task to add
*		stackSize - stack size (in bytes) required by the task
*		priority - the higher the number the lower the priority
* Returns: 		
*		pointer to the task created
--------------------------------------------------------------------------------*/
#ifdef USE_HEAP
Task* __svc(SVC_TASK_NEW) KrisOS_task_create(void* startAddr, size_t stackSize, 
												uint32_t priority);
#endif



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_task_create_static
* Purpose:    	Create a task using statically allocated memory
* Arguments:	
* 		toDeclare - pointer to the pre-allocated task control block of the task to declare
*		startAddr - starting address of the task to add
*		stackBase - pointer to the private static stack area to be used by the task
*		priority - task priority
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_TASK_NEW_S) KrisOS_task_create_static(Task* toCreate, void* startAddr,
											             void* stackBase, int32_t priority);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_task_sleep
* Purpose:    	Suspend the execution of the running task for specified amount of
* 				OS ticks.
* Arguments: 	
*		ticks - number of OS 'ticks' do suspend execution of the task by
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_TASK_SLEEP) KrisOS_task_sleep(uint32_t ticks);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_task_yield
* Purpose:    	Request a context switch to another task (cooperative scheduling)
* Arguments:	-
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_TASK_YIELD) KrisOS_task_yield(void);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_task_delete
* Purpose:    	Permanently remove the calling task.
* Arguments: 	-
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_TASK_DELETE) KrisOS_task_delete(void);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_malloc
* Purpose:    	Dynamically allocate bytesToAlloc bytes of memory
* Arguments:	
*		bytesToAlloc - number of bytes to allocate on heap
* Returns: 
* 		pointer to the memory block allocated or a NULL pointer if unsuccessful
--------------------------------------------------------------------------------*/
void* __svc(SVC_HEAP_ALLOC) KrisOS_malloc(size_t bytesToAlloc);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_free
* Purpose:    	Free the allocated block of memory
* Arguments:	
*		toFree - block of heap memory to free
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_HEAP_FREE) KrisOS_free(void* toFree);



#ifdef USE_MUTEX
/*-------------------------------------------------------------------------------
* Function:    	KrisOS_mutex_init
* Purpose:    	Initialise the mutex given
* Arguments:	
* 		toInit - mutex to initialise
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_MTX_INIT) KrisOS_mutex_init(Mutex* toInit);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_mutex_create
* Purpose:    	Create a mutex using dynamic memory
* Arguments:	-
* Returns: 		
*		Pointer to the mutex created
--------------------------------------------------------------------------------*/
Mutex* __svc(SVC_MTX_CREATE) KrisOS_mutex_create(void);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_mutex_try_lock
* Purpose:    	Attempts to lock the mutex specified. If mutex is already owned then
*				the calling function doesn't wait until it's released.
* Arguments:	
*		toLock - mutex to lock
* Returns: 		
*		exit status (if lock already acquired - EXIT_FAILURE)
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_MTX_TRY_LOCK) KrisOS_mutex_try_lock(Mutex* toLock);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_mutex_lock
* Purpose:    	Take the mutex given. If already taken, place the calling task in
* 				the waiting queue
* Arguments:	
*		toLock - mutex to lock
* Returns: 		
*		exit status 
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_MTX_LOCK) KrisOS_mutex_lock(Mutex* toLock);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_mutex_unlock
* Purpose:    	Release the mutex specified
* Arguments:	
*		toUnlock - mutex to unlock
* Returns: 		
*		exit status 
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_MTX_UNLOCK) KrisOS_mutex_unlock(Mutex* toUnlock);



/*-------------------------------------------------------------------------------
* Function:    	mutex_delete
* Purpose:    	Delete the mutex specified (if created using dynamic memory)
* Arguments:	
*		toDelete - mutex to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_MTX_DELETE) KrisOS_mutex_delete(Mutex* toDelete);
#endif



#ifdef USE_SEMAPHORE
/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_init
* Purpose:    	Initialise the semaphore given
* Arguments:	
* 		toInit - semaphore to initialise
* 		startVal - semaphore counter start value
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_SEM_INIT) KrisOS_sem_init(Semaphore* toInit, uint32_t startVal);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_create
* Purpose:    	Create a semaphore using heap and initialise it
* Arguments:	
* 		startVal - semaphore counter start value
* Returns: 		
*		pointer to the semaphore created
--------------------------------------------------------------------------------*/
Semaphore* __svc(SVC_SEM_CREATE) KrisOS_sem_create(uint32_t startVal);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_delete
* Purpose:    	Remove the semaphore given
* Arguments:	
* 		toDelete - semaphore to delete
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_SEM_DELETE) KrisOS_sem_delete(Semaphore* toDelete);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_try_acquire
* Purpose:    	Attempt to decrement the semaphore without waiting if unsucessful
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_SEM_TRY_ACQUIRE) KrisOS_sem_try_acquire(Semaphore* toAcquire);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_acquire
* Purpose:    	Attempt to decrement the semaphore. Wait if unsuccessful.
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_SEM_ACQUIRE) KrisOS_sem_acquire(Semaphore* toAcquire);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_acquire_timeout
* Purpose:    	Attempt to decrement the semaphore. Wait if unsuccessful for maximum
* 				of 'timout' milliseconds
* Arguments:	
* 		toAcquire - semaphore to acquire
* 		timout - semaphore timeout
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_SEM_ACQUIRE_TIME) KrisOS_sem_acquire_timeout(Semaphore* toAcquire,
																uint32_t timout);


/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_release
* Purpose:    	Release the semaphore specified
* Arguments:	
*		toRelease - mutex to unlock
* Returns: 		
*		exit status 
--------------------------------------------------------------------------------*/
uint32_t __svc(SVC_SEM_RELEASE) KrisOS_sem_release(Semaphore* toRelease);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_release_from_ISR
* Purpose:    	Release the semaphore specified by an interrupt service routine
* Arguments:	
*		toRelease - semaphore to release		
* Returns: 		
*		exit status 
--------------------------------------------------------------------------------*/
uint32_t KrisOS_sem_release_from_ISR(Semaphore* toRelease);



/*-------------------------------------------------------------------------------
* Function:    	KrisOS_sem_acquire_from_ISR
* Purpose:    	Take the semaphore by an interrupt service routine. Don't wait if
*				unsuccessful
* Arguments:	
* 		toAcquire - semaphore to acquire
* Returns: 		
*		exit status 
--------------------------------------------------------------------------------*/
uint32_t KrisOS_sem_acquire_from_ISR(Semaphore* toAcquire);

#endif



#ifdef SHOW_DIAGNOSTIC_DATA
/*-------------------------------------------------------------------------------
* Function:    	KrisOS_task_stack_usage
* Purpose:    	Reset the stack memory given in order to extract stack usage data later
* Arguments:	
*		toPrepare - top of the stack memory to reset
* 		size - size of stack memory
* Returns: 		
*		exit status
--------------------------------------------------------------------------------*/
uint32_t KrisOS_task_stack_usage(uint32_t* toPrepare, uint32_t size);
#endif



/*-------------------------------------------------------------------------------
* Macro:    	KrisOS_task_static_template
* Purpose:    	MACRO speeding up static task declaration by automatically declaring 
*				and initialising variables and constants to be passed to 
*				KrisOS_task_create_static.
* Arguments:	
*		NAME - unique name of the task and prefix to the task description variable names.
*			   1. task function name - void <NAME>(void)
*			   2. task struct - Task <NAME>Task
*			   3. task stack size - <NAME>StackSize = SIZE
*			   4. task stack memory - <NAME>Stack['task stack size']
* 			   5. task priority - <NAME>Priority
*		STACK_SIZE - task private stack memory size
* 		PRIORITY - task priority
--------------------------------------------------------------------------------*/
#define KrisOS_task_static_template(NAME, STACK_SIZE, PRIORITY) 			\
	void NAME ## (void);													\
	Task NAME ## Task;											  			\
	const size_t NAME ## StackSize = STACK_SIZE;							\
	uint8_t NAME ## Stack[NAME ## StackSize];								\
	const uint8_t NAME ## Priority = PRIORITY;
	
	
	
/*-------------------------------------------------------------------------------
* Macro:    	KrisOS_task_dynamic_template
* Purpose:    	MACRO speeding up dynamic task declaration by automatically declaring 
*				and initialising variables and constants to be passed to 
*				KrisOS_task_create.
* Arguments:	
*		NAME - unique name of the task and prefix to the task description variable names.
*			   1. task function name - void <NAME>(void)
*			   2. task struct pointer - Task* <NAME>Task
*			   3. task stack size - <NAME>StackSize = SIZE
* 			   5. task priority - <NAME>Priority
*		STACK_SIZE - task private stack memory size
* 		PRIORITY - task priority
--------------------------------------------------------------------------------*/
#define KrisOS_task_dynamic_template(NAME, STACK_SIZE, PRIORITY) 			\
	void NAME ## (void);													\
	Task* NAME ## TaskPtr;											  		\
	const size_t NAME ## StackSize = STACK_SIZE;							\
	const uint8_t NAME ## Priority = PRIORITY;	
	
	
#endif
