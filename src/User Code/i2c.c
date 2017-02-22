/*******************************************************************************
* File:     	i2c.c
* Brief:    	I2C serial protocol driver
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	15/02/2017
* Last mod: 	21/02/2017
*
* Note: 
*******************************************************************************/
#include "KrisOS.h"
#include "i2c.h"



/*-------------------------------------------------------------------------------
* Function:    	i2c_init
* Purpose:    	Configure the I2C Module as Master with 100kb/s transmission rate.
* Arguments:	-
* Returns: 		-	
--------------------------------------------------------------------------------*/
void i2c_init(void) {
	
	// Activate the I2C0 module and enable clock on port B. Add a small delay.
	SYSCTL->RCGCI2C |= 1 <<	RCGC_I2C0;
	SYSCTL->RCGCGPIO |= (1 << RCGCGPIO_PORTB); 
	while ((SYSCTL->RCGCGPIO & (1 << RCGCGPIO_PORTB)) == 0);
	
	// Enable digital IO and alternative function on PB2 and PB3. Set the SDA pin
	// for open-drain operation
	GPIOB->AFSEL |= (1 << PIN2) | (1 << PIN3);
	GPIOB->DEN |= (1 << PIN2) | (1 << PIN3);
	GPIOB->ODR |= 1 << PIN3;

	// Set PB2 and PB3 to I2C mode
	GPIOB->PCTL &= ~((0xFU << PCTL_PMC2) | (0xFU << PCTL_PMC3)); 
	GPIOB->PCTL |= (0x3 << PCTL_PMC2) | (0x3 << PCTL_PMC3); 
	
	// Initialise I2C0 to Master and set the SCL clock speed to 100kb/s
	I2C0->MCR = 1 << MCR_MFE;
	I2C0->MTPR = SYSTEM_CLOCK_FREQ / (20 * 100000) - 1;
}



/*-------------------------------------------------------------------------------
* Function:    	i2c_slave_addr
* Purpose:    	Set the slave address for the following I2C communication.
* Arguments:
*		address - slave address (7bit)
* Returns: 		-	
--------------------------------------------------------------------------------*/
void i2c_slave_addr(uint8_t address) {	
	I2C0->MSA = address << MSA_SA;
}



/*-------------------------------------------------------------------------------
* Function:    	i2c_write
* Purpose:    	Send a byte of data to the current slave. 
* Arguments:
*		data - data to send
*		start - controls the generation of START condition
*		stop - controls the generation of STOP condition
* Returns: 
*		EXIT STATUS
--------------------------------------------------------------------------------*/
uint32_t i2c_write(uint8_t data, StartCond start, StopCond stop) {
	
	// Used to set the value of Master control status register
	uint8_t config = 0;
	
	// Wait while the I2C module is busy.
	while (I2C0->MCS & (1 << MCS_BUSY));
	
	// Configure the generation of START and STOP conditions 
	if (start == START)
		config |= 1 << MCS_START;
	if (stop == STOP)
		config |= 1 << MCS_STOP;
	
	// Set the next Master operation to transmit and update the data register
	// with the byte to send. 
	I2C0->MSA &= ~(1 << MSA_RS);
	I2C0->MDR = data;

	// Trigger the send operation
	config |= 1 << MCS_RUN;
	I2C0->MCS = config;
	
	// Wait for the operation to complete. If an error occurred generate the STOP
	// condition.
	while (I2C0->MCS & (1 << MCS_BUSY));
	if (I2C0->MCS & (1 << MCS_ERROR)) {
		I2C0->MCS = 0x04;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------------------
* Function:    	i2c_read
* Purpose:    	Receive a byte of data from the current slave. 
* Arguments:
*		start - controls the generation of START condition
*		stop - controls the generation of STOP condition
* Returns: 
*		EXIT STATUS
--------------------------------------------------------------------------------*/
uint8_t i2c_read(StartCond start, StopCond stop) {

	uint8_t config = 0;
	
	// Configure the START and STOP conditions 
	if (start == START)
		config |= 1 << MCS_START;
	if (stop == STOP)
		config |= 1 << MCS_STOP;

	// Set the next Master operation to receive and prepare to trigger the receive
	// request
	I2C0->MSA |= 1 << MSA_RS;
	config |= 1 << MCS_RUN;
	
	// Wait while the I2C module is busy. Then, trigger the read operation.
	while (I2C0->MCS & (1 << MCS_BUSY));
	I2C0->MCS = config;

	// Wait for the operation to complete. If an error occurred generate the STOP
	// condition
	while (I2C0->MCS & (1 << MCS_BUSY));
	if (I2C0->MCS & (1 << MCS_ERROR)) {
		//I2C0->MCS = 0x04;
		return EXIT_FAILURE;
	}
	
	// Return the data received from slave
	return I2C0->MDR;
}


