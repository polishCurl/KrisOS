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
* System clock speed
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
		
	// Yse RCC (not RCC2) and bypass the PLL for the configuration time
	SYSCTL->RCC2 &= ~(1U << USERCC2);
	SYSCTL->RCC |= (1 << RCC_BYPASS);
	
	// Clear the XTAL bits, set the freqency of the oscilator used and the oscilator 
	// source
	SYSCTL->RCC &= ~(0x1F << RCC_XTAL);		
	SYSCTL->RCC |= (0x15 << RCC_XTAL);							   
	SYSCTL->RCC &= ~(0x3 << RCC_OSCSRC);	
	SYSCTL->RCC |= (oscSrc << RCC_OSCSRC); 	 	
	
	if (divider > 0) {
		// Power up the PLL
		SYSCTL->RCC &= ~(1 << RCC_PWRDN); 

		// Set the clock divider		
		SYSCTL->RCC |= (1 << RCC_USESYSDIV);
		SYSCTL->RCC &= ~(0xF << RCC_SYSDIV);
		SYSCTL->RCC |= ((divider - 1) << RCC_SYSDIV);
		
		// Wait for PLL to stabilise and clear the PLL BYPASS bit
		while ((SYSCTL->RIS & (1 << PLLRIS)) == 0); 
		SYSCTL->RCC &= ~(1 << RCC_BYPASS);
	}		
	
	// Derive the system clock frequency from the settings used. This is used
	// later for deriving other clocks from the system clock
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
* Purpose:    	Configure the KrisOS clock
* Arguments: 	
*		cycles - number of systen clock cycles between OS timer interrupts (24 bits)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void systick_config(uint32_t cycles) {
	
	// Disable SysTick for the time of configuration. Set the reload value and 
	// reset the SysTick counter
	SYSTICK->CTRL = 0; 				
	SYSTICK->RELOAD = cycles - 1; 
	SYSTICK->CURRENT = 0; 			
	
	// Main clock as source, SysTick enabled with IRQs. Reset the OS 'ticks' counter
	SYSTICK->CTRL |= (1 << CTRL_CLK_SRC) | (1 << CTRL_INTEN) | (1 << CTRL_ENABLE);			
	KrisOS.ticks = 0;
	
	// OS clock ticks are the most important events in the KrisOS operating system. So,
	// they have maximum priority
	nvic_set_priority(SysTick_IRQn, 0);

}


