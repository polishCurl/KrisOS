; ******************************************************************************
; File:     	cortex_m4_reg.s
; Brief:    	Methods for accessing special purpose registers inside Cortex-M4
; Author: 		Krzysztof Koch
; Version:		V1.00
; Date created:	29/09/2016
; Last mod: 	29/09/2016
;
; Note: 		
; ******************************************************************************

				
				PRESERVE8						  ; Preserve eight-byte alignment of the stack
				THUMB 							  ; Use Thumb instruction set
				AREA    |.text|, CODE, READONLY	  ; Instructions to be placed in ROM		
							
;-------------------------------------------------------------------------------
; Function:    	__disable_irqs
; Purpose:    	Disable interrupts by setting the I bit in PRIMASK register
; Arguments:	-
; Returns: 		-
;-------------------------------------------------------------------------------
__disable_irqs	PROC
				EXPORT	__disable_irqs
				CPSID	I
				BX 		LR
				ENDP
				
				
;-------------------------------------------------------------------------------
; Function:    	__enable_irqs
; Purpose:    	Enable interrupts by clearing the I bit in PRIMASK register
; Arguments:	-
; Returns: 		-
;-------------------------------------------------------------------------------
__enable_irqs	PROC
				EXPORT	__enable_irqs
				CPSIE	I
				BX 		LR
				ENDP
				
		
;-------------------------------------------------------------------------------
; Function:    	__set_base_prio
; Purpose:    	Set the base priority, disables interrupts with priority lower than
;				the argument. Writing 0 turns of priority masking
; Arguments:
;		R0 - base priority
; Returns: 		-
;-------------------------------------------------------------------------------	
__set_base_prio	PROC
				EXPORT 	__set_base_prio
				MSR 	BASEPRI, R0
				BX 		LR
				ENDP
					

;-------------------------------------------------------------------------------
; Function:    	__get_base_prio
; Purpose:    	Read the masking priority level
; Arguments:	-
; Returns: 		
;		R0 - masking priority level
;-------------------------------------------------------------------------------	
__get_base_prio	PROC
				EXPORT 	__get_base_prio
				MRS 	R0, BASEPRI
				BX 		LR
				ENDP
				
				END
				
				
				