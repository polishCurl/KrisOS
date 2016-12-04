/*******************************************************************************
* File:     	time.h
* Brief:    	Header file for time.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	04/12/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"


/*-------------------------------------------------------------------------------
* System clock speed
*------------------------------------------------------------------------------*/
extern uint32_t SYSTEM_CLOCK_FREQ;


/*-------------------------------------------------------------------------------
* Clock ticks
*------------------------------------------------------------------------------*/
extern uint64_t TICKS;


/*-------------------------------------------------------------------------------
* Available oscilators
*------------------------------------------------------------------------------*/
typedef enum {
	MAIN_OSC = 0x0,				// 16MHz external crystal oscilator 
	INT_OSC_16MHz = 0x1, 		// 16MHz internal oscilator 
	INT_OSC_4MHz = 0x2, 		// 4MHz internal oscilator (can't drive the PLL)
	INT_OSC_30kHz = 0x3, 		// 30kHz internal oscilator (can't drive the PLL)
} OSCSRC;


/*-------------------------------------------------------------------------------
* Function:    	system_clock_config
* Purpose:    	System clock setup method. Determines system clock frequency
* Arguments: 	
*		osc_src - oscilator source from which the system clock is derived
*		divider - system clock divider, if 0 then PLL is not used, otherwise the
*				  derived clock frequency is (200MHz / divider)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void system_clock_config(uint32_t oscSrc, uint32_t divider);


/*-------------------------------------------------------------------------------
* Function:    	systick_config
* Purpose:    	Set periodic interrupts every 'cycles' clock cycles using SysTick
* Arguments: 	
*		cycles - number of clock cycles between system timer interrupts (24 bits)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void systick_config(uint32_t cycles);
