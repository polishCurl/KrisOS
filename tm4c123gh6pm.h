/*******************************************************************************
* File:     	tm4c123gh6pm.h
* Brief:    	Register definitions for tm4c123gh6pm microcontroller 
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"
#pragma anon_unions

/*-------------------------------------------------------------------------------
* IO definitions (access restrictions to peripheral registers)
*------------------------------------------------------------------------------*/
#define __I volatile const 		// input, read-only
#define __O volatile 			// output, write-only
#define __IO volatile 			// input-output, read and write permissions


/*-------------------------------------------------------------------------------
* System control registers
*------------------------------------------------------------------------------*/
typedef struct {                                    /*!< SYSCTL Structure                                                      */
  __IO uint32_t  DID0;                              /*!< Device Identification 0                                               */
  __IO uint32_t  DID1;                              /*!< Device Identification 1                                               */
  __IO uint32_t  DC0;                               /*!< Device Capabilities 0                                                 */
  __I  uint32_t  RESERVED;
  __IO uint32_t  DC1;                               /*!< Device Capabilities 1                                                 */
  __IO uint32_t  DC2;                               /*!< Device Capabilities 2                                                 */
  __IO uint32_t  DC3;                               /*!< Device Capabilities 3                                                 */
  __IO uint32_t  DC4;                               /*!< Device Capabilities 4                                                 */
  __IO uint32_t  DC5;                               /*!< Device Capabilities 5                                                 */
  __IO uint32_t  DC6;                               /*!< Device Capabilities 6                                                 */
  __IO uint32_t  DC7;                               /*!< Device Capabilities 7                                                 */
  __IO uint32_t  DC8;                               /*!< Device Capabilities 8                                                 */
  __IO uint32_t  PBORCTL;                           /*!< Brown-Out Reset Control                                               */
  __I  uint32_t  RESERVED1[3];
  __IO uint32_t  SRCR0;                             /*!< Software Reset Control 0                                              */
  __IO uint32_t  SRCR1;                             /*!< Software Reset Control 1                                              */
  __IO uint32_t  SRCR2;                             /*!< Software Reset Control 2                                              */
  __I  uint32_t  RESERVED2;
  __IO uint32_t  RIS;                               /*!< Raw Interrupt Status                                                  */
  __IO uint32_t  IMC;                               /*!< Interrupt Mask Control                                                */
  __IO uint32_t  MISC;                              /*!< Masked Interrupt Status and Clear                                     */
  __IO uint32_t  RESC;                              /*!< Reset Cause                                                           */
  __IO uint32_t  RCC;                               /*!< Run-Mode Clock Configuration                                          */
  __I  uint32_t  RESERVED3[2];
  __IO uint32_t  GPIOHBCTL;                         /*!< GPIO High-Performance Bus Control                                     */
  __IO uint32_t  RCC2;                              /*!< Run-Mode Clock Configuration 2                                        */
  __I  uint32_t  RESERVED4[2];
  __IO uint32_t  MOSCCTL;                           /*!< Main Oscillator Control                                               */
  __I  uint32_t  RESERVED5[32];
  __IO uint32_t  RCGC0;                             /*!< Run Mode Clock Gating Control Register 0                              */
  __IO uint32_t  RCGC1;                             /*!< Run Mode Clock Gating Control Register 1                              */
  __IO uint32_t  RCGC2;                             /*!< Run Mode Clock Gating Control Register 2                              */
  __I  uint32_t  RESERVED6;
  __IO uint32_t  SCGC0;                             /*!< Sleep Mode Clock Gating Control Register 0                            */
  __IO uint32_t  SCGC1;                             /*!< Sleep Mode Clock Gating Control Register 1                            */
  __IO uint32_t  SCGC2;                             /*!< Sleep Mode Clock Gating Control Register 2                            */
  __I  uint32_t  RESERVED7;
  __IO uint32_t  DCGC0;                             /*!< Deep Sleep Mode Clock Gating Control Register 0                       */
  __IO uint32_t  DCGC1;                             /*!< Deep-Sleep Mode Clock Gating Control Register 1                       */
  __IO uint32_t  DCGC2;                             /*!< Deep Sleep Mode Clock Gating Control Register 2                       */
  __I  uint32_t  RESERVED8[6];
  __IO uint32_t  DSLPCLKCFG;                        /*!< Deep Sleep Clock Configuration                                        */
  __I  uint32_t  RESERVED9;
  __IO uint32_t  SYSPROP;                           /*!< System Properties                                                     */
  __IO uint32_t  PIOSCCAL;                          /*!< Precision Internal Oscillator Calibration                             */
  __IO uint32_t  PIOSCSTAT;                         /*!< Precision Internal Oscillator Statistics                              */
  __I  uint32_t  RESERVED10[2];
  __IO uint32_t  PLLFREQ0;                          /*!< PLL Frequency 0                                                       */
  __IO uint32_t  PLLFREQ1;                          /*!< PLL Frequency 1                                                       */
  __IO uint32_t  PLLSTAT;                           /*!< PLL Status                                                            */
  __I  uint32_t  RESERVED11[7];
  __IO uint32_t  SLPPWRCFG;                         /*!< Sleep Power Configuration                                             */
  __IO uint32_t  DSLPPWRCFG;                        /*!< Deep-Sleep Power Configuration                                        */
  __IO uint32_t  DC9;                               /*!< Device Capabilities 9                                                 */
  __I  uint32_t  RESERVED12[3];
  __IO uint32_t  NVMSTAT;                           /*!< Non-Volatile Memory Information                                       */
  __I  uint32_t  RESERVED13[4];
  __IO uint32_t  LDOSPCTL;                          /*!< LDO Sleep Power Control                                               */
  __I  uint32_t  RESERVED14;
  __IO uint32_t  LDODPCTL;                          /*!< LDO Deep-Sleep Power Control                                          */
  __I  uint32_t  RESERVED15[80];
  __IO uint32_t  PPWD;                              /*!< Watchdog Timer Peripheral Present                                     */
  __IO uint32_t  PPTIMER;                           /*!< 16/32-Bit General-Purpose Timer Peripheral Present                    */
  __IO uint32_t  PPGPIO;                            /*!< General-Purpose Input/Output Peripheral Present                       */
  __IO uint32_t  PPDMA;                             /*!< Micro Direct Memory Access Peripheral Present                         */
  __I  uint32_t  RESERVED16;
  __IO uint32_t  PPHIB;                             /*!< Hibernation Peripheral Present                                        */
  __IO uint32_t  PPUART;                            /*!< Universal Asynchronous Receiver/Transmitter Peripheral Present        */
  __IO uint32_t  PPSSI;                             /*!< Synchronous Serial Interface Peripheral Present                       */
  __IO uint32_t  PPI2C;                             /*!< Inter-Integrated Circuit Peripheral Present                           */
  __I  uint32_t  RESERVED17;
  __IO uint32_t  PPUSB;                             /*!< Universal Serial Bus Peripheral Present                               */
  __I  uint32_t  RESERVED18[2];
  __IO uint32_t  PPCAN;                             /*!< Controller Area Network Peripheral Present                            */
  __IO uint32_t  PPADC;                             /*!< Analog-to-Digital Converter Peripheral Present                        */
  __IO uint32_t  PPACMP;                            /*!< Analog Comparator Peripheral Present                                  */
  __IO uint32_t  PPPWM;                             /*!< Pulse Width Modulator Peripheral Present                              */
  __IO uint32_t  PPQEI;                             /*!< Quadrature Encoder Interface Peripheral Present                       */
  __I  uint32_t  RESERVED19[4];
  __IO uint32_t  PPEEPROM;                          /*!< EEPROM Peripheral Present                                             */
  __IO uint32_t  PPWTIMER;                          /*!< 32/64-Bit Wide General-Purpose Timer Peripheral Present               */
  __I  uint32_t  RESERVED20[104];
  __IO uint32_t  SRWD;                              /*!< Watchdog Timer Software Reset                                         */
  __IO uint32_t  SRTIMER;                           /*!< 16/32-Bit General-Purpose Timer Software Reset                        */
  __IO uint32_t  SRGPIO;                            /*!< General-Purpose Input/Output Software Reset                           */
  __IO uint32_t  SRDMA;                             /*!< Micro Direct Memory Access Software Reset                             */
  __I  uint32_t  RESERVED21;
  __IO uint32_t  SRHIB;                             /*!< Hibernation Software Reset                                            */
  __IO uint32_t  SRUART;                            /*!< Universal Asynchronous Receiver/Transmitter Software Reset            */
  __IO uint32_t  SRSSI;                             /*!< Synchronous Serial Interface Software Reset                           */
  __IO uint32_t  SRI2C;                             /*!< Inter-Integrated Circuit Software Reset                               */
  __I  uint32_t  RESERVED22;
  __IO uint32_t  SRUSB;                             /*!< Universal Serial Bus Software Reset                                   */
  __I  uint32_t  RESERVED23[2];
  __IO uint32_t  SRCAN;                             /*!< Controller Area Network Software Reset                                */
  __IO uint32_t  SRADC;                             /*!< Analog-to-Digital Converter Software Reset                            */
  __IO uint32_t  SRACMP;                            /*!< Analog Comparator Software Reset                                      */
  __IO uint32_t  SRPWM;                             /*!< Pulse Width Modulator Software Reset                                  */
  __IO uint32_t  SRQEI;                             /*!< Quadrature Encoder Interface Software Reset                           */
  __I  uint32_t  RESERVED24[4];
  __IO uint32_t  SREEPROM;                          /*!< EEPROM Software Reset                                                 */
  __IO uint32_t  SRWTIMER;                          /*!< 32/64-Bit Wide General-Purpose Timer Software Reset                   */
  __I  uint32_t  RESERVED25[40];
  __IO uint32_t  RCGCWD;                            /*!< Watchdog Timer Run Mode Clock Gating Control                          */
  __IO uint32_t  RCGCTIMER;                         /*!< 16/32-Bit General-Purpose Timer Run Mode Clock Gating Control         */
  __IO uint32_t  RCGCGPIO;                          /*!< General-Purpose Input/Output Run Mode Clock Gating Control            */
  __IO uint32_t  RCGCDMA;                           /*!< Micro Direct Memory Access Run Mode Clock Gating Control              */
  __I  uint32_t  RESERVED26;
  __IO uint32_t  RCGCHIB;                           /*!< Hibernation Run Mode Clock Gating Control                             */
  __IO uint32_t  RCGCUART;                          /*!< Universal Asynchronous Receiver/Transmitter Run Mode Clock Gating
                                                         Control                                                               */
  __IO uint32_t  RCGCSSI;                           /*!< Synchronous Serial Interface Run Mode Clock Gating Control            */
  __IO uint32_t  RCGCI2C;                           /*!< Inter-Integrated Circuit Run Mode Clock Gating Control                */
  __I  uint32_t  RESERVED27;
  __IO uint32_t  RCGCUSB;                           /*!< Universal Serial Bus Run Mode Clock Gating Control                    */
  __I  uint32_t  RESERVED28[2];
  __IO uint32_t  RCGCCAN;                           /*!< Controller Area Network Run Mode Clock Gating Control                 */
  __IO uint32_t  RCGCADC;                           /*!< Analog-to-Digital Converter Run Mode Clock Gating Control             */
  __IO uint32_t  RCGCACMP;                          /*!< Analog Comparator Run Mode Clock Gating Control                       */
  __IO uint32_t  RCGCPWM;                           /*!< Pulse Width Modulator Run Mode Clock Gating Control                   */
  __IO uint32_t  RCGCQEI;                           /*!< Quadrature Encoder Interface Run Mode Clock Gating Control            */
  __I  uint32_t  RESERVED29[4];
  __IO uint32_t  RCGCEEPROM;                        /*!< EEPROM Run Mode Clock Gating Control                                  */
  __IO uint32_t  RCGCWTIMER;                        /*!< 32/64-Bit Wide General-Purpose Timer Run Mode Clock Gating Control    */
  __I  uint32_t  RESERVED30[40];
  __IO uint32_t  SCGCWD;                            /*!< Watchdog Timer Sleep Mode Clock Gating Control                        */
  __IO uint32_t  SCGCTIMER;                         /*!< 16/32-Bit General-Purpose Timer Sleep Mode Clock Gating Control       */
  __IO uint32_t  SCGCGPIO;                          /*!< General-Purpose Input/Output Sleep Mode Clock Gating Control          */
  __IO uint32_t  SCGCDMA;                           /*!< Micro Direct Memory Access Sleep Mode Clock Gating Control            */
  __I  uint32_t  RESERVED31;
  __IO uint32_t  SCGCHIB;                           /*!< Hibernation Sleep Mode Clock Gating Control                           */
  __IO uint32_t  SCGCUART;                          /*!< Universal Asynchronous Receiver/Transmitter Sleep Mode Clock
                                                         Gating Control                                                        */
  __IO uint32_t  SCGCSSI;                           /*!< Synchronous Serial Interface Sleep Mode Clock Gating Control          */
  __IO uint32_t  SCGCI2C;                           /*!< Inter-Integrated Circuit Sleep Mode Clock Gating Control              */
  __I  uint32_t  RESERVED32;
  __IO uint32_t  SCGCUSB;                           /*!< Universal Serial Bus Sleep Mode Clock Gating Control                  */
  __I  uint32_t  RESERVED33[2];
  __IO uint32_t  SCGCCAN;                           /*!< Controller Area Network Sleep Mode Clock Gating Control               */
  __IO uint32_t  SCGCADC;                           /*!< Analog-to-Digital Converter Sleep Mode Clock Gating Control           */
  __IO uint32_t  SCGCACMP;                          /*!< Analog Comparator Sleep Mode Clock Gating Control                     */
  __IO uint32_t  SCGCPWM;                           /*!< Pulse Width Modulator Sleep Mode Clock Gating Control                 */
  __IO uint32_t  SCGCQEI;                           /*!< Quadrature Encoder Interface Sleep Mode Clock Gating Control          */
  __I  uint32_t  RESERVED34[4];
  __IO uint32_t  SCGCEEPROM;                        /*!< EEPROM Sleep Mode Clock Gating Control                                */
  __IO uint32_t  SCGCWTIMER;                        /*!< 32/64-Bit Wide General-Purpose Timer Sleep Mode Clock Gating
                                                         Control                                                               */
  __I  uint32_t  RESERVED35[40];
  __IO uint32_t  DCGCWD;                            /*!< Watchdog Timer Deep-Sleep Mode Clock Gating Control                   */
  __IO uint32_t  DCGCTIMER;                         /*!< 16/32-Bit General-Purpose Timer Deep-Sleep Mode Clock Gating
                                                         Control                                                               */
  __IO uint32_t  DCGCGPIO;                          /*!< General-Purpose Input/Output Deep-Sleep Mode Clock Gating Control     */
  __IO uint32_t  DCGCDMA;                           /*!< Micro Direct Memory Access Deep-Sleep Mode Clock Gating Control       */
  __I  uint32_t  RESERVED36;
  __IO uint32_t  DCGCHIB;                           /*!< Hibernation Deep-Sleep Mode Clock Gating Control                      */
  __IO uint32_t  DCGCUART;                          /*!< Universal Asynchronous Receiver/Transmitter Deep-Sleep Mode
                                                         Clock Gating Control                                                  */
  __IO uint32_t  DCGCSSI;                           /*!< Synchronous Serial Interface Deep-Sleep Mode Clock Gating Control     */
  __IO uint32_t  DCGCI2C;                           /*!< Inter-Integrated Circuit Deep-Sleep Mode Clock Gating Control         */
  __I  uint32_t  RESERVED37;
  __IO uint32_t  DCGCUSB;                           /*!< Universal Serial Bus Deep-Sleep Mode Clock Gating Control             */
  __I  uint32_t  RESERVED38[2];
  __IO uint32_t  DCGCCAN;                           /*!< Controller Area Network Deep-Sleep Mode Clock Gating Control          */
  __IO uint32_t  DCGCADC;                           /*!< Analog-to-Digital Converter Deep-Sleep Mode Clock Gating Control      */
  __IO uint32_t  DCGCACMP;                          /*!< Analog Comparator Deep-Sleep Mode Clock Gating Control                */
  __IO uint32_t  DCGCPWM;                           /*!< Pulse Width Modulator Deep-Sleep Mode Clock Gating Control            */
  __IO uint32_t  DCGCQEI;                           /*!< Quadrature Encoder Interface Deep-Sleep Mode Clock Gating Control     */
  __I  uint32_t  RESERVED39[4];
  __IO uint32_t  DCGCEEPROM;                        /*!< EEPROM Deep-Sleep Mode Clock Gating Control                           */
  __IO uint32_t  DCGCWTIMER;                        /*!< 32/64-Bit Wide General-Purpose Timer Deep-Sleep Mode Clock Gating
                                                         Control                                                               */
  __I  uint32_t  RESERVED40[104];
  __IO uint32_t  PRWD;                              /*!< Watchdog Timer Peripheral Ready                                       */
  __IO uint32_t  PRTIMER;                           /*!< 16/32-Bit General-Purpose Timer Peripheral Ready                      */
  __IO uint32_t  PRGPIO;                            /*!< General-Purpose Input/Output Peripheral Ready                         */
  __IO uint32_t  PRDMA;                             /*!< Micro Direct Memory Access Peripheral Ready                           */
  __I  uint32_t  RESERVED41;
  __IO uint32_t  PRHIB;                             /*!< Hibernation Peripheral Ready                                          */
  __IO uint32_t  PRUART;                            /*!< Universal Asynchronous Receiver/Transmitter Peripheral Ready          */
  __IO uint32_t  PRSSI;                             /*!< Synchronous Serial Interface Peripheral Ready                         */
  __IO uint32_t  PRI2C;                             /*!< Inter-Integrated Circuit Peripheral Ready                             */
  __I  uint32_t  RESERVED42;
  __IO uint32_t  PRUSB;                             /*!< Universal Serial Bus Peripheral Ready                                 */
  __I  uint32_t  RESERVED43[2];
  __IO uint32_t  PRCAN;                             /*!< Controller Area Network Peripheral Ready                              */
  __IO uint32_t  PRADC;                             /*!< Analog-to-Digital Converter Peripheral Ready                          */
  __IO uint32_t  PRACMP;                            /*!< Analog Comparator Peripheral Ready                                    */
  __IO uint32_t  PRPWM;                             /*!< Pulse Width Modulator Peripheral Ready                                */
  __IO uint32_t  PRQEI;                             /*!< Quadrature Encoder Interface Peripheral Ready                         */
  __I  uint32_t  RESERVED44[4];
  __IO uint32_t  PREEPROM;                          /*!< EEPROM Peripheral Ready                                               */
  __IO uint32_t  PRWTIMER;                          /*!< 32/64-Bit Wide General-Purpose Timer Peripheral Ready                 */
} SYSCTL_Type;
#define SYSCTL_Base 0x400FE000
#define SYSCTL ((SYSCTL_Type*) SYSCTL_Base)


