/*******************************************************************************
* File:     	tm4c123gh6pm.h
* Brief:    	External register definitions for tm4c123gh6pm microcontroller 
* Author: 		Krzysztof 
* Version:		V1.
* Date created:	30/09/
* Last mod: 	09/03/
*
* Note: 		
*	Essential register addresses and bit offsets for important on-chip modules 
*	and external peripherals. Currently this file includes definitions which are
*	used either by the KrisOS or the demo user programs:	
*		1. Interrupt numbers 
*		2. System Control Block and System 
*		3. Nested Vectored Interrupt 
*		4. SysTick 
*		5. General Purpose Input/Output 
*		6. Universal Asynchronous Receivers/
*		7. Synchronous Serial Interface 
*		8. Inter-Integrated Circuit interface 
*		9. Analog-to-Digital 
*		10. Pulse Width 
*	
*******************************************************************************/
#include "common.h"
#ifndef TM4C123GH6PM_H					// Include barrier
#define TM4C123GH6PM_H



/*-------------------------------------------------------------------------------
* Allow anonymous unions 
*------------------------------------------------------------------------------*/
#pragma anon_unions



/*-------------------------------------------------------------------------------
* Register types in terms of read/write 
*------------------------------------------------------------------------------*/
#define __I volatile const 				// input, read-
#define __O volatile 					// output, write-
#define __IO volatile 					// input-output, read and write 
	


/*-------------------------------------------------------------------------------
* Interrupt Numbers
--------------------------------------------------------------------------------*/
// Enum type so that less input argument checking is needed when accessing NVIC
// registers for setting priorities, clearing/setting pending bits etc...
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
* System Control Block registers
*------------------------------------------------------------------------------*/
typedef struct
{
  __I  uint32_t CPUID;                  // CPUID Base Register 
  __IO uint32_t ICSR;                   // Interrupt Control and State Register 
  __IO uint32_t VTOR;                   // Vector Table Offset Register 
  __IO uint32_t AIRCR;                  // Application Interrupt and Reset Control Register 
  __IO uint32_t SCR;                    // System Control Register 
  __IO uint32_t CCR;                    // Configuration Control Register 
  __IO uint8_t  SHP[12U];               // System Handlers Priority Registers (4-7, 8-11, 12-15) 
  __IO uint32_t SHCSR;                  // System Handler Control and State Register 
  __IO uint32_t CFSR;                   // Configurable Fault Status Register 
  __IO uint32_t HFSR;                   // HardFault Status Register 
  __IO uint32_t DFSR;                   // Debug Fault Status Register 
  __IO uint32_t MMFAR;                  // MemManage Fault Address Register 
  __IO uint32_t BFAR;                   // BusFault Address Register 
  __IO uint32_t AFSR;                   // Auxiliary Fault Status Register 
  __I  uint32_t PFR[2U];                // Processor Feature Register 
  __I  uint32_t DFR;                    // Debug Feature Register 
  __I  uint32_t ADR;                    // Auxiliary Feature Register 
  __I  uint32_t MMFR[4U];               // Memory Model Feature Register 
  __I  uint32_t ISAR[5U];               // Instruction Set Attributes Register 
       uint32_t RESERVED0[5U];
  __IO uint32_t CPACR;                  // Coprocessor Access Control Register 
} SCB_Type;
#define SCB_Base 0xE000ED00
#define SCB ((SCB_Type*) SCB_Base)

// ICSR register 
#define NMISET 31 					// NMI Set Pending
#define PENDSV 28 					// PendSV Set Pending
#define UNPENDSV 27 				// PendSV Clear Pending
#define PENDSTSET 26 				// SysTick Set Pending
#define PENDSTCLR 25 				// SysTick Clear Pending
#define ISRPRE 23 					// Debug Inrerrupt Handling
#define ISRPEND 22 						// Interrupt Pending
#define VECPEND 12 					// Interrupt Pending Vector Number
#define RETBASE 11 					// Return to Base
#define VECACT 0					// Interrupt Active Vector Number



