/*******************************************************************************
* File:     	demo1.c
* Brief:    	First KrisOS demo program
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/02/2017
* Last mod: 	16/03/2017
*
* Note: 		
* 	Demo application showing mutual exclusion lock on the UART module in
* 	operation. KrisOS performance statistics task is run periodically too.
*	User task are spawned both using static memory allocation and heap memory.
*******************************************************************************/
#include "KrisOS.h"
#include "led_pulse.h"



/*-----------------------------------------------------------------------------
* Declare the user tasks
------------------------------------------------------------------------------*/
KrisOS_task_dynamic_template(primes, 400, 61)
KrisOS_task_static_template(welcomeMessage, 256, 27)
KrisOS_task_dynamic_template(ledPWM, 256, 41)



/*******************************************************************************
* Task: 	Primes
* Purpose: 	Compute and display all prime numbers between low and high
*******************************************************************************/
void primes(void) {
		
	// Range of numbers from which the primes should be picked.
	int32_t low, high;

	// Helper iterator and a flag raised when the currently considered
	// number is divisible by a different number to '1' (non-prime).
	int32_t i, flag;

	// Initialise the range and output the intro message.
	low = 2;
	high = 20000;
	
	// Lock the UART mutex if the OS feature is enabled
	#ifdef USE_MUTEX
		KrisOS_mutex_lock(&uartMtx);
	#endif
	
	fprintf(&uart, "\nPrime numbers between %d and %d are: \n", low, high);
	
	// Release the UART mutex
	#ifdef USE_MUTEX
		KrisOS_mutex_unlock(&uartMtx);
	#endif

	// Display all the prime numbers between high and low. Remove the task
	// when finished
	while (low < high)
	{	
		// Test if the given 'low' is a prime
		flag = 0;
		for(i = 2; i <= low/2; ++i)
			if(low % i == 0)
			{
				flag = 1;
				break;
			}

		// Display the prime number
		if (flag == 0) {
			
			// Lock the UART mutex if the OS feature is enabled
			#ifdef USE_MUTEX
				KrisOS_mutex_lock(&uartMtx);
			#endif
			
			fprintf(&uart, "%d ", low);
			
			// Release the UART mutex
			#ifdef USE_MUTEX
				KrisOS_mutex_unlock(&uartMtx);
			#endif
		}
		++low;
	}		
}
	


/*******************************************************************************
* Task: 	welcomeMessage
* Purpose: 	Displays a welcome message every second 
*******************************************************************************/
void welcomeMessage(void) {

	while(1) {
		KrisOS_task_sleep(9500);
		
		// Lock the UART mutex if the OS feature is enabled
		#ifdef USE_MUTEX
			KrisOS_mutex_lock(&uartMtx);
		#endif
		
		fprintf(&uart, "\n*******************************************************\n");
		fprintf(&uart, "KrisOS - an embedded operating system by Krzysztof Koch");
		fprintf(&uart, "\n*******************************************************\n");
		
		// Release the UART mutex
		#ifdef USE_MUTEX
			KrisOS_mutex_unlock(&uartMtx);
		#endif
	
	}
}



/*******************************************************************************
* SETUP
*******************************************************************************/
int main(void) {	
	
	// Initialise the operating system
	KrisOS_init();
	
	// Create the prime numbers task
	primesTaskPtr = KrisOS_task_create(primes, primesStackSize, primesPriority);
	
	// Initialise the welcome message task
	#ifdef SHOW_DIAGNOSTIC_DATA
		KrisOS_task_stack_usage((void*) welcomeMessageStack, welcomeMessageStackSize);
	#endif
	KrisOS_task_create_static(&welcomeMessageTask, welcomeMessage, 
		&welcomeMessageStack[welcomeMessageStackSize], welcomeMessagePriority); 
	
	// Create the RGB PWM LED task
	ledPWMTaskPtr = KrisOS_task_create(ledPWM, ledPWMStackSize, ledPWMPriority);
	
	// Run the operating system
	KrisOS_start();
	while(1);
}
