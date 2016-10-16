/*******************************************************************************
* File:     	nvic.h
* Brief:    	Header file for nvic.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	10/10/2016
* Last mod: 	10/10/2016
*
* Note: 		Contains interrupt numbers for each interrupt source
*******************************************************************************/



/*-------------------------------------------------------------------------------
* Interrupt Number Definition 
--------------------------------------------------------------------------------*/
typedef enum {
	// Cortex-M4 Processor Exceptions Numbers 
	Reset_IRQn 				= -15,          
	NonMaskableInt_IRQn 	= -14,          
	HardFault_IRQn          = -13,          
	MemoryManagement_IRQn   = -12,          									                                                        
	BusFault_IRQn           = -11,          
	UsageFault_IRQn         = -10,          
	SVCall_IRQn             =  -5,          
	DebugMonitor_IRQn       =  -4,          
	PendSV_IRQn             =  -2,          
	SysTick_IRQn            =  -1,          
	
	// TM4C123GH6PM Specific Interrupt Numbers  
	GPIOA_IRQn              =   0,                                                                   
	GPIOB_IRQn              =   1,          
	GPIOC_IRQn              =   2,          
	GPIOD_IRQn              =   3,          
	GPIOE_IRQn              =   4,          
	UART0_IRQn              =   5,                                                                     
	UART1_IRQn              =   6,          
	SSI0_IRQn               =   7,                                                                       
	I2C0_IRQn               =   8,                                                                       
	PWM0_FAULT_IRQn         =   9,                                                                 
	PWM0_0_IRQn             =  10,                                                                    
	PWM0_1_IRQn             =  11,                                                                     
	PWM0_2_IRQn             =  12,                                                                    
	QEI0_IRQn               =  13,                                                                       
	ADC0SS0_IRQn            =  14,                                                                    
	ADC0SS1_IRQn            =  15,                                                                   
	ADC0SS2_IRQn            =  16,                                                                    
	ADC0SS3_IRQn            =  17,                                                                    
	WATCHDOG0_IRQn          =  18,                                                               
	TIMER0A_IRQn            =  19,                                                                   
	TIMER0B_IRQn            =  20,                                                                    
	TIMER1A_IRQn            =  21,          
	TIMER1B_IRQn            =  22,          
	TIMER2A_IRQn            =  23,          
	TIMER2B_IRQn            =  24,          
	COMP0_IRQn              =  25,                                                                     
	COMP1_IRQn              =  26,          
	SYSCTL_IRQn             =  28,          
	FLASH_CTRL_IRQn         =  29,                                                                
	GPIOF_IRQn              =  30,                                                                      
	UART2_IRQn              =  33,          
	SSI1_IRQn               =  34,          
	TIMER3A_IRQn            =  35,          
	TIMER3B_IRQn            =  36,          
	I2C1_IRQn               =  37,          
	QEI1_IRQn               =  38,          
	CAN0_IRQn               =  39,                                                                    
	CAN1_IRQn               =  40,                                                                      
	HIB_IRQn                =  43,          
	USB0_IRQn               =  44,                                                                    
	PWM0_3_IRQn             =  45,                                                                    
	UDMA_IRQn               =  46,          
	UDMAERR_IRQn            =  47,          
	ADC1SS0_IRQn            =  48,                                                                  
	ADC1SS1_IRQn            =  49,          
	ADC1SS2_IRQn            =  50,                                                                  
	ADC1SS3_IRQn            =  51,          
	SSI2_IRQn               =  57,          
	SSI3_IRQn               =  58,          
	UART3_IRQn              =  59,          
	UART4_IRQn              =  60,                                                                    
	UART5_IRQn              =  61,          
	UART6_IRQn              =  62,          
	UART7_IRQn              =  63,          
	I2C2_IRQn               =  68,          
	I2C3_IRQn               =  69,          
	TIMER4A_IRQn            =  70,                                                             
	TIMER4B_IRQn            =  71,          
	TIMER5A_IRQn            =  92,          
	TIMER5B_IRQn            =  93,          
	WTIMER0A_IRQn           =  94,                                                    
	WTIMER0B_IRQn           =  95,                                                
	WTIMER1A_IRQn           =  96,          
	WTIMER1B_IRQn           =  97,          
	WTIMER2A_IRQn           =  98,          
	WTIMER2B_IRQn           =  99,          
	WTIMER3A_IRQn           = 100,                                                               
	WTIMER3B_IRQn           = 101,                                               
	WTIMER4A_IRQn           = 102,                                                    
	WTIMER4B_IRQn           = 103,                                    
	WTIMER5A_IRQn           = 104,                                 
	WTIMER5B_IRQn           = 105,                         
	SYSEXC_IRQn             = 106,                                                       
	PWM1_0_IRQn             = 134,                                      
	PWM1_1_IRQn             = 135,                          
	PWM1_2_IRQn             = 136,                                                                     
	PWM1_3_IRQn             = 137,                                                                     
	PWM1_FAULT_IRQn         = 138                                                                 
} IRQn_Type;


/*-------------------------------------------------------------------------------
* Function:    	nvic_enable_irq
* Purpose:    	Enable interrupt source
* Arguments: 	
*		irq - interrupt source to enable
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nvic_enable_irq(IRQn_Type irq);


/*-------------------------------------------------------------------------------
* Function:    	nvic_disable_irq
* Purpose:    	Disable interrupt source
* Arguments: 	
*		irq - interrupt source to disable
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nvic_disable_irq(IRQn_Type irq);


/*-------------------------------------------------------------------------------
* Function:    	nvic_set_pending
* Purpose:    	Set Pending Interrupt
* Arguments: 	
*		irq - interrupt source set pending
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nvic_set_pending(IRQn_Type irq);


/*-------------------------------------------------------------------------------
* Function:    	nvic_clear_pending
* Purpose:    	Clear Pending Interrupt
* Arguments: 	
*		irq - interrupt source clear
* Returns: 		-	
--------------------------------------------------------------------------------*/
void nvic_clear_pending(IRQn_Type irq);

