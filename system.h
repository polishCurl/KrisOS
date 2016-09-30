/*******************************************************************************
* File:     	system.h
* Brief:    	Function prototypes for accessing core system functionality
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"


/*-------------------------------------------------------------------------------
* System clock speed
*------------------------------------------------------------------------------*/
extern uint64_t SYSTEM_CLOCK_FREQ;


/*-------------------------------------------------------------------------------
* Clock ticks
*------------------------------------------------------------------------------*/
extern uint64_t TICKS;


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
* Available oscilators
*------------------------------------------------------------------------------*/
typedef enum {
	MAIN_OSC = 0x0,
	INT_OSC_16MHz = 0x1,
	INT_OSC_4MHz = 0x2, 	// can't drive the PLL
	INT_OSC_30kHz = 0x3, 	// can't drive the PLL
	
} OSCSRC;


/*-------------------------------------------------------------------------------
* Function prototypes for accessing special purpose registers
*------------------------------------------------------------------------------*/
void __disable_irqs(void);				// disable IRQs
void __enable_irqs(void);				// enable IRQs
void __set_base_prio(int32_t basePrio);	// disable IRQs with priotity lower than base
int32_t __get_base_prio(void);			// get base priority
void __enable_fpu(void); 				// enable the FPU co-processor

/*-------------------------------------------------------------------------------
* Function:    	SystemClockCongig
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
						 int8_t use400MHzPll, uint8_t divider);


/*-------------------------------------------------------------------------------
* Function:    	systick_config
* Purpose:    	Set periodic interrupts every 'ticks' clock cycles using SysTick
* Arguments: 	
*		ticks - number of clock cycles between system timer interrupts (24 bits)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void systick_config(uint32_t ticks);


/*-------------------------------------------------------------------------------
* Function:    	uart_init
* Purpose:    	Initialisation of UART interface.
* Arguments:
*		baudRate - controls the number of bits sent per second
* Returns: 		-	
--------------------------------------------------------------------------------*/
void uart_init(uint32_t baudRate);
