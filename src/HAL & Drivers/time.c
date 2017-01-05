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
#include "system.h"
#include "kernel.h"



/*-------------------------------------------------------------------------------
* Crystal frequencies
*------------------------------------------------------------------------------*/
typedef enum {
	XTAL_3_579545MHz = 0x4,
	XTAL_3_6864MHz = 0x5,
	XTAL_4MHz =	0x6,
	XTAL_4_096MHz = 0x7,
	XTAL_4_9152MHz = 0x8,
	XTAL_5MHz = 0x9,
	XTAL_5_12MHz = 0xA,
	XTAL_6MHz = 0xB,
	XTAL_6_144MHz = 0xC,
	XTAL_7_3728MHz = 0xD,
	XTAL_8MHz = 0xE,
	XTAL_8_192MHz = 0xF,
	XTAL_10MHz = 0x10,
	XTAL_12MHz = 0x11,
	XTAL_12_288MHz = 0x12,
	XTAL_13_56MHz = 0x13,
	XTAL_14_31818MHz = 0x14,
	XTAL_16MHz = 0x15,
	XTAL_16_384MHz = 0x16,
	XTAL_18MHz = 0x17,
	XTAL_20MHz = 0x18,
	XTAL_24MHz = 0x19,
	XTAL_25MHz = 0x1A
} XTAL;


/*-------------------------------------------------------------------------------
* System clock speed, two variables, one for system, the other for user
*------------------------------------------------------------------------------*/
uint32_t SYSTEM_CLOCK_FREQ;



/*-------------------------------------------------------------------------------
* Function:    	system_clock_config
* Purpose:    	System clock setup method. Determines system clock frequency
* Arguments: 	
*		osc_src - oscilator source from which the system clock is derived
*		divider - system clock divider, if 0 then PLL is not used, otherwise the
*				  derived clock frequency is (200MHz / divider)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void system_clock_config(uint32_t oscSrc, uint32_t divider) {
		
	SYSCTL->RCC2 &= ~(1U << USERCC2_Pos); 	// use RCC (not RCC2)
	SYSCTL->RCC |= (1 << BYPASS_Pos); 		// bypass the PLL for the configuration time
	SYSCTL->RCC &= ~(0x1F << XTAL_Pos);		// Clear the XTAL bits
	SYSCTL->RCC |= (0x15 << XTAL_Pos);		// Set the freqency of the oscilator used						   
	SYSCTL->RCC &= ~(0x3 << OSCSRC_Pos);	// Clear the OSCSRC bits
	SYSCTL->RCC |= (oscSrc << OSCSRC_Pos); 	// Set the oscilator source	
	
	if (divider > 0) {
		SYSCTL->RCC &= ~(1 << PWRDN_Pos); 	// Power up the PLL
		SYSCTL->RCC |= (1 << USESYSDIV_Pos);// Use the system clock divider
		SYSCTL->RCC &= ~(0xF << SYSDIV_Pos);// Set the clock divider
		SYSCTL->RCC |= ((divider - 1) << SYSDIV_Pos);
		while ((SYSCTL->RIS & (1 << PLLRIS_Pos)) == 0); // wait for PLL to stabilise
		SYSCTL->RCC &= ~(1 << BYPASS_Pos); 	// Clear the PLL BYPASS bit
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
	SYSTICK->CURRENT = 0; 			// Clear the current counter value
	
	// Main clock as source, SysTick enabled with IRQs
	SYSTICK->CTRL |= (1 << CLK_SRC_Pos) | (1 << INTEN_Pos) | (1 << ENABLE_Pos);			
	KrisOS.ticks = 0;
}


