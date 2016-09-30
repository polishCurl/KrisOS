/*******************************************************************************
* File:     	system.c
* Brief:    	System related code
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	26/09/2016
* Last mod: 	28/09/2016
*
* Note: 		Methods for configuring core system functionality such as 
* 				the main clock and system timer
*******************************************************************************/
#include "common.h"
#include "tm4c123gh6pm.h"
#include "system.h"


/*-------------------------------------------------------------------------------
* System clock speed
*------------------------------------------------------------------------------*/
uint64_t SYSTEM_CLOCK_FREQ;


/*-------------------------------------------------------------------------------
* Clock ticks
*------------------------------------------------------------------------------*/
uint64_t TICKS;


/*-------------------------------------------------------------------------------
* Function:    	system_init
* Purpose:    	System initialisation method
* Arguments: 	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void system_init(void) {
	system_clock_config(MAIN_OSC, XTAL_16MHz, 1, 1, 5);		// set up the system clock
	__enable_fpu();
}

/*-------------------------------------------------------------------------------
* Function:    	system_clock_config
* Purpose:    	System clock setup method. Determines system clock frequency
* Arguments: 	
*		osc_src - oscilator source from which the system clock is derived
* 		osc_freq - frequency of the oscilator used
* 		use_pll - 0 or 1 depending on whether the Phase-Lock-Loop is used
* 		use_400MHz_pll - 0 or 1 depending on whether the 400MHz or 200MHz PLL is used
*		divider - system clock divider. Divides the PLL frequency by (divider + 1)
*
* Returns: 		-	
--------------------------------------------------------------------------------*/
void system_clock_config(OSCSRC oscSrc, XTAL oscFreq, int8_t usePll, 
					   int8_t use400MHzPll, uint8_t divider) {
	
	SYSCTL_RCC2 |= (1U << 31); 		// Use RCC2 as it provides more options
	SYSCTL_RCC2  |= (1 << 11); 		// Bypass the PLL for setup time
	SYSCTL_RCC &= ~(0x1F << 6);		// Clear the XTAL bits
	SYSCTL_RCC |= (oscFreq << 6);	// Set the freqency of the oscilator used
	SYSCTL_RCC2 &= ~(0x7 << 4); 		// Clear the OSCSRC2 bits
	SYSCTL_RCC2 |= (oscFreq << 4); 	// Set the oscilator source

	if (use400MHzPll) {
		SYSCTL_RCC2 |= (1 << 30);		// Set 400MHz PLL 
		SYSTEM_CLOCK_FREQ = 400000000;  // Update clock value
	}
	else {
		SYSTEM_CLOCK_FREQ = 200000000;  // Update clock value
	}
	
	SYSCTL_RCC2 |= ((divider - 1) << 22); // Set the clock divider
	SYSTEM_CLOCK_FREQ /= divider;
	
	if (usePll) {
		//while ((SYSCTL_RIS & (1 << 6)) == 0); // wait for PLL to stabilise
		SYSCTL_RCC2 &= ~(1 << 11); 	// Clear the PLL BYPASS bit
	}
	else {
		// Update clock value
		if (oscSrc == MAIN_OSC || oscSrc == INT_OSC_16MHz) 
			SYSTEM_CLOCK_FREQ = 16000000;
		else if (oscSrc == INT_OSC_4MHz)
			SYSTEM_CLOCK_FREQ = 4000000;
		else 
			SYSTEM_CLOCK_FREQ = 30000;
	}					  
}


/*-------------------------------------------------------------------------------
* Function:    	__enable_fpu
* Purpose:    	Enable the floating-point unit co-processor
* Arguments: 	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void __enable_fpu(void) {
	CPASR |= (0x3 << 20);
}
					   

/*-------------------------------------------------------------------------------
* Function:    	systick_config
* Purpose:    	Set periodic interrupts every 'ticks' clock cycles using SysTick
* Arguments: 	
*		ticks - number of clock cycles between system timer interrupts (24 bits)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void systick_config(uint32_t cycles) {
	
	SYSTICK->CTRL = 0; 				// Disable SysTick for the time of configuration
	SYSTICK->RELOAD = cycles - 1; 	// Set reload value, counts down to 0, so decrement needed
	SYSTICK->CURRENT = 0; 			// Clear the counter
	SYSTICK->CTRL |= 0x7;			// Main clock as source, SysTick enabled with IRQs
}
