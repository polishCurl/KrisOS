/*******************************************************************************
* File:     	tm4c123gh6pm.h
* Brief:    	Register definitions for tm4c123gh6pm microcontroller 
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"

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
#define SYSCTL_RCC *((__IO uint32_t*) 0x400FE060)
#define SYSCTL_RIS *((__IO uint32_t*) 0x400FE050)
#define SYSCTL_RCC2 *((__IO uint32_t*) 0x400FE070)

// SysTick timer registers
#define SysTickBase 0xE000E010
typedef struct
{
	__IO uint32_t CTRL;                  
	__IO uint32_t RELOAD;                   
	__IO uint32_t CURRENT;
	__I  uint32_t RESERVED;
	__IO uint32_t PRIO;
} SysTickType;
#define SYSTICK ((SysTickType*) SysTickBase)

// UART0 registers
#define UART0Base 0x4000C000
typedef struct
{
	__IO uint32_t DR;
	__IO uint32_t RSR;
	__I uint32_t RESERVED[5];
	__IO uint32_t FR;
	__IO uint32_t IBRD;
	__IO uint32_t FBRD;
	__IO uint32_t LCHR;
	__IO uint32_t CTL;
} UARTType;
#define UART0 ((UARTType*) UART0Base)
#define SYSCTL_RCGUART *((__IO uint32_t*) 0x400FE618)
#define SYSCTL_RCGCGPIO *((__IO uint32_t*) 0x400FE608)

// Coprocessor Access Control Register (FPU)
#define CPASR *((__O uint32_t*) 0xE000ED88)