/*-------------------------------------------------------------------------------
* System control registers
*------------------------------------------------------------------------------*/
typedef struct {                          // SYSCTL Structure 
	__IO uint32_t  DID0;                  // Device Identification 0                                               
	__IO uint32_t  DID1;                  // Device Identification 1                                               
	__IO uint32_t  DC0;                   // Device Capabilities 0                                                 
	__I  uint32_t  RESERVED;
	__IO uint32_t  DC1;                   // Device Capabilities 1                                                 
	__IO uint32_t  DC2;                   // Device Capabilities 2                                                 
	__IO uint32_t  DC3;                   // Device Capabilities 3                                                 
	__IO uint32_t  DC4;                   // Device Capabilities 4                                                 
	__IO uint32_t  DC5;                   // Device Capabilities 5                                                 
	__IO uint32_t  DC6;                   // Device Capabilities 6                                                 
	__IO uint32_t  DC7;                   // Device Capabilities 7                                                 
	__IO uint32_t  DC8;                   // Device Capabilities 8                                                 
	__IO uint32_t  PBORCTL;               // Brown-Out Reset Control                                               
	__I  uint32_t  RESERVED1[3];
	__IO uint32_t  SRCR0;                 // Software Reset Control 0                                              
	__IO uint32_t  SRCR1;                 // Software Reset Control 1                                              
	__IO uint32_t  SRCR2;                 // Software Reset Control 2                                              
	__I  uint32_t  RESERVED2;
	__IO uint32_t  RIS;                   // Raw Interrupt Status                                                  
	__IO uint32_t  IMC;                   // Interrupt Mask Control                                                
	__IO uint32_t  MISC;                  // Masked Interrupt Status and Clear                                     
	__IO uint32_t  RESC;                  // Reset Cause                                                           
	__IO uint32_t  RCC;                   // Run-Mode Clock Configuration                                          
	__I  uint32_t  RESERVED3[2];
	__IO uint32_t  GPIOHBCTL;             // GPIO High-Performance Bus Control                                     
	__IO uint32_t  RCC2;                  // Run-Mode Clock Configuration 2                                        
	__I  uint32_t  RESERVED4[2];
	__IO uint32_t  MOSCCTL;               // Main Oscillator Control                                               
	__I  uint32_t  RESERVED5[32];
	__IO uint32_t  RCGC0;                 // Run Mode Clock Gating Control Register 0                              
	__IO uint32_t  RCGC1;                 // Run Mode Clock Gating Control Register 1                              
	__IO uint32_t  RCGC2;                 // Run Mode Clock Gating Control Register 2                              
	__I  uint32_t  RESERVED6;
	__IO uint32_t  SCGC0;                 // Sleep Mode Clock Gating Control Register 0                            
	__IO uint32_t  SCGC1;                 // Sleep Mode Clock Gating Control Register 1                            
	__IO uint32_t  SCGC2;                 // Sleep Mode Clock Gating Control Register 2                            
	__I  uint32_t  RESERVED7;
	__IO uint32_t  DCGC0;                 // Deep Sleep Mode Clock Gating Control Register 0                       
	__IO uint32_t  DCGC1;                 // Deep-Sleep Mode Clock Gating Control Register 1                       
	__IO uint32_t  DCGC2;                 // Deep Sleep Mode Clock Gating Control Register 2                       
	__I  uint32_t  RESERVED8[6];
	__IO uint32_t  DSLPCLKCFG;            // Deep Sleep Clock Configuration                                        
	__I  uint32_t  RESERVED9;
	__IO uint32_t  SYSPROP;               // System Properties                                                     
	__IO uint32_t  PIOSCCAL;              // Precision Internal Oscillator Calibration                             
	__IO uint32_t  PIOSCSTAT;             // Precision Internal Oscillator Statistics                              
	__I  uint32_t  RESERVED10[2];
	__IO uint32_t  PLLFREQ0;              // PLL Frequency 0                                                       
	__IO uint32_t  PLLFREQ1;              // PLL Frequency 1                                                       
	__IO uint32_t  PLLSTAT;               // PLL Status                                                            
	__I  uint32_t  RESERVED11[7];
	__IO uint32_t  SLPPWRCFG;             // Sleep Power Configuration                                             
	__IO uint32_t  DSLPPWRCFG;            // Deep-Sleep Power Configuration                                        
	__IO uint32_t  DC9;                   // Device Capabilities 9                                                 
	__I  uint32_t  RESERVED12[3];
	__IO uint32_t  NVMSTAT;               // Non-Volatile Memory Information                                       
	__I  uint32_t  RESERVED13[4];
	__IO uint32_t  LDOSPCTL;              // LDO Sleep Power Control                                               
	__I  uint32_t  RESERVED14;
	__IO uint32_t  LDODPCTL;              // LDO Deep-Sleep Power Control                                          
	__I  uint32_t  RESERVED15[80];
	__IO uint32_t  PPWD;                  // Watchdog Timer Peripheral Present                                     
	__IO uint32_t  PPTIMER;               // 16/32-Bit General-Purpose Timer Peripheral Present                    
	__IO uint32_t  PPGPIO;                // General-Purpose Input/Output Peripheral Present                       
	__IO uint32_t  PPDMA;                 // Micro Direct Memory Access Peripheral Present                         
	__I  uint32_t  RESERVED16;
	__IO uint32_t  PPHIB;                 // Hibernation Peripheral Present                                        
	__IO uint32_t  PPUART;                // Universal Asynchronous Receiver/Transmitter Peripheral Present        
	__IO uint32_t  PPSSI;                 // Synchronous Serial Interface Peripheral Present                       
	__IO uint32_t  PPI2C;                 // Inter-Integrated Circuit Peripheral Present                           
	__I  uint32_t  RESERVED17;
	__IO uint32_t  PPUSB;                 // Universal Serial Bus Peripheral Present                               
	__I  uint32_t  RESERVED18[2];
	__IO uint32_t  PPCAN;                 // Controller Area Network Peripheral Present                            
	__IO uint32_t  PPADC;                 // Analog-to-Digital Converter Peripheral Present                        
	__IO uint32_t  PPACMP;                // Analog Comparator Peripheral Present                                  
	__IO uint32_t  PPPWM;                 // Pulse Width Modulator Peripheral Present                              
	__IO uint32_t  PPQEI;                 // Quadrature Encoder Interface Peripheral Present                       
	__I  uint32_t  RESERVED19[4];
	__IO uint32_t  PPEEPROM;              // EEPROM Peripheral Present                                             
	__IO uint32_t  PPWTIMER;              // 32/64-Bit Wide General-Purpose Timer Peripheral Present               
	__I  uint32_t  RESERVED20[104];
	__IO uint32_t  SRWD;                  // Watchdog Timer Software Reset                                         
	__IO uint32_t  SRTIMER;               // 16/32-Bit General-Purpose Timer Software Reset                        
	__IO uint32_t  SRGPIO;                // General-Purpose Input/Output Software Reset                           
	__IO uint32_t  SRDMA;                 // Micro Direct Memory Access Software Reset                             
	__I  uint32_t  RESERVED21;
	__IO uint32_t  SRHIB;                 // Hibernation Software Reset                                            
	__IO uint32_t  SRUART;                // Universal Asynchronous Receiver/Transmitter Software Reset            
	__IO uint32_t  SRSSI;                 // Synchronous Serial Interface Software Reset                           
	__IO uint32_t  SRI2C;                 // Inter-Integrated Circuit Software Reset                               
	__I  uint32_t  RESERVED22;
	__IO uint32_t  SRUSB;                 // Universal Serial Bus Software Reset                                   
	__I  uint32_t  RESERVED23[2];
	__IO uint32_t  SRCAN;                 // Controller Area Network Software Reset                                
	__IO uint32_t  SRADC;                 // Analog-to-Digital Converter Software Reset                            
	__IO uint32_t  SRACMP;                // Analog Comparator Software Reset                                      
	__IO uint32_t  SRPWM;                 // Pulse Width Modulator Software Reset                                  
	__IO uint32_t  SRQEI;                 // Quadrature Encoder Interface Software Reset                           
	__I  uint32_t  RESERVED24[4];
	__IO uint32_t  SREEPROM;              // EEPROM Software Reset                                                 
	__IO uint32_t  SRWTIMER;              // 32/64-Bit Wide General-Purpose Timer Software Reset                   
	__I  uint32_t  RESERVED25[40];
	__IO uint32_t  RCGCWD;                // Watchdog Timer Run Mode Clock Gating Control                          
	__IO uint32_t  RCGCTIMER;             // 16/32-Bit General-Purpose Timer Run Mode Clock Gating Control         
	__IO uint32_t  RCGCGPIO;              // General-Purpose Input/Output Run Mode Clock Gating Control            
	__IO uint32_t  RCGCDMA;               // Micro Direct Memory Access Run Mode Clock Gating Control              
	__I  uint32_t  RESERVED26;
	__IO uint32_t  RCGCHIB;               // Hibernation Run Mode Clock Gating Control                             
	__IO uint32_t  RCGCUART;              // Universal Asynchronous Receiver/Transmitter Run Mode Clock Gating Control
	__IO uint32_t  RCGCSSI;               // Synchronous Serial Interface Run Mode Clock Gating Control            
	__IO uint32_t  RCGCI2C;               // Inter-Integrated Circuit Run Mode Clock Gating Control                
	__I  uint32_t  RESERVED27;
	__IO uint32_t  RCGCUSB;               // Universal Serial Bus Run Mode Clock Gating Control                    
	__I  uint32_t  RESERVED28[2];
	__IO uint32_t  RCGCCAN;               // Controller Area Network Run Mode Clock Gating Control                 
	__IO uint32_t  RCGCADC;               // Analog-to-Digital Converter Run Mode Clock Gating Control             
	__IO uint32_t  RCGCACMP;              // Analog Comparator Run Mode Clock Gating Control                       
	__IO uint32_t  RCGCPWM;               // Pulse Width Modulator Run Mode Clock Gating Control                   
	__IO uint32_t  RCGCQEI;               // Quadrature Encoder Interface Run Mode Clock Gating Control            
	__I  uint32_t  RESERVED29[4];
	__IO uint32_t  RCGCEEPROM;            // EEPROM Run Mode Clock Gating Control                                  
	__IO uint32_t  RCGCWTIMER;            // 32/64-Bit Wide General-Purpose Timer Run Mode Clock Gating Control    
	__I  uint32_t  RESERVED30[40];
	__IO uint32_t  SCGCWD;                // Watchdog Timer Sleep Mode Clock Gating Control                        
	__IO uint32_t  SCGCTIMER;             // 16/32-Bit General-Purpose Timer Sleep Mode Clock Gating Control       
	__IO uint32_t  SCGCGPIO;              // General-Purpose Input/Output Sleep Mode Clock Gating Control          
	__IO uint32_t  SCGCDMA;               // Micro Direct Memory Access Sleep Mode Clock Gating Control            
	__I  uint32_t  RESERVED31;
	__IO uint32_t  SCGCHIB;               // Hibernation Sleep Mode Clock Gating Control                           
	__IO uint32_t  SCGCUART;              // Universal Asynchronous Receiver/Transmitter Sleep Mode Clock Gating Control
	__IO uint32_t  SCGCSSI;               // Synchronous Serial Interface Sleep Mode Clock Gating Control          
	__IO uint32_t  SCGCI2C;               // Inter-Integrated Circuit Sleep Mode Clock Gating Control              
	__I  uint32_t  RESERVED32;
	__IO uint32_t  SCGCUSB;               // Universal Serial Bus Sleep Mode Clock Gating Control                  
	__I  uint32_t  RESERVED33[2];
	__IO uint32_t  SCGCCAN;               // Controller Area Network Sleep Mode Clock Gating Control               
	__IO uint32_t  SCGCADC;               // Analog-to-Digital Converter Sleep Mode Clock Gating Control           
	__IO uint32_t  SCGCACMP;              // Analog Comparator Sleep Mode Clock Gating Control                     
	__IO uint32_t  SCGCPWM;               // Pulse Width Modulator Sleep Mode Clock Gating Control                 
	__IO uint32_t  SCGCQEI;               // Quadrature Encoder Interface Sleep Mode Clock Gating Control          
	__I  uint32_t  RESERVED34[4];
	__IO uint32_t  SCGCEEPROM;            // EEPROM Sleep Mode Clock Gating Control                                
	__IO uint32_t  SCGCWTIMER;            // 32/64-Bit Wide General-Purpose Timer Sleep Mode Clock Gating Control  
	__I  uint32_t  RESERVED35[40];
	__IO uint32_t  DCGCWD;                // Watchdog Timer Deep-Sleep Mode Clock Gating Control                   
	__IO uint32_t  DCGCTIMER;             // 16/32-Bit General-Purpose Timer Deep-Sleep Mode Clock Gating Control 
	__IO uint32_t  DCGCGPIO;              // General-Purpose Input/Output Deep-Sleep Mode Clock Gating Control     
	__IO uint32_t  DCGCDMA;               // Micro Direct Memory Access Deep-Sleep Mode Clock Gating Control       
	__I  uint32_t  RESERVED36;
	__IO uint32_t  DCGCHIB;               // Hibernation Deep-Sleep Mode Clock Gating Control                      
	__IO uint32_t  DCGCUART;              // Universal Asynchronous Receiver/Transmitter Deep-Sleep Mode Clock Gating Control 
	__IO uint32_t  DCGCSSI;               // Synchronous Serial Interface Deep-Sleep Mode Clock Gating Control     
	__IO uint32_t  DCGCI2C;               // Inter-Integrated Circuit Deep-Sleep Mode Clock Gating Control         
	__I  uint32_t  RESERVED37;
	__IO uint32_t  DCGCUSB;               // Universal Serial Bus Deep-Sleep Mode Clock Gating Control             
	__I  uint32_t  RESERVED38[2];
	__IO uint32_t  DCGCCAN;               // Controller Area Network Deep-Sleep Mode Clock Gating Control          
	__IO uint32_t  DCGCADC;               // Analog-to-Digital Converter Deep-Sleep Mode Clock Gating Control      
	__IO uint32_t  DCGCACMP;              // Analog Comparator Deep-Sleep Mode Clock Gating Control                
	__IO uint32_t  DCGCPWM;               // Pulse Width Modulator Deep-Sleep Mode Clock Gating Control            
	__IO uint32_t  DCGCQEI;               // Quadrature Encoder Interface Deep-Sleep Mode Clock Gating Control     
	__I  uint32_t  RESERVED39[4];
	__IO uint32_t  DCGCEEPROM;            // EEPROM Deep-Sleep Mode Clock Gating Control                           
	__IO uint32_t  DCGCWTIMER;            // 32/64-Bit Wide General-Purpose Timer Deep-Sleep Mode Clock Gating Control 
	__I  uint32_t  RESERVED40[104];
	__IO uint32_t  PRWD;                  // Watchdog Timer Peripheral Ready                                       
	__IO uint32_t  PRTIMER;               // 16/32-Bit General-Purpose Timer Peripheral Ready                      
	__IO uint32_t  PRGPIO;                // General-Purpose Input/Output Peripheral Ready                         
	__IO uint32_t  PRDMA;                 // Micro Direct Memory Access Peripheral Ready                           
	__I  uint32_t  RESERVED41;
	__IO uint32_t  PRHIB;                 // Hibernation Peripheral Ready                                          
	__IO uint32_t  PRUART;                // Universal Asynchronous Receiver/Transmitter Peripheral Ready          
	__IO uint32_t  PRSSI;                 // Synchronous Serial Interface Peripheral Ready                         
	__IO uint32_t  PRI2C;                 // Inter-Integrated Circuit Peripheral Ready                             
	__I  uint32_t  RESERVED42;
	__IO uint32_t  PRUSB;                 // Universal Serial Bus Peripheral Ready                                 
	__I  uint32_t  RESERVED43[2];
	__IO uint32_t  PRCAN;                 // Controller Area Network Peripheral Ready                              
	__IO uint32_t  PRADC;                 // Analog-to-Digital Converter Peripheral Ready                          
	__IO uint32_t  PRACMP;                // Analog Comparator Peripheral Ready                                    
	__IO uint32_t  PRPWM;                 // Pulse Width Modulator Peripheral Ready                                
	__IO uint32_t  PRQEI;                 // Quadrature Encoder Interface Peripheral Ready                         
	__I  uint32_t  RESERVED44[4];
	__IO uint32_t  PREEPROM;              // EEPROM Peripheral Ready                                               
	__IO uint32_t  PRWTIMER;              // 32/64-Bit Wide General-Purpose Timer Peripheral Ready                 
} SYSCTL_Type;

