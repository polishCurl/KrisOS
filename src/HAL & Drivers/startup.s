; ******************************************************************************
; File:     	startup.s
; Brief:    	System startup code
; Author: 		Krzysztof Koch
; Version:		V1.00
; Date created:	26/09/2016
; Last mod: 	27/09/2016
;
; Note: 		System startup code containing stack and heap definitions as well 
;				as simple exception and interrupt handlers.
; ******************************************************************************


;-------------------------------------------------------------------------------
; Stack Configuration, double stacking in use
;-------------------------------------------------------------------------------
Handler_Stack_Size	EQU     0x00000200		  ; Stack size for handler mode (MSP)
Thread_Stack_Size	EQU     0x00000100		  ; Stack size for thread mode (PSP)

; Stack memory area - RAM, uninitialised with 8-byte alignment (requirement of AAPCS)
                AREA    STACK, NOINIT, READWRITE, ALIGN=3 
					
Handler_Stack	SPACE   Handler_Stack_Size
__initial_handler_sp	

Thread_Stack   	SPACE   Thread_Stack_Size
__initial_sp	


;-------------------------------------------------------------------------------
; Heap Configuration
;-------------------------------------------------------------------------------
				EXPORT 	Heap_Size
Heap_Size       EQU     0x00000100

; HEAP memory area - RAM, uninitialised with 8-byte alignment (requirement of AAPCS)
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
Heap_Mem        SPACE   Heap_Size
__heap_limit


;-------------------------------------------------------------------------------
; Vector Table Mapped to Address 0 at Reset
;-------------------------------------------------------------------------------
				PRESERVE8						  ; Preserve eight-byte alignment of the stack
				THUMB 							  ; Use Thumb instruction set
                AREA    RESET, DATA, READONLY 	  ; Data to be placed in ROM at address 0x0 
					
					
				EXPORT  __Vectors	
