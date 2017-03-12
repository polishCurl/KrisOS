/*******************************************************************************
* File:     	led_pulse.c
* Brief:    	RGB LED PWM demo user program 
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	09/03/2017
* Last mod: 	09/03/2017
*
* Note: 		
*	A program which shows a gradual RGB LED colour change thanks to changing the 
*	duty cycle of the 3 LEDs in turns.
*
*	The Tiva C Launchpad on-board RGB LED is located at pins:
*		1. PF1 - red
*		2. PF2 - green
*		3. PF3 - blue
*******************************************************************************/
#include "KrisOS.h"
#include "led_pulse.h"



/*******************************************************************************
* Task: 	ledPWM
* Purpose: 	RGB LED controller task which displays gradual colout change
*******************************************************************************/
void ledPWM(void) {
	
	// Current duty cycle of the LED which is now fading-down or fading-up
	uint32_t dutyCycle;
	
	// Maximum duty cycle allowed for the PWM frequency specified 
	uint32_t maxDuty;
	
	// Initialise the PWM on the RGB LED
	maxDuty = ledPWM_init(RGB_LED_PWM_FREQ);
	
	// Make gentle transitions of the RGB colour by dimming/brightening
	// all three LEDs in turn
	for (dutyCycle = 0; dutyCycle <= maxDuty; dutyCycle++) {
		ledPWM_set_duty(dutyCycle, RED);
		KrisOS_task_sleep(RGB_SWITCH_RATE);
	}
	
	while (1) {
		
		for (dutyCycle = 0; dutyCycle <= maxDuty; dutyCycle++) {
			ledPWM_set_duty(dutyCycle, BLUE);
			KrisOS_task_sleep(RGB_SWITCH_RATE);
		}
		
		for (dutyCycle = maxDuty; dutyCycle > 0; dutyCycle--) {
			ledPWM_set_duty(dutyCycle, RED);
			KrisOS_task_sleep(RGB_SWITCH_RATE);
		}
		
		for (dutyCycle = 0; dutyCycle <= maxDuty; dutyCycle++) {
			ledPWM_set_duty(dutyCycle, GREEN);
			KrisOS_task_sleep(RGB_SWITCH_RATE);
		}
		
		for (dutyCycle = maxDuty; dutyCycle > 0; dutyCycle--) {
			ledPWM_set_duty(dutyCycle, BLUE);
			KrisOS_task_sleep(RGB_SWITCH_RATE);
		}
		
		for (dutyCycle = 0; dutyCycle <= maxDuty; dutyCycle++) {
			ledPWM_set_duty(dutyCycle, RED);
			KrisOS_task_sleep(RGB_SWITCH_RATE);
		}
		
		for (dutyCycle = maxDuty; dutyCycle > 0; dutyCycle--) {
			ledPWM_set_duty(dutyCycle, GREEN);
			KrisOS_task_sleep(RGB_SWITCH_RATE);
		}
	}
}



/*-------------------------------------------------------------------------------
* Function:    	ledPWM_init
* Purpose:    	Initialise the PF1, PF2 and PF3 pins for PWM to control the RGB LED
*				colour.
* Arguments:
*		frequency - PWM LED frequency (same for all RGB pins), in Hz
* Returns: 
*		maximum duty cycle allowed given the frequency specified
--------------------------------------------------------------------------------*/
uint32_t ledPWM_init(uint32_t frequency) {
	
	// Activate PWM Module 1 and enable clock on port F. Add a small delay.
	SYSCTL->RCGCPWM |= 1 <<	RCGC_PWM1;	
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTF); 
	while ((SYSCTL->RCGCGPIO & (1 << RCGCGPIO_PORTF)) == 0);
	
	// Unlock the GPIOF port and allow changes to PF1, PF2 and PF3
	GPIOF->LOCK = 0x4C4F434B;
	GPIOF->CR |= (1 << PIN1) | (1 << PIN2) | (1 << PIN3);
	
	// Configure PF1 - M1PWM5, PF2 - M1PWM6, PF3 - M1PWM7
	GPIOF->AFSEL |= (1 << PIN1) | (1 << PIN2) | (1 << PIN3);
	GPIOF->PCTL &= ~((0xF << PCTL_PMC1) | (0xF << PCTL_PMC2) | (0xF << PCTL_PMC3));
	GPIOF->PCTL |= (0x5 << PCTL_PMC1) | (0x5 << PCTL_PMC2) | (0x5 << PCTL_PMC3);
	GPIOF->AMSEL &= ~((0x5 << PCTL_PMC1) | (0x5 << PCTL_PMC2) | (0x5 << PCTL_PMC3));
	GPIOF->DEN |= (1 << PIN1) | (1 << PIN2) | (1 << PIN3);
	GPIOF->DIR |= (1 << PIN1) | (1 << PIN2) | (1 << PIN3);

	// Configure the PWM generator for immediate updates and 
	// countdown mode. High on comparator down and low on load. 
	// The same setting for all RGB LED pins.
	PWM1->_2_CTL = PWM1->_3_CTL = 0; 
	PWM1->_3_GENA = (0x2 << PWMGEN_ACTLOAD) | (0x3 << PWMGEN_ACTCMPAD);
	PWM1->_2_GENB = PWM1->_3_GENB = (0x2 << PWMGEN_ACTLOAD) | (0x3 << PWMGEN_ACTCMPBD);
			
	// Set the PWM counter reload value according to the input frequency and
	// set the duty cycle as a timer comparator value (reset to 0%)
	PWM1->_2_LOAD = PWM1->_3_LOAD = (SYSTEM_CLOCK_FREQ / 64 / frequency) - 1;
	PWM1->_2_CMPB = PWM1->_3_CMPA = PWM1->_3_CMPB = 0;
	
	// Enable PWM on PF1, PF2 and PF3
	PWM1->_2_CTL |= 1 << PWMCTL_ENABLE;
	PWM1->_3_CTL |= 1 << PWMCTL_ENABLE;
	PWM1->ENABLE |= (1 << PWMENABLE_PWM5EN) | (1 << PWMENABLE_PWM6EN) | 
		(1 << PWMENABLE_PWM7EN);	
		
	// Compute and return maximum Comparator value (maximum allowed duty cycle).
	// This is based on the frequency of the PWM output (reload register value)
	return SYSTEM_CLOCK_FREQ / 64 / frequency - 2;
}


/*-------------------------------------------------------------------------------
* Function:    	ledPWM_set_duty
* Purpose:    	Set the PWM duty cycle of the LED colour given.
* Arguments:
*		dutyCycle - duty cycle to set (in PWM clock ticks)
*		colour - LED pin to change its duty cycle (red/green/blue)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void ledPWM_set_duty(uint32_t dutyCycle, RGB_LED colour) {
	
	// Change the PWM comparator register value according to the LED colour
	// chosen
	switch (colour) {
		case RED: PWM1->_2_CMPB = dutyCycle; break;
		case GREEN: PWM1->_3_CMPB = dutyCycle; break;
		case BLUE: PWM1->_3_CMPA = dutyCycle; break;
	}
}