#define SYSCTL_Base 0x400FE000
#define SYSCTL ((SYSCTL_Type*) SYSCTL_Base)

// RCGCUART Universal Asynchronous Receiver/Transmitter Run mode clock gating control
#define RCGC_UART0 0
#define RCGC_UART1 1
#define RCGC_UART2 2
#define RCGC_UART3 3
#define RCGC_UART4 4
#define RCGC_UART5 5
#define RCGC_UART6 6
#define RCGC_UART7 7

// RCGCSSI Synchronous Serial Interfface Run mode clock gating control
#define RCGC_SSI0 0
#define RCGC_SSI1 1
#define RCGC_SSI2 2
#define RCGC_SSI3 3

// RCGCI2C Inter-Integrated Circuit Run mode clock gating control
#define RCGC_I2C0 0
#define RCGC_I2C1 1
#define RCGC_I2C2 2
#define RCGC_I2C3 3

// RCGCADC Analog-to-Digital Converter Run mode clock gating control
#define RCGC_ADC0 0
#define RCGC_ADC1 1

// RCGCPWM Pulse Width Modulator Run mode clock gating control
#define RCGC_PWM0 0
#define RCGC_PWM1 1

// RCGCGPIO register 
#define RCGCGPIO_PORTA 0
#define RCGCGPIO_PORTB 1
#define RCGCGPIO_PORTC 2
#define RCGCGPIO_PORTD 3
#define RCGCGPIO_PORTE 4
#define RCGCGPIO_PORTF 5

