/*******************************************************************************
* File:     	nvic.c
* Brief:    	Nested Vectored Interrupt Controller (NVIC) utility methods
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/10/2016
* Last mod: 	10/10/2016
*
* Note: 		
*******************************************************************************/
#include "nvic.h"
#include "tm4c123gh6pm.h"


/*-------------------------------------------------------------------------------
* Function:    	nvic_enable_irq
* Purpose:    	Enable interrupt source
* Arguments: 	
*		irq - interrupt source to enable
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nvic_enable_irq(IRQn_Type irq) {
	if (irq >= 0)
		NVIC->ISER[irq >> 5] |= (uint32_t) (1 << (irq & 0x1F));
}


/*-------------------------------------------------------------------------------
* Function:    	nvic_disable_irq
* Purpose:    	Disable interrupt source
* Arguments: 	
*		irq - interrupt source to disable
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nvic_disable_irq(IRQn_Type irq) {
	if (irq >= 0)
		NVIC->ICER[irq >> 5] |= (uint32_t) (1 << (irq & 0x1F));
}


/*-------------------------------------------------------------------------------
* Function:    	nvic_set_pending
* Purpose:    	Set Pending Interrupt
* Arguments: 	
*		irq - interrupt source set pending
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nvic_set_pending(IRQn_Type irq) {
	if (irq >= 0)
		NVIC->ISPR[irq >> 5] |= (uint32_t) (1 << (irq & 0x1F));
}


/*-------------------------------------------------------------------------------
* Function:    	nvic_clear_pending
* Purpose:    	Clear Pending Interrupt
* Arguments: 	
*		irq - interrupt source clear
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nvic_clear_pending(IRQn_Type irq) {
	if (irq >= 0)
		NVIC->ICPR[irq >> 5] |= (uint32_t) (1 << (irq & 0x1F));
}