/*-------------------------------------------------------------------------------
* SysTick timer registers
*------------------------------------------------------------------------------*/
typedef struct
{
	__IO uint32_t CTRL;                  
	__IO uint32_t RELOAD;                   
	__IO uint32_t CURRENT;
	__I  uint32_t RESERVED;
	__IO uint32_t PRIO;
} SYSTICK_Type;
#define SYSTICK_Base 0xE000E010
#define SYSTICK ((SYSTICK_Type*) SYSTICK_Base)


/*-------------------------------------------------------------------------------
* GPIO registers
*------------------------------------------------------------------------------*/
typedef struct {                                    /*!< GPIOA Structure                                                       */
  __I  uint32_t  RESERVED[255];
  __IO uint32_t  DATA;                              /*!< GPIO Data                                                             */
  __IO uint32_t  DIR;                               /*!< GPIO Direction                                                        */
  __IO uint32_t  IS;                                /*!< GPIO Interrupt Sense                                                  */
  __IO uint32_t  IBE;                               /*!< GPIO Interrupt Both Edges                                             */
  __IO uint32_t  IEV;                               /*!< GPIO Interrupt Event                                                  */
  __IO uint32_t  IM;                                /*!< GPIO Interrupt Mask                                                   */
  __IO uint32_t  RIS;                               /*!< GPIO Raw Interrupt Status                                             */
  __IO uint32_t  MIS;                               /*!< GPIO Masked Interrupt Status                                          */
  __O  uint32_t  ICR;                               /*!< GPIO Interrupt Clear                                                  */
  __IO uint32_t  AFSEL;                             /*!< GPIO Alternate Function Select                                        */
  __I  uint32_t  RESERVED1[55];
  __IO uint32_t  DR2R;                              /*!< GPIO 2-mA Drive Select                                                */
  __IO uint32_t  DR4R;                              /*!< GPIO 4-mA Drive Select                                                */
  __IO uint32_t  DR8R;                              /*!< GPIO 8-mA Drive Select                                                */
  __IO uint32_t  ODR;                               /*!< GPIO Open Drain Select                                                */
  __IO uint32_t  PUR;                               /*!< GPIO Pull-Up Select                                                   */
  __IO uint32_t  PDR;                               /*!< GPIO Pull-Down Select                                                 */
  __IO uint32_t  SLR;                               /*!< GPIO Slew Rate Control Select                                         */
  __IO uint32_t  DEN;                               /*!< GPIO Digital Enable                                                   */
  __IO uint32_t  LOCK;                              /*!< GPIO Lock                                                             */
  __IO uint32_t  CR;                                /*!< GPIO Commit                                                           */
  __IO uint32_t  AMSEL;                             /*!< GPIO Analog Mode Select                                               */
  __IO uint32_t  PCTL;                              /*!< GPIO Port Control                                                     */
  __IO uint32_t  ADCCTL;                            /*!< GPIO ADC Control                                                      */
  __IO uint32_t  DMACTL;                            /*!< GPIO DMA Control                                                      */
} GPIO_Type;
#define GPIOA_Base 0x40004000
#define GPIOB_Base 0x40005000
#define GPIOC_Base 0x40006000
#define GPIOD_Base 0x40007000
#define GPIOA ((GPIO_Type*) GPIOA_Base)
#define GPIOB ((GPIO_Type*) GPIOB_Base)
#define GPIOC ((GPIO_Type*) GPIOC_Base)
#define GPIOD ((GPIO_Type*) GPIOD_Base)