// RCC register 
#define RCC_ACG 27
#define RCC_SYSDIV 23
#define RCC_USESYSDIV 22
#define RCC_USEPWMDIV 20
#define RCC_PWMDIV 17
#define RCC_PWRDN 13
#define RCC_BYPASS 11
#define RCC_XTAL 6
#define RCC_OSCSRC 4
#define RCC_MOSCDIS 0

// RCC2 register 
#define USERCC2 31
#define DIV400 30
#define SYSDIV2 23
#define SYSDIV2LSB 22
#define USBPWRDN 14
#define PWRDN2 13
#define BYPASS2 11
#define OSCSRC2 4

// RIS register 
#define PLLRIS 6



/*-------------------------------------------------------------------------------
* NVIC registers
*------------------------------------------------------------------------------*/
typedef struct
{
	__IO uint32_t ISER[8];               	// Interrupt Set Enable Register 
	__I  uint32_t RESERVED0[24];
	__IO uint32_t ICER[8];               	// Interrupt Clear Enable Register 
	__I  uint32_t RESERVED1[24];
	__IO uint32_t ISPR[8];               	// Interrupt Set Pending Register 
	__I  uint32_t RESERVED2[24];
	__IO uint32_t ICPR[8];               	// Interrupt Clear Pending Register 
	__I  uint32_t RESERVED3[24];
	__IO uint32_t IABR[8];               	// Interrupt Active bit Register 
	__I  uint32_t RESERVED4[56];
	__IO uint8_t  IP[240];               	// Interrupt Priority Register (8Bit wide) 
	__I  uint32_t RESERVED5[644];
	__O  uint32_t STIR;                   	// Software Trigger Interrupt Register 
}  NVIC_Type;

#define NVIC_Base 0xE000E100
#define NVIC ((NVIC_Type*) NVIC_Base)



/*-------------------------------------------------------------------------------
* SysTick timer registers
*------------------------------------------------------------------------------*/
typedef struct
{
	__IO uint32_t CTRL;                  	// SysTick Control and Status Register
	__IO uint32_t RELOAD;                   // SysTick Reload Value Register
	__IO uint32_t CURRENT;					// SysTick Current Value Register
} SYSTICK_Type;

#define SYSTICK_Base 0xE000E010
#define SYSTICK ((SYSTICK_Type*) SYSTICK_Base)

// CTRL register 
#define CTRL_COUNT 16
#define CTRL_CLK_SRC 2
#define CTRL_INTEN 1
#define CTRL_ENABLE 0



/*-------------------------------------------------------------------------------
* GPIO registers
*------------------------------------------------------------------------------*/
typedef struct {                       // GPIOA Structure                                                       
	__I  uint32_t  RESERVED[255];
	__IO uint32_t  DATA;                 // GPIO Data                                                             
	__IO uint32_t  DIR;                  // GPIO Direction                                                        
	__IO uint32_t  IS;                   // GPIO Interrupt Sense                                                  
	__IO uint32_t  IBE;                  // GPIO Interrupt Both Edges                                             
	__IO uint32_t  IEV;                  // GPIO Interrupt Event                                                  
	__IO uint32_t  IM;                   // GPIO Interrupt Mask                                                   
	__IO uint32_t  RIS;                  // GPIO Raw Interrupt Status                                             
	__IO uint32_t  MIS;                  // GPIO Masked Interrupt Status                                          
	__O  uint32_t  ICR;                  // GPIO Interrupt Clear                                                  
	__IO uint32_t  AFSEL;                // GPIO Alternate Function Select                                        
	__I  uint32_t  RESERVED1[55];
	__IO uint32_t  DR2R;                 // GPIO 2-mA Drive Select                                                
	__IO uint32_t  DR4R;                 // GPIO 4-mA Drive Select                                                
	__IO uint32_t  DR8R;                 // GPIO 8-mA Drive Select                                                
	__IO uint32_t  ODR;                  // GPIO Open Drain Select                                                
	__IO uint32_t  PUR;                  // GPIO Pull-Up Select                                                   
	__IO uint32_t  PDR;                  // GPIO Pull-Down Select                                                 
	__IO uint32_t  SLR;                  // GPIO Slew Rate Control Select                                         
	__IO uint32_t  DEN;                  // GPIO Digital Enable                                                   
	__IO uint32_t  LOCK;                 // GPIO Lock                                                             
	__IO uint32_t  CR;                   // GPIO Commit                                                           
	__IO uint32_t  AMSEL;                // GPIO Analog Mode Select                                               
	__IO uint32_t  PCTL;                 // GPIO Port Control                                                     
	__IO uint32_t  ADCCTL;               // GPIO ADC Control                                                      
	__IO uint32_t  DMACTL;               // GPIO DMA Control                                                      
} GPIO_Type;

#define GPIOA_Base 0x40004000
#define GPIOB_Base 0x40005000
#define GPIOC_Base 0x40006000
#define GPIOD_Base 0x40007000
#define GPIOE_Base 0x40024000
#define GPIOF_Base 0x40025000
#define GPIOA ((GPIO_Type*) GPIOA_Base)
#define GPIOB ((GPIO_Type*) GPIOB_Base)
#define GPIOC ((GPIO_Type*) GPIOC_Base)
#define GPIOD ((GPIO_Type*) GPIOD_Base)
#define GPIOE ((GPIO_Type*) GPIOE_Base)
#define GPIOF ((GPIO_Type*) GPIOF_Base)

// GPIOPCTL register 
#define PCTL_PMC0 0
#define PCTL_PMC1 4
#define PCTL_PMC2 8
#define PCTL_PMC3 12
#define PCTL_PMC4 16
#define PCTL_PMC5 20
#define PCTL_PMC6 24
#define PCTL_PMC7 28

// PIN offsets
#define PIN0 0
#define PIN1 1
#define PIN2 2
#define PIN3 3
#define PIN4 4
#define PIN5 5
#define PIN6 6
#define PIN7 7



/*-------------------------------------------------------------------------------
* UART0 registers
*------------------------------------------------------------------------------*/
typedef struct {                     	// UART0 Structure                                                       
  __IO uint32_t  DR;                   	// UART Data                                                             
  __IO uint32_t  RSR;                	// UART Receive Status/Error Clear                                       
  __I  uint32_t  RESERVED[4];
  __IO uint32_t  FR;                   	// UART Flag                                                             
  __I  uint32_t  RESERVED1;
  __IO uint32_t  ILPR;                 	// UART IrDA Low-Power Register                                          
  __IO uint32_t  IBRD;                 	// UART Integer Baud-Rate Divisor                                        
  __IO uint32_t  FBRD;                 	// UART Fractional Baud-Rate Divisor                                     
  __IO uint32_t  LCRH;                 	// UART Line Control                                                     
  __IO uint32_t  CTL;                  	// UART Control                                                          
  __IO uint32_t  IFLS;                 	// UART Interrupt FIFO Level Select                                      
  __IO uint32_t  IM;                   	// UART Interrupt Mask                                                   
  __IO uint32_t  RIS;                  	// UART Raw Interrupt Status                                             
  __IO uint32_t  MIS;                  	// UART Masked Interrupt Status                                          
  __O  uint32_t  ICR;                  	// UART Interrupt Clear                                                  
  __IO uint32_t  DMACTL;               	// UART DMA Control                                                      
  __I  uint32_t  RESERVED2[22];
  __IO uint32_t  _9BITADDR;            	// UART 9-Bit Self Address                                               
  __IO uint32_t  _9BITAMASK;           	// UART 9-Bit Self Address Mask                                          
  __I  uint32_t  RESERVED3[965];
  __IO uint32_t  PP;                   	// UART Peripheral Properties                                            
  __I  uint32_t  RESERVED4;
  __IO uint32_t  CC;                   	// UART Clock Configuration                                              
} UART_Type;

