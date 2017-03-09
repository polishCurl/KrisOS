/*******************************************************************************
* File:     	light_sensor.h
* Brief:    	Header file for light_sensor.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/02/2017
* Last mod: 	09/03/2017
*
* Note: 
*******************************************************************************/
#include "KrisOS.h"



/*-----------------------------------------------------------------------------
* Piezo buzzer address
------------------------------------------------------------------------------*/
#define PIEZO (*((volatile uint32_t*) 0x40005008))



/*-----------------------------------------------------------------------------
* Light sensor threshold value
------------------------------------------------------------------------------*/
#define LIGHT_THRES 3500



/*-------------------------------------------------------------------------------
* Semaphore for synchronisation between the digital comparator interrupt handler
* and the task for reacting to excessive amount of light.
--------------------------------------------------------------------------------*/
extern Semaphore lightSensorSem;



/*-------------------------------------------------------------------------------
* Function:    	light_sensor_init
* Purpose:    	Initialise illumination level monitor. Set the ADC to conitnuously 
*				monitor the voltage level from the photoresistor and trigger an 
*				interrupt if the illumination level is too high.
* Arguments:	
*		threshold - maximum illumination level threshold
* Returns: 		-	
--------------------------------------------------------------------------------*/
void light_sensor_init(uint32_t threshold);



/*-------------------------------------------------------------------------------
* Function:    	buzzer_init
* Purpose:    	Initialise the piezo buzzer for generating sound alerts when the 
*				amount of light falling on the photoresistor exceeds the limit.
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void buzzer_init(void);



/*-------------------------------------------------------------------------------
* Function:    	buzzer_tone
* Purpose:    	Beep the buzzer. Emit a sound of given frequency.
* Arguments:	
*		frequency - output sound frequency in Hz
* Returns: 		-	
--------------------------------------------------------------------------------*/
void buzzer_tone(uint32_t frequency);



/*-------------------------------------------------------------------------------
* Function:    	buzzer_off
* Purpose:    	Switch off the buzzer. Finish the alert 'melody'
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void buzzer_off(void);