/*-------------------------------------------------------------------------------
* UART0 registers
*------------------------------------------------------------------------------*/
typedef struct {                                    /*!< UART0 Structure                                                       */
  __IO uint32_t  DR;                                /*!< UART Data                                                             */
  __IO uint32_t  RSR;                             /*!< UART Receive Status/Error Clear                                       */
  __I  uint32_t  RESERVED[4];
  __IO uint32_t  FR;                                /*!< UART Flag                                                             */
  __I  uint32_t  RESERVED1;
  __IO uint32_t  ILPR;                              /*!< UART IrDA Low-Power Register                                          */
  __IO uint32_t  IBRD;                              /*!< UART Integer Baud-Rate Divisor                                        */
  __IO uint32_t  FBRD;                              /*!< UART Fractional Baud-Rate Divisor                                     */
  __IO uint32_t  LCRH;                              /*!< UART Line Control                                                     */
  __IO uint32_t  CTL;                               /*!< UART Control                                                          */
  __IO uint32_t  IFLS;                              /*!< UART Interrupt FIFO Level Select                                      */
  __IO uint32_t  IM;                                /*!< UART Interrupt Mask                                                   */
  __IO uint32_t  RIS;                               /*!< UART Raw Interrupt Status                                             */
  __IO uint32_t  MIS;                               /*!< UART Masked Interrupt Status                                          */
  __O  uint32_t  ICR;                               /*!< UART Interrupt Clear                                                  */
  __IO uint32_t  DMACTL;                            /*!< UART DMA Control                                                      */
  __I  uint32_t  RESERVED2[22];
  __IO uint32_t  _9BITADDR;                         /*!< UART 9-Bit Self Address                                               */
  __IO uint32_t  _9BITAMASK;                        /*!< UART 9-Bit Self Address Mask                                          */
  __I  uint32_t  RESERVED3[965];
  __IO uint32_t  PP;                                /*!< UART Peripheral Properties                                            */
  __I  uint32_t  RESERVED4;
  __IO uint32_t  CC;                                /*!< UART Clock Configuration                                              */
} UART_Type;
#define UART0_Base 0x4000C000
#define UART0 ((UART_Type*) UART0_Base)


/*-------------------------------------------------------------------------------
* Coprocessor Access Control Register (FPU)
*------------------------------------------------------------------------------*/
#define CPASR *((__O uint32_t*) 0xE000ED88)


