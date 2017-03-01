/*******************************************************************************
* File:     	demo3.c
* Brief:    	Third KrisOS demo program
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/02/2017
* Last mod: 	25/02/2017
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"



/*-----------------------------------------------------------------------------
* Declare the user tasks
------------------------------------------------------------------------------*/
KrisOS_task_static_template(thermometer, 256, 20)
KrisOS_task_static_template(lightSensor, 400, 2)
KrisOS_task_dynamic_template(nokiaLCDSetup, 256, 1)
KrisOS_task_dynamic_template(nokiaLCDBacklight, 256, 32)



/*-----------------------------------------------------------------------------
* SETUP
------------------------------------------------------------------------------*/
int main(void) {	

	// Initialise the operating system
	KrisOS_init();
	
	// Create the LCD screen setup task to be run first (high priority)
	nokiaLCDSetupTaskPtr = KrisOS_task_create(nokiaLCDSetup, nokiaLCDSetupStackSize, 
										      nokiaLCDSetupPriority);
	
	// Create the thermometer task
	KrisOS_task_stack_usage((void*) &thermometerStack[0], thermometerStackSize);
	KrisOS_task_create_static(&thermometerTask, thermometer, 
							  &thermometerStack[thermometerStackSize], 
	                          thermometerPriority); 
	
	// Create light monitor task
	KrisOS_task_stack_usage((void*) &lightSensorStack[0], lightSensorStackSize);
	KrisOS_task_create_static(&lightSensorTask, lightSensor, 
		                      &lightSensorStack[lightSensorStackSize], 
	                          lightSensorPriority); 

	// Create the LCD screen setup task to be run first (high priority)
	nokiaLCDBacklightTaskPtr = KrisOS_task_create(nokiaLCDBacklight, 
												  nokiaLCDBacklightStackSize, 
												  nokiaLCDBacklightPriority);
	
	// Run the operating system
	KrisOS_start();
	
	while(1);
}
