/*******************************************************************************
* File:     	demo2.c
* Brief:    	Second KrisOS demo program
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/02/2017
* Last mod: 	09/03/2017
*
* Note: 		
* 	Demo application showing aperiodic tasks interacting with the environment
*	using a set of sensors and actuators. A full spectrum of KrisOS features 
*	is used except for the KrisOS usage stats task and the UART driver.
*******************************************************************************/
#include "KrisOS.h"



/*-----------------------------------------------------------------------------
* Declare the user tasks
------------------------------------------------------------------------------*/
KrisOS_task_static_template(thermometerWriter, 256, 20)
KrisOS_task_static_template(thermometerReader, 256, 53)
KrisOS_task_static_template(lightSensor, 400, 13)
KrisOS_task_dynamic_template(nokiaLCDSetup, 400, 1)
KrisOS_task_dynamic_template(nokiaLCDBacklight, 256, 5)
KrisOS_task_dynamic_template(ledPWM, 256, 7)



/*-----------------------------------------------------------------------------
* SETUP
------------------------------------------------------------------------------*/
int main(void) {	

	// Initialise the operating system
	KrisOS_init();
	
	// Create the LCD screen setup task to be run first (high priority)
	nokiaLCDSetupTaskPtr = KrisOS_task_create(nokiaLCDSetup, nokiaLCDSetupStackSize, 
										      nokiaLCDSetupPriority);
	
	// Create the thermometer temperature reading task
	KrisOS_task_create_static(&thermometerWriterTask, thermometerWriter, 
							  &thermometerWriterStack[thermometerWriterStackSize], 
	                          thermometerWriterPriority); 
	
	// Create the thermometer temperature averaging and displaying task
	KrisOS_task_create_static(&thermometerReaderTask, thermometerReader, 
							  &thermometerReaderStack[thermometerReaderStackSize], 
	                          thermometerReaderPriority); 
	
	// Create the light monitor task
	KrisOS_task_create_static(&lightSensorTask, lightSensor, 
		                      &lightSensorStack[lightSensorStackSize], 
	                          lightSensorPriority); 

	// Create the LCD screen backlight LED controlling task
	nokiaLCDBacklightTaskPtr = KrisOS_task_create(nokiaLCDBacklight, 
												  nokiaLCDBacklightStackSize, 
												  nokiaLCDBacklightPriority);	  
												  
	// Create the RGB LED colour transition task
	ledPWMTaskPtr = KrisOS_task_create(ledPWM, ledPWMStackSize, ledPWMPriority);
	
	// Run the operating system
	KrisOS_start();
	
	while(1);
}
