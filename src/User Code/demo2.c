/*******************************************************************************
* File:     	uart_mtx_stats_demo.c
* Brief:    	First demo program
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/02/2017
* Last mod: 	10/02/2017
*
* Note: 		
* 	Demo application showing mutual exclusion lock on the UART module in
* 	operation. OS performance statistics task is run periodically too. All user
* 	tasks are dynamically allocated on heap and spawned from an initiator tasks,
* 	which deletes itself upon completion.
*******************************************************************************/
#include "KrisOS.h"

Mutex testMutex;


/*-------------------------------------------------------------------------------
* Task: 	Primes
* Purpose: 	Compute and display all prime numbers between low and high
--------------------------------------------------------------------------------*/
Task* primesTask;

void primes(void) {

	// Range of numbers from which the primes should be picked.
	int32_t low, high;

	// Helper iterator and a flag raised when the currently considered
	// number is divisible by more numbers than '1'.
	int32_t i, flag;
		
	while(1) {
		
		// Initialise the range and output header message.
		low = 5000;
		high = 15000;
		
		// Print the starting message
		KrisOS_mutex_lock(&uartMtx);
		fprintf(uart, "\nPrime numbers between %d and %d are: \n", low, high);
		KrisOS_mutex_unlock(&uartMtx);
	
		// Display all the prime numbers between high and low, go to sleep for 
		// some time, then repeat.
		while (low < high)
		{	
			// Test if the given 'low' is a prime and print it if so.
			flag = 0;
			for(i = 2; i <= low/2; ++i)
				if(low % i == 0)
				{
					flag = 1;
					break;
				}

			if (flag == 0) {
				KrisOS_mutex_lock(&uartMtx);
				fprintf(uart, "%d ", low);
				KrisOS_mutex_unlock(&uartMtx);
			}
			
			++low;
		}
		KrisOS_task_sleep(15000);
	}		
}
	



/*-------------------------------------------------------------------------------
* Task: 	secondTimer
* Purpose: 	Display the number of seconds elapsed since the start of this task
--------------------------------------------------------------------------------*/
Task* secondTimerTask;

void secondTimer(void) {

	uint32_t secondsElapsed = 0;
	while(1) {
		KrisOS_task_sleep(1000);
		KrisOS_mutex_lock(&uartMtx);
		fprintf(uart, "\n\t\t\t<%d seconds elapsed>\n", ++secondsElapsed);
		KrisOS_mutex_unlock(&uartMtx);
	}
}




/*-------------------------------------------------------------------------------
* Task: 	Initiator
* Purpose: 	Dynamically creates other tasks and registers them at the scheduler.
* 			Then the task deletes itself.
--------------------------------------------------------------------------------*/
Task* initiatorTask;

void initiator(void) {
	primesTask = (Task*) KrisOS_task_create(primes, 400, 75);
	secondTimerTask = (Task*) KrisOS_task_create(secondTimer, 200, 13);
}




/*******************************************************************************
* SETUP
*******************************************************************************/
int main(void) {	
	
	// Initialise the operating system
	KrisOS_init();
	
	KrisOS_mutex_init(&testMutex);
	
	// Create (on heap) and register the spawning task
	initiatorTask = (Task*) KrisOS_task_create(initiator, 200, 0);
	
	// Run the operating system
	KrisOS_start();
	while(1);
}
