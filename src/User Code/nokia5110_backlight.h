/*******************************************************************************
* File:     	nokia5110_backlight.h
* Brief:    	Header file for nokia5110_backlight.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	25/02/2017
* Last mod: 	25/02/2017
*
* Note: 
*
*******************************************************************************/
#include "KrisOS.h"



/*-------------------------------------------------------------------------------
* Semaphore to be used between the GPIOF IRQ handler and nokiaLCDBacklight task
* for controling the operation of nokia 5110 backlight
--------------------------------------------------------------------------------*/
extern Semaphore* backlightSem;



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_backlight_switch_init
* Purpose:    	Initialise the switch button SW2 (PF0) for the LED backlight control
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_backlight_switch_init(void);



/*-------------------------------------------------------------------------------
* Function:    	nokia5110_backlight_control_pin_init
* Purpose:    	Initialise the GPIO pin (PB0) for controlling LCD screen backlight
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nokia5110_backlight_control_pin_init(void);
