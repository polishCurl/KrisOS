/*******************************************************************************
* File:     	demo3.c
* Brief:    	Third KrisOS demo program
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/02/2017
* Last mod: 	10/02/2017
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"
#include "nokia5110.h"



/*-------------------------------------------------------------------------------
* Task: 	nokiaLCDTest
* Purpose: 	Test the nokia 5110 LCD screen driver
--------------------------------------------------------------------------------*/
KrisOS_task_define(nokiaLCDTest, 400, 1)


void nokiaLCDTest(void) {

	nokia5110_init();
	while(1) {
		fprintf(&nokia5110, "KrisOS"); 
		nokia5110_set_cursor(0, 4);
		fprintf(&nokia5110, "Embedded OS by\nKrzysztof Koch"); 
		KrisOS_task_sleep(5000);
		nokia5110_clear();
		fprintf(&nokia5110, "Time for the I2C protocol and temperature sensor reading");
		KrisOS_task_sleep(5000);
		nokia5110_clear();
	}
}




/*******************************************************************************
* SETUP
*******************************************************************************/
int main(void) {	
	
	// Initialise the operating system
	KrisOS_init();
	
	// Create the LCD screen tester task
	KrisOS_task_create_static(&nokiaLCDTestTask, nokiaLCDTest, 
		&nokiaLCDTestStack[nokiaLCDTestStackSize], nokiaLCDTestPriority); 

	// Run the operating system
	KrisOS_start();
	while(1);
}
