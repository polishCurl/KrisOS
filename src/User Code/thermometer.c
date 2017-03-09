/*******************************************************************************
* File:     	thermometer.c
* Brief:    	Digital thermometer user program
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	15/02/2017
* Last mod: 	09/03/2017
*
* Note: 
*	Digital thermometer program that periodically reads the TC74 digital temperature
*	sensor and displays the average temperature over TEMP_AVERAG_SAMPLE_NO samples.
*
*	The program consists of 2 tasks.  
*		1. The 'writer' task run once every 125ms that reads the current temperature
*		and writes it to a queue. The 125ms delay is because the maximum conversion 
*		rate for the TC74 is 8 samples/s.
*		2. The 'reader' tasks which waits for TEMP_AVERAG_SAMPLE_NO to be read from
*		the queue, averages them and displays on the nokia LCD screen. The queue buffer
*		size THERMOMETER_QUEUE_SIZE != TEMP_AVERAG_SAMPLE_NO as ussually the values 
*		are read from the queue just after they've been written (low workload of the CPU)
*
* 	Tiva C pin usage for the I2C thermometer:
*		PB2 - SCL
* 		PB3 - SDA
*
* 	Both SDA and SCL are pulled high using external 4.7kOhm resistors. The slave
*	address of the digital temperature sensor is 0x48
*******************************************************************************/
#include "KrisOS.h"
#include "thermometer.h"
#include "nokia5110.h"



/*-------------------------------------------------------------------------------
* Queue for sending temperature values between the 'thermometerWriter' and 
* 'thermometerReader' tasks
--------------------------------------------------------------------------------*/
Queue* thermometerQueue;



/*******************************************************************************
* Task: 	thermometerWriter
* Purpose: 	Thermometer task that periodically reads the temperature from TC74
*			and  passes the measurements to the thermometerQueue
*******************************************************************************/
void thermometerWriter(void) {
	
	// Current temperature
	int8_t temperature;
	
	// Initialise the inter-task queue
	thermometerQueue = KrisOS_queue_create(THERMOMETER_QUEUE_SIZE, sizeof(int8_t));
	
	// Initialise the I2C module for communicating with the temperature sensor
	i2c_init();
	i2c_slave_addr(0x48);
	
	while(1) {
		
		// Read the temperature from TC74
		i2c_write(0x00, START, STOP);
		temperature = i2c_read(START, STOP);
		
		// Write to the queue
		KrisOS_queue_write(thermometerQueue, &temperature);
			
		// The standard temperature converstion rate for TC74 is 8 samples/s so
		// a delay is necessary
		KrisOS_task_sleep(TEMPERATURE_CONVERSION_DELAY);
	}
}



/*******************************************************************************
* Task: 	thermometerReader
* Purpose: 	Read a predefined number of samples from the thermometer queue and
*			display their average on the LCD screen
*******************************************************************************/
void thermometerReader(void) {
	
	// Current temperature and the average temperature over TEMP_AVERAG_SAMPLE_NO
	// samples
	int8_t temperatureRead;
	int32_t temperatureAverage;
	
	// Sample sequence number, for averaging temperature to get more stable reading
	uint32_t sampleNo;
	
	while(1) {
		
		// Accumulate the temperature readings and compute their average
		temperatureAverage = 0;
		for (sampleNo = 0; sampleNo < TEMP_AVERAG_SAMPLE_NO; sampleNo++) {
			KrisOS_queue_read(thermometerQueue, &temperatureRead);
			temperatureAverage += temperatureRead;
		}
		temperatureAverage /= TEMP_AVERAG_SAMPLE_NO;
		
		// Display the temperature on the LCD screen
		KrisOS_mutex_lock(nokiaMtx);
		nokia5110_set_cursor(0, 0);
		fprintf(&nokia5110, "Temp: %3dC", temperatureAverage);
		nokia5110_set_cursor(0, 0);
		KrisOS_mutex_unlock(nokiaMtx);
	}
}



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
*		data - data to send (1 byte)
*		start - controls the generation of START condition
*		stop - controls the generation of STOP condition
* Returns: 
*		EXIT STATUS
--------------------------------------------------------------------------------*/
uint32_t i2c_write(uint8_t data, StartCond start, StopCond stop) {
	
	// Helper variable to set the value of Master control status register at once
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
		return EXIT_FAILURE;
	}
	
	// Return the data received from slave
	return I2C0->MDR;
}


