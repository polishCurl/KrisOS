/*******************************************************************************
* File:     	demo2.c
* Brief:    	Second KrisOS demo program
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/02/2017
* Last mod: 	10/02/2017
*
* Note: 		
* 	Demo application showing mutual exclusion lock on the UART module in
* 	operation. KrisOS performance statistics task is run periodically too. All user
* 	tasks are dynamically allocated on heap and spawned from an initiator tasks,
* 	which deletes itself upon completion.
*******************************************************************************/
#include "KrisOS.h"
#include "led_pulse.h"



/*-----------------------------------------------------------------------------
* Declare the user tasks
------------------------------------------------------------------------------*/
KrisOS_task_dynamic_template(primes, 400, 61)
KrisOS_task_dynamic_template(secondTimer, 256, 27)
KrisOS_task_dynamic_template(initiator, 256, 1)
KrisOS_task_dynamic_template(ledPWM, 256, 41)



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
	KrisOS_mutex_lock(&uartMtx);
	fprintf(&uart, "\nPrime numbers between %d and %d are: \n", low, high);
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
			fprintf(&uart, "%d ", low);
			KrisOS_mutex_unlock(&uartMtx);
		}
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
		KrisOS_task_sleep(10000);
		KrisOS_mutex_lock(&uartMtx);
		fprintf(&uart, "\n\t\t\t<%d seconds elapsed>\n", ++secondsElapsed);
		KrisOS_mutex_unlock(&uartMtx);
	}
}




/*******************************************************************************
* Task: 	initiator
* Purpose: 	Creates other tasks and registers them at the scheduler. Run first.
*******************************************************************************/
void initiator(void) {
	
	// Create the prime numbers task (using heap)
	primesTaskPtr = KrisOS_task_create(primes, primesStackSize, primesPriority);
	
	// Create the second counter task (using heap)
	secondTimerTaskPtr = KrisOS_task_create(secondTimer, secondTimerStackSize, 
	                                        secondTimerPriority);
	
	// Create the RGB PWM LED task
	ledPWMTaskPtr = KrisOS_task_create(ledPWM, ledPWMStackSize, ledPWMPriority);
}



/*******************************************************************************
* SETUP
*******************************************************************************/
int main(void) {	
	
	// Initialise the operating system
	KrisOS_init();
	
	// Create the initiator task spawning the remaining tasks
	initiatorTaskPtr = KrisOS_task_create(initiator, initiatorStackSize, 
										  initiatorPriority);
	
	// Run the operating system
	KrisOS_start();
	while(1);
}
