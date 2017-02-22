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
#include "light_sensor.h"



/*-------------------------------------------------------------------------------
* Nokia 5110 LCD screen mutual exclusion lock
--------------------------------------------------------------------------------*/
Mutex nokiaMtx;



/*-------------------------------------------------------------------------------
* Task: 	nokiaLCDSetup
* Purpose: 	One-off LCD screen setup task run first and then removed from memory
*			(allocated on heap)
--------------------------------------------------------------------------------*/
void nokiaLCDSetup(void) {

	// Initialise the screen together with a mutex on it
	KrisOS_mutex_init(&nokiaMtx);
	nokia5110_init();
	
	// 'Say my name baby'
	nokia5110_set_cursor(25, 5);
	fprintf(&nokia5110, "KrisOS"); 	
}




KrisOS_task_define(thermometer, 200, 20)
/*-------------------------------------------------------------------------------
* Task: 	thermometer
* Purpose: 	Thermometer task that periodically reads the temperature from TC74
*			digital resistor and displays it on nokia LCD screen
--------------------------------------------------------------------------------*/
void thermometer(void) {
	
	// Current temperature
	int8_t temperature;
	
	// Initialise the I2C module for communicating with the temperature sensor
	i2c_init();
	i2c_slave_addr(0x48);
	
	while(1) {
		
		// Request the temperature reading
		i2c_write(0x00, START, STOP);
		temperature = i2c_read(START, STOP);
		
		// Display the current temperature
		KrisOS_mutex_lock(&nokiaMtx);
		nokia5110_set_cursor(0, 0);
		fprintf(&nokia5110, "Temp:  %dC", temperature);
		KrisOS_mutex_unlock(&nokiaMtx);
		
		// Wait for some time
		KrisOS_task_sleep(500);
	}
}




KrisOS_task_define(lightSensor, 200, 2)
/*-------------------------------------------------------------------------------
* Task: 	lightSensor
* Purpose: 	The illumination level monitor task
--------------------------------------------------------------------------------*/
void lightSensor(void) {
	
	// Initialise the semaphore for interrupt-task synchronisation
	KrisOS_sem_init(&lightSensorSem, 0);
	
	// Initialise the ADC module with digital comparator interrupts when the input
	// value exceed given threshold
	light_sensor_init(3000);
	
	// Initialise the buzzer
	buzzer_init();

	while(1) {
		
		// Wait on the semaphore - wait untill ADC0SS3 IRQ is generated which 
		// releases this semaphore
		KrisOS_sem_acquire(&lightSensorSem);
		
		// Output the warning message
		KrisOS_mutex_lock(&nokiaMtx);
		nokia5110_set_cursor(0, 1);
		fprintf(&nokia5110, "TOO MUCH LIGHT");
		KrisOS_mutex_unlock(&nokiaMtx);
		
		// Generate the 'beep'
		PIEZO = 0x2;
		KrisOS_task_sleep(200);
		PIEZO = 0;
		KrisOS_task_sleep(800);
		
		// Clear the warning message
		KrisOS_mutex_lock(&nokiaMtx);
		nokia5110_set_cursor(0, 1);
		fprintf(&nokia5110, "              ");
		KrisOS_mutex_unlock(&nokiaMtx);
	}
}



/*******************************************************************************
* SETUP
*******************************************************************************/
int main(void) {	

	// Initialise the operating system
	KrisOS_init();
	
	// Create the LCD screen setup task to be run first (high priority)
	KrisOS_task_create(nokiaLCDSetup, 200, 1);
	
	// Create the thermometer task
	KrisOS_stack_usage_config((void*) &thermometerStack[0], thermometerStackSize);
	KrisOS_task_create_static(&thermometerTask, thermometer, 
		&thermometerStack[thermometerStackSize], thermometerPriority); 
	
	// Create light monitor task
	KrisOS_stack_usage_config((void*) &lightSensorStack[0], lightSensorStackSize);
	KrisOS_task_create_static(&lightSensorTask, lightSensor, 
		&lightSensorStack[lightSensorStackSize], lightSensorPriority); 

	// Run the operating system
	KrisOS_start();
	while(1);
}
