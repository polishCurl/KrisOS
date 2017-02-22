/*******************************************************************************
* File:     	light_sensor.h
* Brief:    	Header file for light_sensor.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/02/2017
* Last mod: 	21/02/2017
*
* Note: 
* 	
*******************************************************************************/
#include "KrisOS.h"



/*-----------------------------------------------------------------------------
* Piezo buzzer address
------------------------------------------------------------------------------*/
#define PIEZO (*((volatile uint32_t*) 0x40005008))



/*-------------------------------------------------------------------------------
* Semaphore for synchronisation between the digital comparator interrupt handler
* and the task for reacting to
--------------------------------------------------------------------------------*/
extern Semaphore lightSensorSem;



/*-------------------------------------------------------------------------------
* Function:    	light_sensor_init
* Purpose:    	Initialise illumination level monitor. Set the ADC to conitnuously 
*				monitor the voltage level from the photoresistor and trigger an 
*				interrupt if the illumionation level is too high.
* Arguments:	
*		threshold - maximum illumination level threshold
* Returns: 		-	
--------------------------------------------------------------------------------*/
void light_sensor_init(uint32_t threshold);



/*-------------------------------------------------------------------------------
* Function:    	buzzer_init
* Purpose:    	Initialise the piezo buzzer at GPIO PB1 for generating beeps 
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void buzzer_init(void);