__Vectors		DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     GPIOA_Handler             ;   0: GPIO Port A
                DCD     GPIOB_Handler             ;   1: GPIO Port B
                DCD     GPIOC_Handler             ;   2: GPIO Port C
                DCD     GPIOD_Handler             ;   3: GPIO Port D
                DCD     GPIOE_Handler             ;   4: GPIO Port E
                DCD     UART0_Handler             ;   5: UART0 Rx and Tx
                DCD     UART1_Handler             ;   6: UART1 Rx and Tx
                DCD     SSI0_Handler              ;   7: SSI0 Rx and Tx
                DCD     I2C0_Handler              ;   8: I2C0 Master and Slave
                DCD     PMW0_FAULT_Handler        ;   9: PWM Fault
                DCD     PWM0_0_Handler            ;  10: PWM Generator 0
                DCD     PWM0_1_Handler            ;  11: PWM Generator 1
                DCD     PWM0_2_Handler            ;  12: PWM Generator 2
                DCD     QEI0_Handler              ;  13: Quadrature Encoder 0
                DCD     ADC0SS0_Handler           ;  14: ADC Sequence 0
                DCD     ADC0SS1_Handler           ;  15: ADC Sequence 1
                DCD     ADC0SS2_Handler           ;  16: ADC Sequence 2
                DCD     ADC0SS3_Handler           ;  17: ADC Sequence 3
                DCD     WDT0_Handler              ;  18: Watchdog timer
                DCD     TIMER0A_Handler           ;  19: Timer 0 subtimer A
                DCD     TIMER0B_Handler           ;  20: Timer 0 subtimer B
                DCD     TIMER1A_Handler           ;  21: Timer 1 subtimer A
                DCD     TIMER1B_Handler           ;  22: Timer 1 subtimer B
                DCD     TIMER2A_Handler           ;  23: Timer 2 subtimer A
                DCD     TIMER2B_Handler           ;  24: Timer 2 subtimer B
                DCD     COMP0_Handler             ;  25: Analog Comparator 0
                DCD     COMP1_Handler             ;  26: Analog Comparator 1
                DCD     COMP2_Handler             ;  27: Analog Comparator 2
                DCD     SYSCTL_Handler            ;  28: System Control (PLL, OSC, BO)
                DCD     FLASH_Handler             ;  29: FLASH Control
                DCD     GPIOF_Handler             ;  30: GPIO Port F
                DCD     GPIOG_Handler             ;  31: GPIO Port G
                DCD     GPIOH_Handler             ;  32: GPIO Port H
                DCD     UART2_Handler             ;  33: UART2 Rx and Tx
                DCD     SSI1_Handler              ;  34: SSI1 Rx and Tx
                DCD     TIMER3A_Handler           ;  35: Timer 3 subtimer A
                DCD     TIMER3B_Handler           ;  36: Timer 3 subtimer B
                DCD     I2C1_Handler              ;  37: I2C1 Master and Slave
                DCD     QEI1_Handler              ;  38: Quadrature Encoder 1
                DCD     CAN0_Handler              ;  39: CAN0
                DCD     CAN1_Handler              ;  40: CAN1
                DCD     CAN2_Handler              ;  41: CAN2
                DCD     0                         ;  42: Reserved
                DCD     HIB_Handler               ;  43: Hibernate
                DCD     USB0_Handler              ;  44: USB0
                DCD     PWM0_3_Handler            ;  45: PWM Generator 3
                DCD     UDMA_Handler              ;  46: uDMA Software Transfer
                DCD     UDMAERR_Handler           ;  47: uDMA Error
                DCD     ADC1SS0_Handler           ;  48: ADC1 Sequence 0
                DCD     ADC1SS1_Handler           ;  49: ADC1 Sequence 1
                DCD     ADC1SS2_Handler           ;  50: ADC1 Sequence 2
                DCD     ADC1SS3_Handler           ;  51: ADC1 Sequence 3
                DCD     0                         ;  52: Reserved
                DCD     0                         ;  53: Reserved
                DCD     GPIOJ_Handler             ;  54: GPIO Port J
                DCD     GPIOK_Handler             ;  55: GPIO Port K
                DCD     GPIOL_Handler             ;  56: GPIO Port L
                DCD     SSI2_Handler              ;  57: SSI2 Rx and Tx
                DCD     SSI3_Handler              ;  58: SSI3 Rx and Tx
                DCD     UART3_Handler             ;  59: UART3 Rx and Tx
                DCD     UART4_Handler             ;  60: UART4 Rx and Tx
                DCD     UART5_Handler             ;  61: UART5 Rx and Tx
                DCD     UART6_Handler             ;  62: UART6 Rx and Tx
                DCD     UART7_Handler             ;  63: UART7 Rx and Tx
                DCD     0                         ;  64: Reserved
                DCD     0                         ;  65: Reserved
                DCD     0                         ;  66: Reserved
                DCD     0                         ;  67: Reserved
                DCD     I2C2_Handler              ;  68: I2C2 Master and Slave
                DCD     I2C3_Handler              ;  69: I2C3 Master and Slave
                DCD     TIMER4A_Handler           ;  70: Timer 4 subtimer A
                DCD     TIMER4B_Handler           ;  71: Timer 4 subtimer B
                DCD     0                         ;  72: Reserved
                DCD     0                         ;  73: Reserved
                DCD     0                         ;  74: Reserved
                DCD     0                         ;  75: Reserved
                DCD     0                         ;  76: Reserved
                DCD     0                         ;  77: Reserved
                DCD     0                         ;  78: Reserved
                DCD     0                         ;  79: Reserved
                DCD     0                         ;  80: Reserved
                DCD     0                         ;  81: Reserved
                DCD     0                         ;  82: Reserved
                DCD     0                         ;  83: Reserved
                DCD     0                         ;  84: Reserved
                DCD     0                         ;  85: Reserved
                DCD     0                         ;  86: Reserved
                DCD     0                         ;  87: Reserved
                DCD     0                         ;  88: Reserved
                DCD     0                         ;  89: Reserved
                DCD     0                         ;  90: Reserved
                DCD     0                         ;  91: Reserved
                DCD     TIMER5A_Handler           ;  92: Timer 5 subtimer A
                DCD     TIMER5B_Handler           ;  93: Timer 5 subtimer B
                DCD     WTIMER0A_Handler          ;  94: Wide Timer 0 subtimer A
                DCD     WTIMER0B_Handler          ;  95: Wide Timer 0 subtimer B
                DCD     WTIMER1A_Handler          ;  96: Wide Timer 1 subtimer A
                DCD     WTIMER1B_Handler          ;  97: Wide Timer 1 subtimer B
                DCD     WTIMER2A_Handler          ;  98: Wide Timer 2 subtimer A
                DCD     WTIMER2B_Handler          ;  99: Wide Timer 2 subtimer B
                DCD     WTIMER3A_Handler          ; 100: Wide Timer 3 subtimer A
                DCD     WTIMER3B_Handler          ; 101: Wide Timer 3 subtimer B
                DCD     WTIMER4A_Handler          ; 102: Wide Timer 4 subtimer A
                DCD     WTIMER4B_Handler          ; 103: Wide Timer 4 subtimer B
                DCD     WTIMER5A_Handler          ; 104: Wide Timer 5 subtimer A
                DCD     WTIMER5B_Handler          ; 105: Wide Timer 5 subtimer B
                DCD     FPU_Handler               ; 106: FPU
                DCD     0                         ; 107: Reserved
                DCD     0                         ; 108: Reserved
                DCD     I2C4_Handler              ; 109: I2C4 Master and Slave
                DCD     I2C5_Handler              ; 110: I2C5 Master and Slave
                DCD     GPIOM_Handler             ; 111: GPIO Port M
                DCD     GPION_Handler             ; 112: GPIO Port N
                DCD     QEI2_Handler              ; 113: Quadrature Encoder 2
                DCD     0                         ; 114: Reserved
                DCD     0                         ; 115: Reserved
                DCD     GPIOP0_Handler            ; 116: GPIO Port P (Summary or P0)
                DCD     GPIOP1_Handler            ; 117: GPIO Port P1
                DCD     GPIOP2_Handler            ; 118: GPIO Port P2
                DCD     GPIOP3_Handler            ; 119: GPIO Port P3
                DCD     GPIOP4_Handler            ; 120: GPIO Port P4
                DCD     GPIOP5_Handler            ; 121: GPIO Port P5
                DCD     GPIOP6_Handler            ; 122: GPIO Port P6
                DCD     GPIOP7_Handler            ; 123: GPIO Port P7
                DCD     GPIOQ0_Handler            ; 124: GPIO Port Q (Summary or Q0)
                DCD     GPIOQ1_Handler            ; 125: GPIO Port Q1
                DCD     GPIOQ2_Handler            ; 126: GPIO Port Q2
                DCD     GPIOQ3_Handler            ; 127: GPIO Port Q3
                DCD     GPIOQ4_Handler            ; 128: GPIO Port Q4
                DCD     GPIOQ5_Handler            ; 129: GPIO Port Q5
                DCD     GPIOQ6_Handler            ; 130: GPIO Port Q6
                DCD     GPIOQ7_Handler            ; 131: GPIO Port Q7
                DCD     GPIOR_Handler             ; 132: GPIO Port R
                DCD     GPIOS_Handler             ; 133: GPIO Port S
                DCD     PMW1_0_Handler            ; 134: PWM 1 Generator 0
                DCD     PWM1_1_Handler            ; 135: PWM 1 Generator 1
                DCD     PWM1_2_Handler            ; 136: PWM 1 Generator 2
                DCD     PWM1_3_Handler            ; 137: PWM 1 Generator 3
                DCD     PWM1_FAULT_Handler        ; 138: PWM 1 Fault


				AREA    |.text|, CODE, READONLY	  	; Instructions to be placed in ROM
