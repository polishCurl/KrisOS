/*******************************************************************************
* File:     	nvic.c
* Brief:    	Nested Vectored Interrupt Controller (NVIC) utility methods
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/10/2016
* Last mod: 	05/12/2016
*
* Note: 		
*	The NVIC registers can only be accessed from the priviliged mode. Since the
*	user code runs in unpriuviliged Thread mode, there is a need for a set of
*	SVC calls for configuring the Interrupts from user level.
*******************************************************************************/
#include "system.h"
#include "kernel.h"



/*-------------------------------------------------------------------------------
* Minimum allowed IRQ priority at NVIC controller
*------------------------------------------------------------------------------*/
#define MIN_IRQ_PRIO 7



/*-------------------------------------------------------------------------------
* Right shift of the Interrupt number which is necessary to access the correct
* Set Enable, Clear Enable, Set Pending or Clear Pending register. Remember, each
* of these sets of registers contains 32 bits/fields for each interrupt. So if we
* shift right by 5 (divide by 32) we get the register offset.
--------------------------------------------------------------------------------*/
#define REG_Adr 5 				



/*-------------------------------------------------------------------------------
* After the right register offset is computed (see above), the bit offset
* is extracted from the IRQ number by taking its 5 least significant bits.
* This what the bitmask below is used for.
--------------------------------------------------------------------------------*/
#define PEND_EN_Msk 0x1F	



/*-------------------------------------------------------------------------------
* Each priority field for an Interrupt source occupies bits 7 to 5 (3 bits)
* of a byte inside the Interrupt Priority Registers. So if byte access is used
* a bit offset of 5 is needed. To get rid of remaining 3 priority fields in a 4-byte
* register the following bit mask should be used too.
--------------------------------------------------------------------------------*/
#define PRIO_BIT_OFFSET 5 
#define PRIO_Msk 0x7 			// Interrupt priority mask



/*-------------------------------------------------------------------------------
* Number of system exceptions with programmable priority. Only Reset, NMI and
* Hard Fault exceptions have fixed priorities: -3, -2, -1 correspondingly (higher 
* than any other Interrupt/Exception
--------------------------------------------------------------------------------*/
#define EXCEP_IRQ_No 12 		
			
			

/*-------------------------------------------------------------------------------
* Function:    	nvic_enable_irq
* Purpose:    	Enable interrupt source
* Arguments: 	
*		irq - interrupt source to enable
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_enable_irq(IRQn_Type irq) {
	
	// Applies only to Interrups (not Exceptions, which are always enabled)
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
	
	// Applies only to Interrups (not Exceptions, which are always enabled)
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
*		irq - interrupt source to set pending
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
*		irq - interrupt source to clear
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_clear_pending(IRQn_Type irq) {
	
	// Applies only to Interrups (not Exceptions)
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
*		irq - interrupt number to have its status checked
* Returns: 		
*		0 - not active, 1 - active
--------------------------------------------------------------------------------*/
uint32_t nvic_read_active(IRQn_Type irq) {
	
	// Applies only to Interrups (not Exceptions)
	if (irq >= 0)
		return (NVIC->IABR[irq >> REG_Adr] >> (irq & PEND_EN_Msk)) & 0x1;
	else
		return 1;
}



/*-------------------------------------------------------------------------------
* Function:    	nvic_set_priority
* Purpose:    	Set the priority of given interrupt
* Arguments: 	
*		irq - interrupt source to set the priority of
* 		priority - priority to set, from 0 (highest) to 7 (lowest)
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_set_priority(IRQn_Type irq, uint32_t priority) {
	
	// Test if priority to set is a valid one
	if (priority > MIN_IRQ_PRIO)
		exit(EXIT_INVALID_IRQ_PRIO);
	
	// Priorities of exceptions and interrupts are set differently. The 
	// exception priorities are stored inside the System Control Block registers
	// while the interrupt priorities can be changed by writing to NVIC registers
	if (irq >= 0)
		NVIC->IP[irq] = (priority & PRIO_Msk) << PRIO_BIT_OFFSET;
	else 
		SCB->SHP[EXCEP_IRQ_No + irq] = (priority & PRIO_Msk) << PRIO_BIT_OFFSET;
	
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	nvic_get_priority
* Purpose:    	Get the priority of given interrupt
* Arguments: 	
*		irq - interrupt source to get the priority of
* Returns: 		
*		priority of given IRQ (0 - highest, 7 - lowest)
--------------------------------------------------------------------------------*/
uint32_t nvic_get_priority(IRQn_Type irq) {
	
	// Priorities of exceptions and interrupts are retrieved differently. The 
	// exception priorities are stored inside the System Control Block registers
	// while the interrupt priorities can be changed by writing to NVIC registers
	if (irq >= 0)
		return (NVIC->IP[irq] >> PRIO_BIT_OFFSET) & PRIO_Msk;
	else 
		return (SCB->SHP[EXCEP_IRQ_No + irq] >> PRIO_BIT_OFFSET) & PRIO_Msk;
}
