/*******************************************************************************
* File:     	demo1.c
* Brief:    	First demo program
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/02/2017
* Last mod: 	10/02/2017
*
* Note: 		
* 	Demo application showing two statically allocated task competing for UART.
*	This simple use case shows the priority scheduler in action running two
* 	periodic tasks.
*******************************************************************************/
#include "KrisOS.h"



/*-----------------------------------------------------------------------------
* Declare the user tasks
------------------------------------------------------------------------------*/
KrisOS_task_static_template(primes, 400, 42)
KrisOS_task_static_template(secondTimer, 200, 23)



/*******************************************************************************
* Task: 	Primes
* Purpose: 	Compute and display all prime numbers between low and high
*******************************************************************************/
void primes(void) {
		
	// Range of numbers from which the primes should be picked.
	int32_t low, high;

	// Helper iterator and a flag raised when the currently considered
	// number is divisible by more numbers than '1'.
	int32_t i, flag;

	// Initialise the range and output header message.
	low = 2;
	high = 20000;
	fprintf(&uart, "\nPrime numbers between %d and %d are: \n", low, high);

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

		if (flag == 0) 
			fprintf(&uart, "%d ", low);
		
		++low;
	}		
}
	


/*******************************************************************************
* Task: 	secondTimer
* Purpose: 	Display the number of seconds elapsed since the start of this task
*******************************************************************************/
void secondTimer(void) {

	uint32_t secondsElapsed = 0;
	while(1) {
		KrisOS_task_sleep(100);
		fprintf(&uart, "\n\t\t\t<%d seconds elapsed>\n", ++secondsElapsed);
	}
}



/*-------------------------------------------------------------------------------
* SETUP
--------------------------------------------------------------------------------*/
int main(void) {	
	
	// Initialise the operating system
	KrisOS_init();
	
	// Initialise the task for which the memory has already been allocated
	KrisOS_task_create_static(&primesTask, primes, &primesStack[primesStackSize], 
		primesPriority); 
	
	KrisOS_task_create_static(&secondTimerTask, secondTimer, 
		&secondTimerStack[secondTimerStackSize], secondTimerPriority); 
	
	// Run the operating system
	KrisOS_start();
	while(1);
}