#define UART0_Base 0x4000C000
#define UART0 ((UART_Type*) UART0_Base)

// CTL register 
#define CTL_CTSEN 15
#define CTL_RTSEN 14
#define CTL_RTS 11
#define CTL_RXE 9
#define CTL_TXE 8
#define CTL_LBE 7
#define CTL_HSE 5
#define CTL_EOT 4
#define CTL_SMART 3
#define CTL_SIRLP 2
#define CTL_SIREN 1
#define CTL_UARTEN 0

// LCRH register 
#define LCHR_SPS 7
#define LCHR_WLEN 5
#define LCHR_FEN 4
#define LCHR_STP2 3
#define LCHR_EPS 2
#define LCHR_PEN 1
#define LCHR_BRK 0

// FR register 
#define FR_TXFE 7 				// Transmit FIFO empty
#define FR_RXFF 6				// Receive FIFO full
#define FR_TXFF 5 				// Transmit FIFO full
#define FR_RXFE 4 				// Receive FIFO empty
#define FR_BUSY 3 				// UART busy
#define FR_CTS 0 				// Clear to send

// IFLS register 
#define RXIFSEL 3 				// UART Receive Interrupt FIFO Level Select
#define TXIFSEL 0 				// UART Transmit Interrupt FIFO Level Select

// IM register 
#define BITIM9 12 				// UART 9-Bit Mode Interrupt Mask
#define OEIM 10 				// UART Overrun Error Interrupt Mask
#define BEIM 9 					// UART Break Error Interrupt Mask
#define PEIM 8 					// UART Parity Error Interrupt Mask
#define FEIM 7 					// UART Framing Error Interrupt Mask
#define RTIM 6 					// UART Receive Time-Out Interrupt Mask
#define TXIM 5 					// UART Transmit Interrupt Mask
#define RXIM 4 					// UART Receive Interrupt Mask
#define CTIM 1 					// UART Clear to Send Interrupt Mask

// RIS register 
#define BITRIS9 12 				// UART 9-Bit Mode Raw Interrupt Status
#define OERIS 10 				// UART Overrun Error Raw Interrupt Status
#define BERIS 9 				// UART Break Error Raw Interrupt Status
#define PERIS 8 				// UART Parity Error Raw Interrupt Status
#define FERIS 7 				// UART Framing Error Raw Interrupt Status
#define RTRIS 6 				// UART Receive Time-Out Raw Interrupt Status
#define TXRIS 5 				// UART Transmit Raw Interrupt Status
#define RXRIS 4 				// UART Receive Raw Interrupt Status
#define CTRIS 1 				// UART Clear to Send Raw Interrupt Status

// ICR register 
#define BITIC9 12 				// UART 9-Bit Mode Interrupt Clear
#define OEIC 10 				// UART Overrun Error Interrupt Clear
#define BEIC 9 					// UART Break Error Interrupt Clear
#define PEIC 8 					// UART Parity Error Interrupt Clear
#define FEIC 7 					// UART Framing Error Interrupt Clear
#define RTIC 6 					// UART Receive TICe-Out Interrupt Clear
#define TXIC 5 					// UART Transmit Interrupt Clear
#define RXIC 4 					// UART Receive Interrupt Clear
#define CTIC 1 					// UART Clear to Send Interrupt Clear



/*-------------------------------------------------------------------------------
* SSI registers 
*------------------------------------------------------------------------------*/
typedef struct {                    // SSI0 Structure                                                 
  __IO uint32_t  CR0;               // SSI Control 0                                                     
  __IO uint32_t  CR1;               // SSI Control 1
  __IO uint32_t  DR;                // SSI Data
  __IO uint32_t  SR;                // SSI Status
  __IO uint32_t  CPSR;              // SSI Clock Prescale
  __IO uint32_t  IM;                // SSI Interrupt Mask 
  __IO uint32_t  RIS;               // SSI Raw Interrupt Status
  __IO uint32_t  MIS;               // SSI Masked Interrupt Status
  __O  uint32_t  ICR;               // SSI Interrupt Clear
  __IO uint32_t  DMACTL;            // SSI DMA Control
  __I  uint32_t  RESERVED[1000];
  __IO uint32_t  CC;                // SSI Clock Configuration
} SSI_Type;

#define SSI0_Base 0x40008000
#define SSI0 ((SSI_Type*) SSI0_Base)

// CR0 register 
#define CR0_SCR 8
#define CR0_SPH 7
#define CR0_SPO 6
#define CR0_FRF 4
#define CR0_DSS 0

// CR1 register 
#define CR1_EOT 4
#define CR1_MS 2
#define CR1_SSE 1
#define CR1_LBM 0

// SR register 
#define SR_BSY 4
#define SR_RFF 3
#define SR_RNE 2
#define SR_TNF 1
#define SR_TFE 0

// IM register 
#define IM_TXIM 3
#define IM_RXIM 2
#define IM_RTIM 1
#define IM_RORIM 0

// RIS register 
#define RIS_TXRIS 3
#define RIS_RXRIS 2
#define RIS_RTRIS 1
#define RIS_RORRIS 0

// MIS register 
#define MIS_TXMIS 3
#define MIS_RXMIS 2
#define MIS_RTMIS 1
#define MIS_RORMIS 0



/*-------------------------------------------------------------------------------
* I2C registers 
*------------------------------------------------------------------------------*/
typedef struct {                                    // I2C0 Structure    
  __IO uint32_t  MSA;                               // I2C Master Slave Address
  union {
    __IO uint32_t  MCS_I2C0_ALT;                    // I2C Master Control/Status                                             
    __IO uint32_t  MCS;                             // I2C Master Control/Status                                             
  };
  __IO uint32_t  MDR;                               // I2C Master Data                                                       
  __IO uint32_t  MTPR;                              // I2C Master Timer Period                                               
  __IO uint32_t  MIMR;                              // I2C Master Interrupt Mask                                             
  __IO uint32_t  MRIS;                              // I2C Master Raw Interrupt Status                                       
  __IO uint32_t  MMIS;                              // I2C Master Masked Interrupt Status                                    
  __O  uint32_t  MICR;                              // I2C Master Interrupt Clear                                            
  __IO uint32_t  MCR;                               // I2C Master Configuration                                              
  __IO uint32_t  MCLKOCNT;                          // I2C Master Clock Low Timeout Count                                    
  __I  uint32_t  RESERVED;
  __IO uint32_t  MBMON;                             // I2C Master Bus Monitor                                                
  __I  uint32_t  RESERVED1[2];
  __IO uint32_t  MCR2;                              // I2C Master Configuration 2                                            
  __I  uint32_t  RESERVED2[497];
  __IO uint32_t  SOAR;                              // I2C Slave Own Address                                       
  union {
    __IO uint32_t  SCSR_I2C0_ALT;                   // I2C Slave Control/Status                                              
    __IO uint32_t  SCSR;                            // I2C Slave Control/Status                                              
  };
  __IO uint32_t  SDR;                               // I2C Slave Data                                                        
  __IO uint32_t  SIMR;                              // I2C Slave Interrupt Mask                                              
  __IO uint32_t  SRIS;                              // I2C Slave Raw Interrupt Status                                        
  __IO uint32_t  SMIS;                              // I2C Slave Masked Interrupt Status                                     
  __O  uint32_t  SICR;                              // I2C Slave Interrupt Clear                                             
  __IO uint32_t  SOAR2;                             // I2C Slave Own Address 2                                               
  __IO uint32_t  SACKCTL;                           // I2C Slave ACK Control                                                 
  __I  uint32_t  RESERVED3[487];
  __IO uint32_t  PP;                                // I2C Peripheral Properties                                             
  __IO uint32_t  PC;                                // I2C Peripheral Configuration                                          
} I2C_Type;

