/*******************************************************************************
* File:     	time.c
* Brief:    	System timing congifuration methods
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		Methods for controlling the system clock speed, periodic 
*				interrupts using SysTick timer, as well as other timing utilities
*******************************************************************************/
#include "time.h"
#include "tm4c123gh6pm.h"


/*-------------------------------------------------------------------------------
* System clock speed
*------------------------------------------------------------------------------*/
uint32_t SYSTEM_CLOCK_FREQ;


/*-------------------------------------------------------------------------------
* Clock ticks
*------------------------------------------------------------------------------*/
uint64_t TICKS;


/*-------------------------------------------------------------------------------
* Function:    	system_clock_config
* Purpose:    	System clock setup method. Determines system clock frequency
* Arguments: 	
*		osc_src - oscilator source from which the system clock is derived
*		divider - system clock divider, if 0 then PLL is not used, otherwise the
*				  derived clock frequency is (200MHz / divider)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void system_clock_config(OSCSRC oscSrc, uint8_t divider) {
		
	SYSCTL->RCC2 &= ~(1U << 31);
	SYSCTL->RCC |= (1 << 11);
	SYSCTL->RCC &= ~(0x1F << 6);		// Clear the XTAL bits
	SYSCTL->RCC |= (0x15 << 6);			// Set the freqency of the oscilator used						   
	SYSCTL->RCC &= ~(0x3 << 4); 		// Clear the OSCSRC bits
	SYSCTL->RCC |= (oscSrc << 4); 		// Set the oscilator source	
	
	if (divider > 0) {
		SYSCTL->RCC &= ~(1 << 13); 		// Power up the PLL
		SYSCTL->RCC |= (1 << 22); 		// Use the system clock divider
		SYSCTL->RCC &= ~(0xF << 23); 	// Set the clock divider
		SYSCTL->RCC |= ((divider - 1) << 23); 	// Set the clock divider
		while ((SYSCTL->RIS & (1 << 6)) == 0); 	// wait for PLL to stabilise
		SYSCTL->RCC &= ~(1 << 11); 		// Clear the PLL BYPASS bit
	}				   
	
	if (oscSrc == MAIN_OSC || oscSrc == INT_OSC_16MHz) 
		if (divider == 0) 
			SYSTEM_CLOCK_FREQ = 16000000;
		else 
			SYSTEM_CLOCK_FREQ = 200000000 / divider;
	else if (oscSrc == INT_OSC_4MHz)
		SYSTEM_CLOCK_FREQ = 4000000;
	else 
		SYSTEM_CLOCK_FREQ = 30000;
}


/*-------------------------------------------------------------------------------
* Function:    	systick_config
* Purpose:    	Set periodic interrupts every 'ticks' clock cycles using SysTick
* Arguments: 	
*		cycles - number of clock cycles between system timer interrupts (24 bits)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void systick_config(uint32_t cycles) {
	
	SYSTICK->CTRL = 0; 				// Disable SysTick for the time of configuration
	SYSTICK->RELOAD = cycles - 1; 	// Set reload value, counts down to 0, so decrement needed
	SYSTICK->CURRENT = 0; 			// Clear the counter
	SYSTICK->CTRL |= 0x7;			// Main clock as source, SysTick enabled with IRQs
	TICKS = 0;
}
