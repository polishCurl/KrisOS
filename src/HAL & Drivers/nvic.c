/*******************************************************************************
* File:     	nvic.c
* Brief:    	Nested Vectored Interrupt Controller (NVIC) utility methods
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/10/2016
* Last mod: 	05/12/2016
*
* Note: 		
*******************************************************************************/
#include "system.h"
#include "kernel.h"



/*-------------------------------------------------------------------------------
* Minimum allowed IRQ priority at NVIC controller
*------------------------------------------------------------------------------*/
#define MIN_IRQ_PRIO 7



/*-------------------------------------------------------------------------------
* Bit masks and shift values	
--------------------------------------------------------------------------------*/
#define PRIO 5 					// LSB position of priority within given byte
#define PRIO_Msk 0x7 			// Interrupt priority mask
#define ACT_Msk 0x1 			// Interrupt-active mask
#define EXCEPTION_No 15 		// Number of Cortex-M4 processor exceptions
#define REG_Adr 5 				// Shift for register address offset for enable and pending bits
#define PEND_EN_Msk 0x1F		// Bitmask for accessing the right pending/active bit
#define EXCEP_IRQ_No 12 		// Number of system exceptions with programmable priority



/*-------------------------------------------------------------------------------
* Function:    	nvic_enable_irq
* Purpose:    	Enable interrupt source
* Arguments: 	
*		irq - interrupt source to enable
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_enable_irq(IRQn_Type irq) {
	
	// Applies only to Interrups (not Exceptions)
	if (irq >= 0) {
		NVIC->ISER[irq >> REG_Adr] |= (uint32_t) (1 << (irq & PEND_EN_Msk));
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}



/*-------------------------------------------------------------------------------
* Function:    	nvic_disable_irq
* Purpose:    	Disable interrupt source
* Arguments: 	
*		irq - interrupt source to disable
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_disable_irq(IRQn_Type irq) {
	
	// Applies only to Interrups (not Exceptions)
	if (irq >= 0) {
		NVIC->ICER[irq >> REG_Adr] |= (uint32_t) (1 << (irq & PEND_EN_Msk));
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}



/*-------------------------------------------------------------------------------
* Function:    	nvic_set_pending
* Purpose:    	Set Pending Interrupt
* Arguments: 	
*		irq - interrupt source set pending
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_set_pending(IRQn_Type irq) {
	
	// Applies only to Interrups (not Exceptions)
	if (irq >= 0) {
		NVIC->ISPR[irq >> REG_Adr] |= (uint32_t) (1 << (irq & PEND_EN_Msk));
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}



/*-------------------------------------------------------------------------------
* Function:    	nvic_clear_pending
* Purpose:    	Clear Pending Interrupt
* Arguments: 	
*		irq - interrupt source clear
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_clear_pending(IRQn_Type irq) {
	if (irq >= 0) {
		NVIC->ICPR[irq >> REG_Adr] |= (uint32_t) (1 << (irq & PEND_EN_Msk));
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}



/*-------------------------------------------------------------------------------
* Function:    	nvic_read_active
* Purpose:    	Check if given Interrupt is currently active
* Arguments: 	
*		irq - interrupt number to have it status checked
* Returns: 		
*		0 if not active, 1 otherwise
--------------------------------------------------------------------------------*/
uint32_t nvic_read_active(IRQn_Type irq) {
	if (irq >= 0)
		return (NVIC->IABR[irq >> REG_Adr] >> (irq & PEND_EN_Msk)) & ACT_Msk;
	else
		return 1;
}



/*-------------------------------------------------------------------------------
* Function:    	nvic_set_priority
* Purpose:    	Set the priority of given interrupt
* Arguments: 	
*		irq - interrupt number to set the priority of
* 		priority - priority to set, from 0 (highest) to 7 (lowest)
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_set_priority(IRQn_Type irq, uint32_t priority) {
	
	// Test if priority to set is a valid one
	if (priority > MIN_IRQ_PRIO)
		exit(EXIT_INVALID_IRQ_PRIO);
	
	// Priorities of exceptions and interrupts are set differently
	if (irq >= 0)
		NVIC->IP[irq] = (priority & PRIO_Msk) << PRIO;
	else 
		SCB->SHP[EXCEP_IRQ_No + irq] = (priority & PRIO_Msk) << PRIO;
	
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	nvic_get_priority
* Purpose:    	Get the priority of given interrupt
* Arguments: 	
*		irq - interrupt number to get the priority of
* Returns: 		
*		priority of given IRQ (0 - highest, 7 - lowest)
--------------------------------------------------------------------------------*/
uint32_t nvic_get_priority(IRQn_Type irq) {
	
	// Priorities of exceptions and interrupts are retrieved differently
	if (irq >= 0)
		return (NVIC->IP[irq] >> PRIO) & PRIO_Msk;
	else 
		return (SCB->SHP[EXCEP_IRQ_No + irq] >> PRIO) & PRIO_Msk;
}