#define I2C0_BASE 0x40020000UL
#define I2C0 ((I2C_Type*) I2C0_BASE)

// MCR Master Configuration Register 
#define MCR_GPE 6
#define MCR_SFE 5
#define MCR_MFE 4
#define MCR_LPBK 0

// MTPR Master Timer Period Register  
#define MTPR_HS 7
#define MTPR_TPR 0

// MCS Master Control Status
#define MCS_HS 4 			// Write
#define MCS_ACK 3
#define MCS_STOP 2
#define MCS_START 1
#define MCS_RUN 0
#define MCS_CLKTO 7			// Read
#define MCS_BUSBSY 6
#define MCS_IDLE 5
#define MCS_ARBLST 4
#define MCS_DATACK 3
#define MCS_ADRACK 2 			
#define MCS_ERROR 1
#define MCS_BUSY 0

// MSA Master Slave Address
#define MSA_SA 1
#define MSA_RS 0



/*-------------------------------------------------------------------------------
* ADC registers 
*------------------------------------------------------------------------------*/
typedef struct {                                    // ADC0 Structure                                                        
  __IO uint32_t  ACTSS;                             // ADC Active Sample Sequencer                                           
  __IO uint32_t  RIS;                               // ADC Raw Interrupt Status                                              
  __IO uint32_t  IM;                                // ADC Interrupt Mask                                                    
  __IO uint32_t  ISC;                               // ADC Interrupt Status and Clear                                        
  __IO uint32_t  OSTAT;                             // ADC Overflow Status                                                   
  __IO uint32_t  EMUX;                              // ADC Event Multiplexer Select                                          
  __IO uint32_t  USTAT;                             // ADC Underflow Status                                                  
  __IO uint32_t  TSSEL;                             // ADC Trigger Source Select                                             
  __IO uint32_t  SSPRI;                             // ADC Sample Sequencer Priority                                         
  __IO uint32_t  SPC;                               // ADC Sample Phase Control                                              
  __IO uint32_t  PSSI;                              // ADC Processor Sample Sequence Initiate                                
  __I  uint32_t  RESERVED;
  __IO uint32_t  SAC;                               // ADC Sample Averaging Control                                          
  __IO uint32_t  DCISC;                             // ADC Digital Comparator Interrupt Status and Clear                     
  __IO uint32_t  CTL;                               // ADC Control                                                           
  __I  uint32_t  RESERVED1;
  __IO uint32_t  SSMUX0;                            // ADC Sample Sequence Input Multiplexer Select 0                        
  __IO uint32_t  SSCTL0;                            // ADC Sample Sequence Control 0                                         
  __IO uint32_t  SSFIFO0;                           // ADC Sample Sequence Result FIFO 0                                     
  __IO uint32_t  SSFSTAT0;                          // ADC Sample Sequence FIFO 0 Status                                     
  __IO uint32_t  SSOP0;                             // ADC Sample Sequence 0 Operation                                       
  __IO uint32_t  SSDC0;                             // ADC Sample Sequence 0 Digital Comparator Select                       
  __I  uint32_t  RESERVED2[2];
  __IO uint32_t  SSMUX1;                            // ADC Sample Sequence Input Multiplexer Select 1                        
  __IO uint32_t  SSCTL1;                            // ADC Sample Sequence Control 1                                         
  __IO uint32_t  SSFIFO1;                           // ADC Sample Sequence Result FIFO 1                                     
  __IO uint32_t  SSFSTAT1;                          // ADC Sample Sequence FIFO 1 Status                                     
  __IO uint32_t  SSOP1;                             // ADC Sample Sequence 1 Operation                                       
  __IO uint32_t  SSDC1;                             // ADC Sample Sequence 1 Digital Comparator Select                       
  __I  uint32_t  RESERVED3[2];
  __IO uint32_t  SSMUX2;                            // ADC Sample Sequence Input Multiplexer Select 2                        
  __IO uint32_t  SSCTL2;                            // ADC Sample Sequence Control 2                                         
  __IO uint32_t  SSFIFO2;                           // ADC Sample Sequence Result FIFO 2                                     
  __IO uint32_t  SSFSTAT2;                          // ADC Sample Sequence FIFO 2 Status                                     
  __IO uint32_t  SSOP2;                             // ADC Sample Sequence 2 Operation                                       
  __IO uint32_t  SSDC2;                             // ADC Sample Sequence 2 Digital Comparator Select                       
  __I  uint32_t  RESERVED4[2];
  __IO uint32_t  SSMUX3;                            // ADC Sample Sequence Input Multiplexer Select 3                        
  __IO uint32_t  SSCTL3;                            // ADC Sample Sequence Control 3                                         
  __IO uint32_t  SSFIFO3;                           // ADC Sample Sequence Result FIFO 3                                     
  __IO uint32_t  SSFSTAT3;                          // ADC Sample Sequence FIFO 3 Status                                     
  __IO uint32_t  SSOP3;                             // ADC Sample Sequence 3 Operation                                       
  __IO uint32_t  SSDC3;                             // ADC Sample Sequence 3 Digital Comparator Select                       
  __I  uint32_t  RESERVED5[786];
  __O  uint32_t  DCRIC;                             // ADC Digital Comparator Reset Initial Conditions                       
  __I  uint32_t  RESERVED6[63];
  __IO uint32_t  DCCTL0;                            // ADC Digital Comparator Control 0                                      
  __IO uint32_t  DCCTL1;                            // ADC Digital Comparator Control 1                                      
  __IO uint32_t  DCCTL2;                            // ADC Digital Comparator Control 2                                      
  __IO uint32_t  DCCTL3;                            // ADC Digital Comparator Control 3                                      
  __IO uint32_t  DCCTL4;                            // ADC Digital Comparator Control 4                                      
  __IO uint32_t  DCCTL5;                            // ADC Digital Comparator Control 5                                      
  __IO uint32_t  DCCTL6;                            // ADC Digital Comparator Control 6                                      
  __IO uint32_t  DCCTL7;                            // ADC Digital Comparator Control 7                                      
  __I  uint32_t  RESERVED7[8];
  __IO uint32_t  DCCMP0;                            // ADC Digital Comparator Range 0                                        
  __IO uint32_t  DCCMP1;                            // ADC Digital Comparator Range 1                                        
  __IO uint32_t  DCCMP2;                            // ADC Digital Comparator Range 2                                        
  __IO uint32_t  DCCMP3;                            // ADC Digital Comparator Range 3                                        
  __IO uint32_t  DCCMP4;                            // ADC Digital Comparator Range 4                                        
  __IO uint32_t  DCCMP5;                            // ADC Digital Comparator Range 5                                        
  __IO uint32_t  DCCMP6;                            // ADC Digital Comparator Range 6                                        
  __IO uint32_t  DCCMP7;                            // ADC Digital Comparator Range 7                                        
  __I  uint32_t  RESERVED8[88];
  __IO uint32_t  PP;                                // ADC Peripheral Properties                                             
  __IO uint32_t  PC;                                // ADC Peripheral Configuration                                          
  __IO uint32_t  CC;                                // ADC Clock Configuration                                               
} ADC_Type;