;-------------------------------------------------------------------------------
; Reset Handler
;-------------------------------------------------------------------------------
Reset_Handler   PROC
				IMPORT	__set_control
				IMPORT  main
				EXPORT  Reset_Handler 
				LDR 	R0, =__initial_handler_sp 	; Load the proces stack pointer
				MSR 	PSP, R0
				MOV 	R0, #0x3
				BL 		__set_control 				; Use double stacking
                LDR     R0, =main				  	; Run the main method
                BX      R0
                ENDP



;-------------------------------------------------------------------------------
; Simple exception handlers. WEAK - imported into other sources if no other  
; source exports an alternative
;-------------------------------------------------------------------------------
NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP


;-------------------------------------------------------------------------------
; Reset Handler
;-------------------------------------------------------------------------------					
SVC_Handler     PROC
				IMPORT 	svc_exc_return
				IMPORT 	SVC_Handler_C
                TST 	LR, #4
				ITE		EQ					; determine which stack was used before 
				MRSEQ	R0, MSP				; the SVC call
				MRSNE	R0, PSP
				LDR 	R1, =svc_exc_return	; save the EXC_RETURN
				STR 	LR, [R1] 
				BL 		SVC_Handler_C
				LDR 	R1, =svc_exc_return	; restore the EXC_RETURN
				LDR		LR, [R1]
				BX 		LR
                ENDP
					
					
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP


