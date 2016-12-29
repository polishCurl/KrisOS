/*******************************************************************************
* File:     	special_reg_access.h
* Brief:    	Header file for special_reg_access.s
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"



/*-------------------------------------------------------------------------------
* Function:    	__disable_irqs
* Purpose:    	Disable interrupts by setting the I bit in PRIMASK register
* Arguments:	-
* Returns: 		-
*------------------------------------------------------------------------------*/
void __disable_irqs(void);

	
	
/*-------------------------------------------------------------------------------
* Function:    	__enable_irqs
* Purpose:    	Enable interrupts by clearing the I bit in PRIMASK register
* Arguments:	-
* Returns: 		-
*------------------------------------------------------------------------------*/
void __enable_irqs(void);
	
	
		
/*-------------------------------------------------------------------------------
* Function:    	__set_base_prio
* Purpose:    	Set the base priority, disables interrupts with priority lower than
*				the argument. Writing 0 turns of priority masking
* Arguments:
*		R0 - base priority
* Returns: 		-
*------------------------------------------------------------------------------*/	
void __set_base_prio(uint32_t basePrio);



/*-------------------------------------------------------------------------------
* Function:    	__get_base_prio
* Purpose:    	Read the masking priority level
* Arguments:	-
* Returns: 		
*		R0 - masking priority level
*------------------------------------------------------------------------------*/
uint32_t __get_base_prio(void);



/*-------------------------------------------------------------------------------
* Function:    	__set_psp
* Purpose:    	Set the value of the Process Stack Pointer
* Arguments:
*		address - new base address of process stack
* Returns: 		-
*------------------------------------------------------------------------------*/	
void __set_psp(uint32_t address);



/*-------------------------------------------------------------------------------
* Function:    	__get_psp
* Purpose:    	Get the value of the Process Stack Pointer
* Arguments:	-
* Returns: 		
*		value of PSP
*------------------------------------------------------------------------------*/	
uint32_t __get_psp(void);



/*-------------------------------------------------------------------------------
* Function:    	__enable_fpu
* Purpose:    	Enable the floating-point unit co-processor
* Arguments: 	-
* Returns: 		-	
*------------------------------------------------------------------------------*/
void __enable_fpu(void);



/*-------------------------------------------------------------------------------
* Function:    	__disable_fpu
* Purpose:    	Disable the floating-point unit co-processor
* Arguments: 	-
* Returns: 		-	
*------------------------------------------------------------------------------*/
void __disable_fpu(void);



/*------------------------------------------------------------------------------
* Function:    	__set_control
* Purpose:    	Set the value of the CONTROL register
* Arguments: 	
*		R0 - value to set
* Returns: 		-	
*-----------------------------------------------------------------------------*/
void __set_control(uint32_t value);



/*------------------------------------------------------------------------------
* Function:    	__get_control
* Purpose:    	Get the value from the CONTROL register
* Arguments: 	-
* Returns: 		
*		value from CONTROL register
*-----------------------------------------------------------------------------*/
uint32_t __get_control(void);



/*------------------------------------------------------------------------------
* Function:    	__get_ipsr
* Purpose:    	Get the value from the IPSR register
* Arguments: 	-
* Returns: 		
*		value from IPSR register
*-----------------------------------------------------------------------------*/
uint32_t __get_ipsr(void);



/*------------------------------------------------------------------------------
* Function:    	__start_critical
* Purpose:    	Enter critical section by disabling interrupts and saving the 
* 				prevous interrupt control value in PRIMASK register
* Arguments: 	-	
* Returns: 		-
*-----------------------------------------------------------------------------*/
void __start_critical(void);



/*------------------------------------------------------------------------------
* Function:    	__end_critical
* Purpose:    	End critical section by restoring the previous value of PRIMASK
* Arguments: 	-	
* Returns: 		-
*-----------------------------------------------------------------------------*/
void __end_critical(void);


