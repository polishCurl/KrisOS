/*******************************************************************************
* File:     	cortex_m4.h
* Brief:    	Cortex-M4 MCU descriptor 
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	27/09/2016
* Last mod: 	27/09/2016
*
* Note: 		Helper functions and definitions for Cortex-M4 core. Defines
				standard datatypes as well as functions for accessing special
				purpose registers inside the core
*******************************************************************************/


/*-------------------------------------------------------------------------------
* Included headers
*------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include "heap_manager.h"


/*-------------------------------------------------------------------------------
* IO definitions (access restrictions to peripheral registers)
*------------------------------------------------------------------------------*/
#define __I volatile const 		// input, read-only
#define __O volatile 			// output, write-only
#define __IO volatile 			// input-output, read and write permissions


/*-------------------------------------------------------------------------------
* Peripheral register addresses
*------------------------------------------------------------------------------*/

// Main clock registers
#define SYSCTL_RCC_R *((__IO uint32_t*) 0x400FE060)
#define SYSCTL_RIS_R *((__IO uint32_t*) 0x400FE050)
#define SYSCTL_RCC2_R *((__IO uint32_t*) 0x400FE070)




/*-------------------------------------------------------------------------------
* System clock speed
*------------------------------------------------------------------------------*/
extern uint64_t SYSTEM_CLOCK_FREQ;