;-------------------------------------------------------------------------------
; PendSV Interrupt Handler
;
;	Registers saved by the software (PendSV_Handler):
;	+------------+
;	| EXC_RETURN | <- Saved SP (orig. SP - 72 bytes), sp in Task Control Block 
;	|  Control   |    initially points here
;	|     R8     | 
;	|     R9     |
;	|     R10    |
;	|     R11    |
;	|     R4     |
;	|     R5     |
;	|     R6     |
;	|     R7     |
;	+------------+
;
;	Exception frame saved by the NVIC hardware onto stack:
;	+------+
;	|  R0  | <- SP after entering interrupt (orig. SP - 32 bytes)
;	|  R1  |
;	|  R2  |
;	|  R3  |
;	|  R12 |
;	|  LR  |
;	|  PC  |
;	| xPSR |
;	|      | <- SP before interrupt (orig. SP), bottom of task's stack
;	+------+
;
; At context switch in addition to the registers that are save by NVIC implictly, 
; the remaining ones are also saved together with the value of CONTROL register and 
; SVC_EXC_RETURN. This is for determining whether the task saved has priviliged 
; access level (system task) or not and if the task was using FPU or not. 
;-------------------------------------------------------------------------------	
PendSV_Handler	PROC
				IMPORT 	scheduler
                EXPORT  PendSV_Handler
					
				; Save current context
				CPSID	I 					; disable Interrupts
				MRS 	R0, PSP				; get current PSP
				TST 	LR, #0x10			; check if floating point context should 
                IT 		EQ					; be saved
				VSTMDBEQ R0!, {S16-S31} 	; save floating point registers
				MOV 	R2, LR
				MRS 	R3, CONTROL
				STMDB 	R0!, {R2-R11} 		; push the LR, CONTROL and R4 to R11 registers
				LDR 	R2, =scheduler		; save the PSP into current task's metadata
				LDR 	R1, [R2]
				STR 	R0, [R1] 		

				; Load next context
				LDR 	R3, [R2, #4] 		; load the next task to run
				LDR		R0, [R3]			; load the SP of the next task
				STR		R3, [R2]			; update the runPtr
				LDMIA 	R0!, {R2-R11} 		; pop LR, CONTROL and R4 to R11 
				MOV		LR, R2
				MSR 	CONTROL, R3
				ISB
				TST 	LR, #0x10
				IT		EQ 					; Test bit 4. If zero, need to unstack 
				VLDMIAEQ R0!, {S16-S31} 	; floating point registers
				MSR 	PSP, R0 			; set PSP to next task
				CPSIE	I					; re-enable Interrupts 
				BX 		LR					; return
                ENDP
					
					
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP


;-------------------------------------------------------------------------------
; External interrupt handlers. WEAK - imported into other sources if no other  
; source exports an alternative
;-------------------------------------------------------------------------------
Default_Handler	PROC
	
				EXPORT  GPIOA_Handler [WEAK]
				EXPORT  GPIOB_Handler [WEAK]
				EXPORT  GPIOC_Handler [WEAK]	
				EXPORT  GPIOD_Handler [WEAK]
				EXPORT  GPIOE_Handler [WEAK]
				EXPORT  UART0_Handler [WEAK]
				EXPORT  UART1_Handler [WEAK]
				EXPORT  SSI0_Handler [WEAK]	
				EXPORT  I2C0_Handler [WEAK]
				EXPORT  PMW0_FAULT_Handler [WEAK]
				EXPORT  PWM0_0_Handler [WEAK]
				EXPORT  PWM0_1_Handler [WEAK]
				EXPORT  PWM0_2_Handler [WEAK]
				EXPORT  QEI0_Handler [WEAK]
				EXPORT  ADC0SS0_Handler [WEAK]	
				EXPORT  ADC0SS1_Handler [WEAK]	
				EXPORT  ADC0SS2_Handler [WEAK]
				EXPORT  ADC0SS3_Handler [WEAK]
                EXPORT  WDT0_Handler [WEAK]
                EXPORT  TIMER0A_Handler [WEAK]
                EXPORT  TIMER0B_Handler [WEAK]
                EXPORT  TIMER1A_Handler [WEAK]
                EXPORT  TIMER1B_Handler [WEAK]
                EXPORT  TIMER2A_Handler [WEAK]
                EXPORT  TIMER2B_Handler [WEAK]
                EXPORT  COMP0_Handler [WEAK]
                EXPORT  COMP1_Handler [WEAK]
                EXPORT  COMP2_Handler [WEAK]
                EXPORT  SYSCTL_Handler [WEAK]
                EXPORT  FLASH_Handler [WEAK]
                EXPORT  GPIOF_Handler [WEAK]
                EXPORT  GPIOG_Handler [WEAK]
                EXPORT  GPIOH_Handler [WEAK]
                EXPORT  UART2_Handler [WEAK]
                EXPORT  SSI1_Handler [WEAK]
                EXPORT  TIMER3A_Handler [WEAK]
                EXPORT  TIMER3B_Handler [WEAK]
                EXPORT  I2C1_Handler [WEAK]
                EXPORT  QEI1_Handler [WEAK]
                EXPORT  CAN0_Handler [WEAK]
                EXPORT  CAN1_Handler [WEAK]
                EXPORT  CAN2_Handler [WEAK]
                EXPORT  HIB_Handler [WEAK]
                EXPORT  USB0_Handler [WEAK]
                EXPORT  PWM0_3_Handler [WEAK]
                EXPORT  UDMA_Handler [WEAK]
                EXPORT  UDMAERR_Handler [WEAK]
                EXPORT  ADC1SS0_Handler [WEAK]
                EXPORT  ADC1SS1_Handler [WEAK]
                EXPORT  ADC1SS2_Handler [WEAK]
                EXPORT  ADC1SS3_Handler [WEAK]
                EXPORT  GPIOJ_Handler [WEAK]
                EXPORT  GPIOK_Handler [WEAK]
                EXPORT  GPIOL_Handler [WEAK]
                EXPORT  SSI2_Handler [WEAK]
                EXPORT  SSI3_Handler [WEAK]
                EXPORT  UART3_Handler [WEAK]
                EXPORT  UART4_Handler [WEAK]
                EXPORT  UART5_Handler [WEAK]
                EXPORT  UART6_Handler [WEAK]
                EXPORT  UART7_Handler [WEAK]
                EXPORT  I2C2_Handler [WEAK]
                EXPORT  I2C3_Handler [WEAK]
                EXPORT  TIMER4A_Handler [WEAK]
                EXPORT  TIMER4B_Handler [WEAK]
                EXPORT  TIMER5A_Handler [WEAK]
                EXPORT  TIMER5B_Handler [WEAK]
                EXPORT  WTIMER0A_Handler [WEAK]
                EXPORT  WTIMER0B_Handler [WEAK]
                EXPORT  WTIMER1A_Handler [WEAK]
                EXPORT  WTIMER1B_Handler [WEAK]
                EXPORT  WTIMER2A_Handler [WEAK]
                EXPORT  WTIMER2B_Handler [WEAK]
                EXPORT  WTIMER3A_Handler [WEAK]
                EXPORT  WTIMER3B_Handler [WEAK]
                EXPORT  WTIMER4A_Handler [WEAK]
                EXPORT  WTIMER4B_Handler [WEAK]
                EXPORT  WTIMER5A_Handler [WEAK]
                EXPORT  WTIMER5B_Handler [WEAK]
                EXPORT  FPU_Handler [WEAK]
                EXPORT  I2C4_Handler [WEAK]
                EXPORT  I2C5_Handler [WEAK]
                EXPORT  GPIOM_Handler [WEAK]
                EXPORT  GPION_Handler [WEAK]
                EXPORT  QEI2_Handler [WEAK]
                EXPORT  GPIOP0_Handler [WEAK]
                EXPORT  GPIOP1_Handler [WEAK]
                EXPORT  GPIOP2_Handler [WEAK]
                EXPORT  GPIOP3_Handler [WEAK]
                EXPORT  GPIOP4_Handler [WEAK]
                EXPORT  GPIOP5_Handler [WEAK]
                EXPORT  GPIOP6_Handler [WEAK]
                EXPORT  GPIOP7_Handler [WEAK]
                EXPORT  GPIOQ0_Handler [WEAK]
                EXPORT  GPIOQ1_Handler [WEAK]
                EXPORT  GPIOQ2_Handler [WEAK]
                EXPORT  GPIOQ3_Handler [WEAK]
                EXPORT  GPIOQ4_Handler [WEAK]
                EXPORT  GPIOQ5_Handler [WEAK]
                EXPORT  GPIOQ6_Handler [WEAK]
                EXPORT  GPIOQ7_Handler [WEAK]
                EXPORT  GPIOR_Handler [WEAK]
                EXPORT  GPIOS_Handler [WEAK]
                EXPORT  PMW1_0_Handler [WEAK]
                EXPORT  PWM1_1_Handler [WEAK]
                EXPORT  PWM1_2_Handler [WEAK]
                EXPORT  PWM1_3_Handler [WEAK]
                EXPORT  PWM1_FAULT_Handler [WEAK]
 

GPIOA_Handler 
GPIOB_Handler 
GPIOC_Handler 	
GPIOD_Handler 
GPIOE_Handler 
UART0_Handler 
UART1_Handler 
SSI0_Handler 	
I2C0_Handler 
PMW0_FAULT_Handler 
PWM0_0_Handler 
PWM0_1_Handler 
PWM0_2_Handler 
QEI0_Handler 
ADC0SS0_Handler 	
ADC0SS1_Handler 	
ADC0SS2_Handler 
ADC0SS3_Handler
WDT0_Handler 
TIMER0A_Handler 
TIMER0B_Handler 
TIMER1A_Handler 
TIMER1B_Handler 
TIMER2A_Handler 
TIMER2B_Handler 
COMP0_Handler 
COMP1_Handler 
COMP2_Handler 
SYSCTL_Handler 
FLASH_Handler 
GPIOF_Handler 
GPIOG_Handler 
GPIOH_Handler 
UART2_Handler 
SSI1_Handler 
TIMER3A_Handler 
TIMER3B_Handler 
I2C1_Handler 
QEI1_Handler 
CAN0_Handler 
CAN1_Handler 
CAN2_Handler 
HIB_Handler 
USB0_Handler 
PWM0_3_Handler 
UDMA_Handler 
UDMAERR_Handler 
ADC1SS0_Handler 
ADC1SS1_Handler 
ADC1SS2_Handler 
ADC1SS3_Handler 
GPIOJ_Handler 
GPIOK_Handler 
GPIOL_Handler 
SSI2_Handler 
SSI3_Handler 
UART3_Handler 
UART4_Handler 
UART5_Handler 
UART6_Handler 
UART7_Handler 
I2C2_Handler 
I2C3_Handler 
TIMER4A_Handler 
TIMER4B_Handler 
TIMER5A_Handler 
TIMER5B_Handler 
WTIMER0A_Handler 
WTIMER0B_Handler 
WTIMER1A_Handler 
WTIMER1B_Handler 
WTIMER2A_Handler 
WTIMER2B_Handler 
WTIMER3A_Handler 
WTIMER3B_Handler 
WTIMER4A_Handler 
WTIMER4B_Handler 
WTIMER5A_Handler 
WTIMER5B_Handler 
FPU_Handler 
I2C4_Handler 
I2C5_Handler 
GPIOM_Handler 
GPION_Handler 
QEI2_Handler 
GPIOP0_Handler 
GPIOP1_Handler 
GPIOP2_Handler 
GPIOP3_Handler 
GPIOP4_Handler 
GPIOP5_Handler 
GPIOP6_Handler 
GPIOP7_Handler 
GPIOQ0_Handler 
GPIOQ1_Handler 
GPIOQ2_Handler 
GPIOQ3_Handler 
GPIOQ4_Handler 
GPIOQ5_Handler 
GPIOQ6_Handler 
GPIOQ7_Handler 
GPIOR_Handler 
GPIOS_Handler 
PMW1_0_Handler 
PWM1_1_Handler 
PWM1_2_Handler 
PWM1_3_Handler 
PWM1_FAULT_Handler 
				B       .
                ENDP
					
					
;-------------------------------------------------------------------------------
; Function:    	__user_initial_stackheap
; Purpose:    	Returns the locations of the initial stack and heap.
; Arguments:	-
; Returns: 		
;		R0 - heap base address
; 		R1 - stack limit address
;		R2 - heap limit address
; 		R3 - stack base address
;-------------------------------------------------------------------------------	
				IMPORT __use_two_region_memory
				EXPORT 	__user_initial_stackheap
__user_initial_stackheap\
				PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, = (Handler_Stack + Handler_Stack_Size)
                LDR     R2, = (Heap_Mem + Heap_Size)
                LDR     R3, = Handler_Stack
                BX      LR
				ENDP				
					
				ALIGN
				END
				