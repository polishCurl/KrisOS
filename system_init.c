/*******************************************************************************
* File:     	system_init.s
* Brief:    	Initialisation functions for main system components
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	26/09/2016
* Last mod: 	28/09/2016
*
* Note: 		
*******************************************************************************/


/*-------------------------------------------------------------------------------
* Included headers
*------------------------------------------------------------------------------*/
#include "cortex_m4.h"
#include "system_init.h"


/*-------------------------------------------------------------------------------
* System clock speed
*------------------------------------------------------------------------------*/
uint64_t SYSTEM_CLOCK_FREQ;


/*-------------------------------------------------------------------------------
* Function:    	SystemInit
* Purpose:    	System initialisation method
* Arguments: 	-
* Returns: 		-	
* Note: 		
*	Initialisation of system clock, heap manager.
--------------------------------------------------------------------------------*/
void system_init(void) {
	system_clock_config(MAIN_OSC , XTAL_16MHz, 1, 1, 5);
	heap_init();
}



/*-------------------------------------------------------------------------------
* Function:    	SystemClockConfig
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
	
	SYSCTL_RCC2_R |= (1U << 31); 		// Use RCC2 as it provides more options
	SYSCTL_RCC2_R  |= (1 << 11); 		// Bypass the PLL for setup time
	SYSCTL_RCC_R &= ~(0x1F << 6);		// Clear the XTAL bits
	SYSCTL_RCC_R |= (oscFreq << 6);	// Set the freqency of the oscilator used
	SYSCTL_RCC2_R &= ~(0x7 << 4); 		// Clear the OSCSRC2 bits
	SYSCTL_RCC2_R |= (oscFreq << 4); 	// Set the oscilator source

	if (use400MHzPll) {
		SYSCTL_RCC2_R |= (1 << 30);		// Set 400MHz PLL 
		SYSTEM_CLOCK_FREQ = 400000000;  // Update clock value
	}
	else {
		SYSTEM_CLOCK_FREQ = 200000000;  // Update clock value
	}
	
	SYSCTL_RCC2_R |= ((divider - 1) << 22); // Set the clock divider
	SYSTEM_CLOCK_FREQ /= divider;
	
	if (usePll) {
		SYSCTL_RCC2_R &= ~(1 << 11); 	// Clear the PLL BYPASS bit
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


