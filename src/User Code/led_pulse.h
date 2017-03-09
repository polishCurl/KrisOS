/*******************************************************************************
* File:     	led_pulse.h
* Brief:    	Header file for led_pulse.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	09/03/2017
* Last mod: 	09/03/2017
*
* Note: 		
*******************************************************************************/
#include "KrisOS.h"



/*-----------------------------------------------------------------------------
* PWM counter reload value - the PWM frequency
------------------------------------------------------------------------------*/
#define RGB_LED_PWM_FREQ 100



/*-----------------------------------------------------------------------------
* RGB LED switching rate in OS 'ticks'
------------------------------------------------------------------------------*/
#define RGB_SWITCH_RATE 10



/*-----------------------------------------------------------------------------
* The three LED pins associated with PF1, PF2 and PF3 GPIO pins
------------------------------------------------------------------------------*/
typedef enum { 
	RED,
	GREEN,
	BLUE,
} RGB_LED;



/*-------------------------------------------------------------------------------
* Function:    	ledPWM_init
* Purpose:    	Initialise the PF1, PF2 and PF3 pins for PWM to control the RGB LED
*				colour.
* Arguments:
*		frequency - PWM LED frequency (same for all RGB pins), in Hz
* Returns: 
*		maximum duty cycle allowed given the frequency specified
--------------------------------------------------------------------------------*/
uint32_t ledPWM_init(uint32_t frequency);



/*-------------------------------------------------------------------------------
* Function:    	ledPWM_set_duty
* Purpose:    	Set the PWM duty cycle of the LED colour given.
* Arguments:
*		dutyCycle - duty cycle to set (in PWM clock ticks)
*		colour - LED pin to change its duty cycle (red/green/blue)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void ledPWM_set_duty(uint32_t dutyCycle, RGB_LED colour);
