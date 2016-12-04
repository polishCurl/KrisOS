; ******************************************************************************
; File:     	special_reg_access.s
; Brief:    	Routines for accessing special purpose registers
; Author: 		Krzysztof Koch
; Version:		V1.00
; Date created:	26/09/2016
; Last mod: 	27/09/2016
;
; Note: 		Methods allowing to access internal special registers and other
; 				functionality which can't easily be accessed using C language syntax
; ******************************************************************************


				PRESERVE8						  	; Preserve eight-byte alignment of the stack
				THUMB 							  	; Use Thumb instruction set
				AREA    |.text|, CODE, READONLY		; Instructions to be placed in ROM
;-------------------------------------------------------------------------------
; Function:    	__disable_irqs
; Purpose:    	Disable interrupts by setting the I bit in PRIMASK register
; Arguments:	-
; Returns: 		-
;-------------------------------------------------------------------------------
				EXPORT	__disable_irqs
__disable_irqs	PROC
				CPSID	I
				BX 		LR
				ENDP
				
				
;-------------------------------------------------------------------------------
; Function:    	__enable_irqs
; Purpose:    	Enable interrupts by clearing the I bit in PRIMASK register
; Arguments:	-
; Returns: 		-
;-------------------------------------------------------------------------------
				EXPORT	__enable_irqs
__enable_irqs	PROC
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
				EXPORT 	__set_base_prio
__set_base_prio	PROC
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
				EXPORT 	__get_base_prio
__get_base_prio	PROC
				MRS 	R0, BASEPRI
				BX 		LR
				ENDP
				
;-------------------------------------------------------------------------------
; Function:    	__set_psp
; Purpose:    	Set the value of the Process Stack Pointer
; Arguments: 	
;		R0 - new base address of the process stack
; Returns: 		-	
;-------------------------------------------------------------------------------
				EXPORT	__set_psp
__set_psp		PROC
				MSR		PSP, r0
				BX 		LR
				ENDP


;-------------------------------------------------------------------------------
; Function:    	__get_psp
; Purpose:    	Get the value of the Process Stack Pointer
; Arguments: 	-
; Returns: 		
;		R0 - value of PSP
;-------------------------------------------------------------------------------
				EXPORT	__get_psp
__get_psp		PROC
				MRS		R0, PSP
				BX 		LR
				ENDP


;-------------------------------------------------------------------------------
; Function:    	__enable_fpu
; Purpose:    	Enable the floating-point unit co-processor
; Arguments: 	-
; Returns: 		-	
;-------------------------------------------------------------------------------
				EXPORT	__enable_fpu
__enable_fpu	PROC
				LDR   	R0, =0xE000ED88
				LDR     R1, [R0]
				ORR     R1, R1, #(0xF << 20)
				STR     R1, [R0]
				DSB							; wait for store to complete
				ISB 						; flush the pipeline	
				BX 		LR
				ENDP


;-------------------------------------------------------------------------------
; Function:    	__disable_fpu
; Purpose:    	Enable the floating-point unit co-processor
; Arguments: 	-
; Returns: 		-	
;-------------------------------------------------------------------------------
				EXPORT	__disable_fpu
__disable_fpu	PROC
				LDR   	R0, =0xE000ED88
				LDR     R1, [R0]
				BIC     R1, R1, #(0xF << 20)
				STR     R1, [R0]
				DSB							; wait for store to complete
				ISB 						; flush the pipeline	
				BX 		LR
				ENDP
				

;-------------------------------------------------------------------------------
; Function:    	__set_control
; Purpose:    	Set the value of the CONTROL register
; Arguments: 	
;		R0 - value to set
; Returns: 		-	
;-------------------------------------------------------------------------------
				EXPORT	__set_control
__set_control	PROC
				MSR		CONTROL, r0
				ISB							; flush the pipeline
				BX 		LR
				ENDP


;-------------------------------------------------------------------------------
; Function:    	__get_control
; Purpose:    	Get the value from the CONTROL register
; Arguments: 	-	
; Returns: 		
;		R0 - value from CONTROL register
;-------------------------------------------------------------------------------
				EXPORT	__get_control
__get_control	PROC
				MRS		R0, CONTROL
				BX 		LR
				ENDP
		

;-------------------------------------------------------------------------------
; Function:    	__start_critical
; Purpose:    	Enter critical section by disabling interrupts and saving the 
; 				prevous interrupt control value in PRIMASK register
; Arguments: 	-	
; Returns: 		-
;-------------------------------------------------------------------------------
				EXPORT	__start_critical
__start_critical\
				PROC
				MRS		R0, PRIMASK
				PUSH 	{R0}
				CPSID	I
				BX 		LR
				ENDP


;-------------------------------------------------------------------------------
; Function:    	__end_critical
; Purpose:    	End critical section by restoring the previous value of PRIMASK
; Arguments: 	-	
; Returns: 		-
;-------------------------------------------------------------------------------
				EXPORT	__end_critical
__end_critical	PROC
				POP 	{R0}
				MSR		PRIMASK, R0
				BX 		LR
				ENDP

				
				
				ALIGN
                END