#define ADC0_BASE 0x40038000UL
#define ADC1_BASE 0x40039000UL
#define ADC0 ((ADC_Type*) ADC0_BASE)
#define ADC1 ((ADC_Type*) ADC1_BASE)


// ACTSS Active Sample Sequencer Register
#define ACTSS_ASEN0 0
#define ACTSS_ASEN1 1
#define ACTSS_ASEN2 2
#define ACTSS_ASEN3 3
#define ACTSS_BUSY 16

// RIS Raw Interrupt Status Register
#define RIS_INR0 0
#define RIS_INR1 1
#define RIS_INR2 2
#define RIS_INR3 3
#define RIS_INRDC 16

// IM Interrupt Mask Register
#define IM_MASK0 0
#define IM_MASK1 1
#define IM_MASK2 2
#define IM_MASK3 3
#define IM_DCONSS0 16
#define IM_DCONSS1 17
#define IM_DCONSS2 18
#define IM_DCONSS3 19

// ISC Interrupt Status and Clear Register
#define ISC_IN0 0
#define ISC_IN1 1
#define ISC_IN2 2
#define ISC_IN3 3
#define ISC_DCINSS0 16
#define ISC_DCINSS1 17
#define ISC_DCINSS2 18
#define ISC_DCINSS3 19

// EMUX Event Multiplexer Select Register
#define EMUX_EM0 0
#define EMUX_EM1 4
#define EMUX_EM2 8
#define EMUX_EM3 12

// ADC Processor Sample Sequence Initiate Register                
#define PSSI_SS0 0    
#define PSSI_SS1 1 
#define PSSI_SS2 2  
#define PSSI_SS3 3 
#define PSSI_SYNCWAIT 27  
#define PSSI_GSYNC 31

// SSCTL3 Sample Sequence Control 3 Register
#define SSCTL3_D0 0
#define SSCTL3_END0 1
#define SSCTL3_IE0 2
#define SSCTL3_TS0 3

// ADC Digital Comparator Control 0 Register
#define DCCTL0_CIM 0
#define DCCTL0_CIC 2
#define DCCTL0_CIE 4 
#define DCCTL0_CTM 8
#define DCCTL0_CTC 10
#define DCCTL0_CTE 12

// ADC Digital Comparator Range 0 Register
#define DCCMP0_COMP0 0
#define DCCMP0_COMP1 16

// DCRIC Digital Comparator Reset Initial Condition Register
#define DCRIC_DCINT0 0
#define DCRIC_DCINT1 1
#define DCRIC_DCINT2 2
#define DCRIC_DCINT3 3
#define DCRIC_DCINT4 4
#define DCRIC_DCINT5 5
#define DCRIC_DCINT6 6
#define DCRIC_DCINT7 7
#define DCRIC_DCTRIG0 16
#define DCRIC_DCTRIG1 17
#define DCRIC_DCTRIG2 18
#define DCRIC_DCTRIG3 19
#define DCRIC_DCTRIG4 20
#define DCRIC_DCTRIG5 21
#define DCRIC_DCTRIG6 22
#define DCRIC_DCTRIG7 23

// DCISC Digital Comparator Interrupt Status and Clear Register
#define DCISC_DCINT0 0
#define DCISC_DCINT1 1
#define DCISC_DCINT2 2
#define DCISC_DCINT3 3
#define DCISC_DCINT4 4
#define DCISC_DCINT5 5
#define DCISC_DCINT6 6
#define DCISC_DCINT7 7



