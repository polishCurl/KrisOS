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
#include "i2c.h"



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



/*-------------------------------------------------------------------------------
* Task: 	i2cTest
* Purpose: 	Test the I2C interface to the TC74A0 serial digital thermal sensor
--------------------------------------------------------------------------------*/
KrisOS_task_define(i2cTest, 400, 20)


void i2cTest(void) {
	int8_t temperature;
	i2c_init();
	i2c_slave_addr(0x48);
	
	while(1) {
		temperature = 0xff;
		//i2c_write(0x01, START, CONTINUE); 
		//i2c_write(0x80, CONTINUED, STOP);
		i2c_write(0x00, START, STOP);
		temperature = i2c_read(START, STOP);
		fprintf(&uart, "%d\n", temperature);
		KrisOS_task_sleep(5000);
	}
}



/*******************************************************************************
* SETUP
*******************************************************************************/
int main(void) {	
	
	// Initialise the operating system
	KrisOS_init();
	
	// Create the LCD screen tester task
	KrisOS_stack_usage_config((void*) &nokiaLCDTestStack[0], nokiaLCDTestStackSize);
	KrisOS_task_create_static(&nokiaLCDTestTask, nokiaLCDTest, 
		&nokiaLCDTestStack[nokiaLCDTestStackSize], nokiaLCDTestPriority); 
	
	// Create the I2C tester task
	KrisOS_stack_usage_config((void*) &i2cTestStack[0], i2cTestStackSize);
	KrisOS_task_create_static(&i2cTestTask, i2cTest, 
		&i2cTestStack[i2cTestStackSize], i2cTestPriority); 

	// Run the operating system
	KrisOS_start();
	while(1);
}
