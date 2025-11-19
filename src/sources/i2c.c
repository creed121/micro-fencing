/**
 * @file i2c.c
 * @brief I2C implementation for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#include "../includes/i2c.h"
#define slave_addr 0xD0  // MPU6050 I2C address with AD0 low

 // ==========   I2C Write   ==========
void i2c_single_write(unsigned char reg, unsigned char data)
{
    // Using SSP Module
	// Send start bit and wait for it to complete
	// Note that SEN is macro-defined as SSPCON2bits.SEN
	SEN = 1;
	while(SEN);
    
	// Send Slave_Address + R/W
	SSPBUF = slave_addr + 0;
	while (R_NOT_W);
	// Check ACK; Technically unnecessary and liable to cause problems
	/**/
	if (ACKSTAT)
	{
		// Abort
	}
	/**/
    
	// Send Slave Register Address
	SSPBUF = reg;
	while (R_NOT_W);
	// Check ACK if desired
    
	// Write Data
	SSPBUF = data;
	while (R_NOT_W);
	// Check ACK if desired
	/**/
	if (ACKSTAT)
	{
		// Abort
	}
	/**/
    
	// Send Stop bit
	PEN = 1;
	while (PEN);
}
// ========== End I2C Write ==========

// ==========   I2C Read   ==========
unsigned char i2c_single_read(unsigned char reg)
{
    // Using SSP Module
	// Send start bit
	SEN = 1;
	while(SEN);
    
	// Send Slave address (in write mode, to select register)
	SSPBUF = slave_addr + 0;
	while(R_NOT_W);
	// Check ACK; Technically unnecessary and liable to cause problems
	/**/
	if (ACKSTAT)
	{
		// Abort
	}
	/**/
    
	// Send Register address
	SSPBUF = reg;
	while(R_NOT_W);

    // Check ACK if desired
	if (ACKSTAT)
	{
		// Abort
	}
    
	// Send Restart Bit
	RSEN = 1;
	while(RSEN);
    
	// Send Slave address, in Read mode
	SSPBUF = slave_addr + 1;
	while(R_NOT_W);

    // Check ACK if desired
	if (ACKSTAT)
	{
		// Abort
	}
    
	// Wait to receive byte and for buffer to fill
	RCEN = 1;
	while(RCEN);
	while(!BF);
	// Acknowledge (if desired)
	/**/
	ACKDT = 0;
	ACKEN = 1;
	while (ACKEN);
	/**/
    
	// Send Stop bit
	PEN = 1;
	while(PEN);
    
	// Return received byte
	return SSPBUF;

}
// ========== End I2C Read ==========