/*-------------------------------------------------------------------------------
* PWM registers 
*------------------------------------------------------------------------------*/
typedef struct {                                    // PWM0 Structure    
  __IO uint32_t  CTL;                               // PWM Master Control                                                    
  __IO uint32_t  SYNC;                              // PWM Time Base Sync
  __IO uint32_t  ENABLE;                            // PWM Output Enable                                                     
  __IO uint32_t  INVERT;                            // PWM Output Inversion                                                  
  __IO uint32_t  FAULT;                             // PWM Output Fault                                                      
  __IO uint32_t  INTEN;                             // PWM Interrupt Enable                                                  
  __IO uint32_t  RIS;                               // PWM Raw Interrupt Status                                              
  __IO uint32_t  ISC;                               // PWM Interrupt Status and Clear                                        
  __IO uint32_t  STATUS;                            // PWM Status                                                            
  __IO uint32_t  FAULTVAL;                          // PWM Fault Condition Value                                             
  __IO uint32_t  ENUPD;                             // PWM Enable Update                                                     
  __I  uint32_t  RESERVED[5];
  __IO uint32_t  _0_CTL;                            // PWM0 Control                                                          
  __IO uint32_t  _0_INTEN;                          // PWM0 Interrupt and Trigger Enable                                     
  __IO uint32_t  _0_RIS;                            // PWM0 Raw Interrupt Status                                             
  __IO uint32_t  _0_ISC;                            // PWM0 Interrupt Status and Clear                                       
  __IO uint32_t  _0_LOAD;                           // PWM0 Load                                                             
  __IO uint32_t  _0_COUNT;                          // PWM0 Counter                                                          
  __IO uint32_t  _0_CMPA;                           // PWM0 Compare A                                                        
  __IO uint32_t  _0_CMPB;                           // PWM0 Compare B                                                        
  __IO uint32_t  _0_GENA;                           // PWM0 Generator A Control                                              
  __IO uint32_t  _0_GENB;                           // PWM0 Generator B Control                                              
  __IO uint32_t  _0_DBCTL;                          // PWM0 Dead-Band Control                                                
  __IO uint32_t  _0_DBRISE;                         // PWM0 Dead-Band Rising-Edge Delay                                      
  __IO uint32_t  _0_DBFALL;                         // PWM0 Dead-Band Falling-Edge-Delay                                     
  __IO uint32_t  _0_FLTSRC0;                        // PWM0 Fault Source 0                                                   
  __IO uint32_t  _0_FLTSRC1;                        // PWM0 Fault Source 1                                                   
  __IO uint32_t  _0_MINFLTPER;                      // PWM0 Minimum Fault Period                                             
  __IO uint32_t  _1_CTL;                            // PWM1 Control                                                          
  __IO uint32_t  _1_INTEN;                          // PWM1 Interrupt and Trigger Enable                                     
  __IO uint32_t  _1_RIS;                            // PWM1 Raw Interrupt Status                                             
  __IO uint32_t  _1_ISC;                            // PWM1 Interrupt Status and Clear                                       
  __IO uint32_t  _1_LOAD;                           // PWM1 Load                                                             
  __IO uint32_t  _1_COUNT;                          // PWM1 Counter                                                          
  __IO uint32_t  _1_CMPA;                           // PWM1 Compare A                                                        
  __IO uint32_t  _1_CMPB;                           // PWM1 Compare B                                                        
  __IO uint32_t  _1_GENA;                           // PWM1 Generator A Control                                              
  __IO uint32_t  _1_GENB;                           // PWM1 Generator B Control                                              
  __IO uint32_t  _1_DBCTL;                          // PWM1 Dead-Band Control                                                
  __IO uint32_t  _1_DBRISE;                         // PWM1 Dead-Band Rising-Edge Delay                                      
  __IO uint32_t  _1_DBFALL;                         // PWM1 Dead-Band Falling-Edge-Delay                                     
  __IO uint32_t  _1_FLTSRC0;                        // PWM1 Fault Source 0                                                   
  __IO uint32_t  _1_FLTSRC1;                        // PWM1 Fault Source 1                                                   
  __IO uint32_t  _1_MINFLTPER;                      // PWM1 Minimum Fault Period                                             
  __IO uint32_t  _2_CTL;                            // PWM2 Control                                                          
  __IO uint32_t  _2_INTEN;                          // PWM2 Interrupt and Trigger Enable                                     
  __IO uint32_t  _2_RIS;                            // PWM2 Raw Interrupt Status                                             
  __IO uint32_t  _2_ISC;                            // PWM2 Interrupt Status and Clear                                       
  __IO uint32_t  _2_LOAD;                           // PWM2 Load                                                             
  __IO uint32_t  _2_COUNT;                          // PWM2 Counter                                                          
  __IO uint32_t  _2_CMPA;                           // PWM2 Compare A                                                        
  __IO uint32_t  _2_CMPB;                           // PWM2 Compare B                                                        
  __IO uint32_t  _2_GENA;                           // PWM2 Generator A Control                                              
  __IO uint32_t  _2_GENB;                           // PWM2 Generator B Control                                              
  __IO uint32_t  _2_DBCTL;                          // PWM2 Dead-Band Control                                                
  __IO uint32_t  _2_DBRISE;                         // PWM2 Dead-Band Rising-Edge Delay                                      
  __IO uint32_t  _2_DBFALL;                         // PWM2 Dead-Band Falling-Edge-Delay                                     
  __IO uint32_t  _2_FLTSRC0;                        // PWM2 Fault Source 0                                                   
  __IO uint32_t  _2_FLTSRC1;                        // PWM2 Fault Source 1                                                   
  __IO uint32_t  _2_MINFLTPER;                      // PWM2 Minimum Fault Period                                             
  __IO uint32_t  _3_CTL;                            // PWM3 Control                                                          
  __IO uint32_t  _3_INTEN;                          // PWM3 Interrupt and Trigger Enable                                     
  __IO uint32_t  _3_RIS;                            // PWM3 Raw Interrupt Status                                             
  __IO uint32_t  _3_ISC;                            // PWM3 Interrupt Status and Clear                                       
  __IO uint32_t  _3_LOAD;                           // PWM3 Load                                                             
  __IO uint32_t  _3_COUNT;                          // PWM3 Counter                                                          
  __IO uint32_t  _3_CMPA;                           // PWM3 Compare A                                                        
  __IO uint32_t  _3_CMPB;                           // PWM3 Compare B                                                        
  __IO uint32_t  _3_GENA;                           // PWM3 Generator A Control                                              
  __IO uint32_t  _3_GENB;                           // PWM3 Generator B Control                                              
  __IO uint32_t  _3_DBCTL;                          // PWM3 Dead-Band Control                                                
  __IO uint32_t  _3_DBRISE;                         // PWM3 Dead-Band Rising-Edge Delay                                      
  __IO uint32_t  _3_DBFALL;                         // PWM3 Dead-Band Falling-Edge-Delay                                     
  __IO uint32_t  _3_FLTSRC0;                        // PWM3 Fault Source 0                                                   
  __IO uint32_t  _3_FLTSRC1;                        // PWM3 Fault Source 1                                                   
  __IO uint32_t  _3_MINFLTPER;                      // PWM3 Minimum Fault Period                                             
  __I  uint32_t  RESERVED1[432];
  __IO uint32_t  _0_FLTSEN;                         // PWM0 Fault Pin Logic Sense                                            
  __I  uint32_t  _0_FLTSTAT0;                       // PWM0 Fault Status 0                                                   
  __I  uint32_t  _0_FLTSTAT1;                       // PWM0 Fault Status 1                                                   
  __I  uint32_t  RESERVED2[29];
  __IO uint32_t  _1_FLTSEN;                         // PWM1 Fault Pin Logic Sense                                            
  __I  uint32_t  _1_FLTSTAT0;                       // PWM1 Fault Status 0                                                   
  __I  uint32_t  _1_FLTSTAT1;                       // PWM1 Fault Status 1                                                   
  __I  uint32_t  RESERVED3[30];
  __I  uint32_t  _2_FLTSTAT0;                       // PWM2 Fault Status 0                                                   
  __I  uint32_t  _2_FLTSTAT1;                       // PWM2 Fault Status 1                                                   
  __I  uint32_t  RESERVED4[30];
  __I  uint32_t  _3_FLTSTAT0;                       // PWM3 Fault Status 0                                                   
  __I  uint32_t  _3_FLTSTAT1;                       // PWM3 Fault Status 1                                                   
  __I  uint32_t  RESERVED5[397];
  __IO uint32_t  PP;                                // PWM Peripheral Properties                                             
} PWM_Type;

#define PWM0_BASE 0x40028000UL
#define PWM1_BASE 0x40029000UL
#define PWM0 ((PWM_Type*) PWM0_BASE)
#define PWM1 ((PWM_Type*) PWM1_BASE)

// PWM Control Register
#define PWMCTL_ENABLE 0
#define PWMCTL_MODE 1
#define PWMCTL_DEBUG 2
#define PWMCTL_LOADUPD 3
#define PWMCTL_CMPAUPD 4
#define PWMCTL_CMPBUPD 5
#define PWMCTL_GENAUPD 6
#define PWMCTL_GENBUPD 8
#define PWMCTL_DBCTLUPD 10
#define PWMCTL_DBRISEUPD 12
#define PWMCTL_DBFALLUPD 14
#define PWMCTL_FLTSRC 16
#define PWMCTL_MINFLTPER 17
#define PWMCTL_LATCH 18

// PWM Generator Control Register
#define PWMGEN_ACTZERO 0
#define PWMGEN_ACTLOAD 2
#define PWMGEN_ACTCMPAU 4
#define PWMGEN_ACTCMPAD 6
#define PWMGEN_ACTCMPBU 8
#define PWMGEN_ACTCMPBD 10

// PWM Output Enable Register
#define PWMENABLE_PWM0EN 0
#define PWMENABLE_PWM1EN 1
#define PWMENABLE_PWM2EN 2
#define PWMENABLE_PWM3EN 3
#define PWMENABLE_PWM4EN 4
#define PWMENABLE_PWM5EN 5
#define PWMENABLE_PWM6EN 6
#define PWMENABLE_PWM7EN 7


#endif
