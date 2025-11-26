/**
 * @file i2c.c
 * @brief I2C implementation for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

//#include "../includes/i2c.h"
#include "i2c.h"
#define slave_addr 0xD0  // MPU6050 I2C address with AD0 low (shifted)

#define MPU6050_7BIT_ADDR 0x68  // 7-bit address

void i2c_single_write(unsigned char reg, unsigned char data)
{
    unsigned char addr_byte = (MPU6050_7BIT_ADDR << 1) | 0x00; // write (R/W = 0)

    // Send Start
    SSP2CON2bits.SEN = 1;
    while (SSP2CON2bits.SEN); // wait for start to complete

    // Send slave address (write)
    SSP2BUF = addr_byte;
    while (SSP2STATbits.BF); // wait for buffer to be shifted out (transmit started)
    // Check ACK
    if (SSP2CON2bits.ACKSTAT) {
        // NACK received - handle error or return early
        SSP2CON2bits.PEN = 1; while (SSP2CON2bits.PEN);
        return;
    }

    // Send register address
    SSP2BUF = reg;
    while (SSP2STATbits.BF);
    if (SSP2CON2bits.ACKSTAT) {
        SSP2CON2bits.PEN = 1; while (SSP2CON2bits.PEN);
        return;
    }

    // Send data
    SSP2BUF = data;
    while (SSP2STATbits.BF);
    if (SSP2CON2bits.ACKSTAT) {
        // NACK on data byte
    }

    // Send Stop
    SSP2CON2bits.PEN = 1;
    while (SSP2CON2bits.PEN);
}

unsigned char i2c_single_read(unsigned char reg)
{
    // Using SSP2 Module
	// Send start bit
	SSP2CON2bits.SEN = 1;
	while(SSP2CON2bits.SEN);
    
	// Send Slave address (in write mode, to select register)
	SSP2BUF = slave_addr | 0x00;
	while(SSP2STATbits.R_NOT_W);
	// Check ACK; Technically unnecessary and liable to cause problems
	/**/
	if (SSP2CON2bits.ACKSTAT)
	{
		// Abort
	}
	/**/
    
	// Send Register address
	SSP2BUF = reg;
	while(SSP2STATbits.R_NOT_W);

    // Check ACK if desired
	if (SSP2CON2bits.ACKSTAT)
	{
		// Abort
	}
    
	// Send Restart Bit
	SSP2CON2bits.RSEN = 1;
	while(SSP2CON2bits.RSEN);
    
	// Send Slave address, in Read mode
	SSP2BUF = slave_addr | 0x01;
	while(SSP2STATbits.R_NOT_W);

    // Check ACK if desired
	if (SSP2CON2bits.ACKSTAT)
	{
		// Abort
	}
    
	// Wait to receive byte and for buffer to fill
	SSP2CON2bits.RCEN = 1;
	while(SSP2CON2bits.RCEN && !SSP2STATbits.BF);
	// Acknowledge (NACK since only reading one byte)
	/**/
	SSP2CON2bits.ACKDT = 1;
	SSP2CON2bits.ACKEN = 1;
	while (SSP2CON2bits.ACKEN);
	/**/
    
	// Send Stop bit
	SSP2CON2bits.PEN = 1;
	while(SSP2CON2bits.PEN);
    
	// Return received byte
	return SSP2BUF;
}

void i2c_bulk_write(unsigned char reg, unsigned char* data, unsigned char length)
{
    unsigned char i;
	// Using SSP2 Module
	// Send start bit and wait for it to complete
	// Note that SEN is macro-defined as SSPCON2bits.SEN
	SSP2CON2bits.SEN = 1;
	while(SSP2CON2bits.SEN);
    
	// Send Slave_Address + R/W
	SSP2BUF = slave_addr | 0x00;
	while (SSP2STATbits.R_NOT_W);
	// Check ACK; Technically unnecessary and liable to cause problems
	/**/
	if (SSP2CON2bits.ACKSTAT)
	{
		// Abort
	}
	/**/
    
	// Send Slave Register Address
	SSP2BUF = reg;
	while (SSP2STATbits.R_NOT_W);
	// Check ACK if desired
    
	for (i = 0; i < length; i++)
	{
		// Write Data
		SSP2BUF = data[i];
		while (SSP2STATbits.R_NOT_W);
		// Check ACK if desired
		/**/
		if (SSP2CON2bits.ACKSTAT)
		{
			// Abort
		}
		/**/
	}
    
	// Send Stop bit
	SSP2CON2bits.PEN = 1;
	while (SSP2CON2bits.PEN);
}

void i2c_bulk_read(unsigned char reg, unsigned char* buffer, unsigned char length)
{
    unsigned char i;
	// Using SSP2 Module
	// Send start bit
	SSP2CON2bits.SEN = 1;
	while(SSP2CON2bits.SEN);
    
	// Send Slave address (in write mode, to select register)
	SSP2BUF = slave_addr | 0x00;
	while(SSP2STATbits.R_NOT_W);
	// Check ACK; Technically unnecessary and liable to cause problems
	/**/
	if (SSP2CON2bits.ACKSTAT)
	{
		// Abort
	}
	/**/
    
	// Send Register address
	SSP2BUF = reg;
	while(SSP2STATbits.R_NOT_W);

    // Check ACK if desired
	if (SSP2CON2bits.ACKSTAT)
	{
		// Abort
	}
    
	// Send Restart Bit
	SSP2CON2bits.RSEN = 1;
	while(SSP2CON2bits.RSEN);
    
	// Send Slave address, in Read mode
	SSP2BUF = slave_addr | 0x01;
	while(SSP2STATbits.R_NOT_W);

    // Check ACK if desired
	if (SSP2CON2bits.ACKSTAT)
	{
		// Abort
	}
    
	for (i = 0; i < length; i++)
	{
		// Wait to receive byte and for buffer to fill
		SSP2CON2bits.RCEN = 1;
		while(SSP2CON2bits.RCEN && !SSP2STATbits.BF);
		// Store received byte in buffer
		buffer[i] = SSP2BUF;
		// Acknowledge (if desired)
		/**/
		// ACK all bytes except last, NACK on last
		SSP2CON2bits.ACKDT = (i != (length - 1)) ? 0 : 1;
		SSP2CON2bits.ACKEN = 1;
		while (SSP2CON2bits.ACKEN);
		/**/
		
	}
    
	// Send Stop bit
	SSP2CON2bits.PEN = 1;
	while(SSP2CON2bits.PEN);
}