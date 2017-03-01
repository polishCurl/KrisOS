/*******************************************************************************
* File:     	nvic.h
* Brief:    	Header file for nvic.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/10/2016
* Last mod: 	05/12/2016
*
* Note: 		Contains interrupt numbers for each interrupt source
*******************************************************************************/
#include "common.h"
#include "tm4c123gh6pm.h"



/*-------------------------------------------------------------------------------
* Function:    	nvic_enable_irq
* Purpose:    	Enable interrupt source
* Arguments: 	
*		irq - interrupt source to enable
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_enable_irq(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	nvic_disable_irq
* Purpose:    	Disable interrupt source
* Arguments: 	
*		irq - interrupt source to disable
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_disable_irq(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	nvic_set_pending
* Purpose:    	Set Pending Interrupt
* Arguments: 	
*		irq - interrupt source set pending
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_set_pending(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	nvic_clear_pending
* Purpose:    	Clear Pending Interrupt
* Arguments: 	
*		irq - interrupt source clear
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_clear_pending(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	nvic_read_active
* Purpose:    	Check if given Interrupt is currently active
* Arguments: 	
*		irq - interrupt number to have it status checked
* Returns: 		
*		0 if not active, 1 otherwise
--------------------------------------------------------------------------------*/
uint32_t nvic_read_active(IRQn_Type irq);



/*-------------------------------------------------------------------------------
* Function:    	nvic_set_priority
* Purpose:    	Set the priority of given interrupt
* Arguments: 	
*		irq - interrupt number to set the priority of
* 		priority - priority to set, from 0 (highest) to 7 (lowest)
* Returns: 
* 		exit status
--------------------------------------------------------------------------------*/
uint32_t nvic_set_priority(IRQn_Type irq, uint32_t priority);



/*-------------------------------------------------------------------------------
* Function:    	nvic_get_priority
* Purpose:    	Get the priority of given interrupt
* Arguments: 	
*		irq - interrupt number to get the priority of
* Returns: 		
*		priority of given IRQ (0 - highest, 7 - lowest)
--------------------------------------------------------------------------------*/
uint32_t nvic_get_priority(IRQn_Type irq);
