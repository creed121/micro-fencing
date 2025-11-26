/**
 * @file i2c.h
 * @brief I2C implementation for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */
#ifndef I2C_H
#define I2C_H

#include <pic18f25k22.h>

/**
 * @brief Write a byte to a specific register of an I2C slave device.
 * @param reg The register address to write to.
 * @param data The byte to write.
 * @return void
 * Note: Slave address is hardcoded as 0xD0 for MPU6050 with AD0 low.
 */
void i2c_single_write(unsigned char reg, unsigned char data);

/**
 * @brief Read a byte from a specific register of an I2C slave device.
 * @param reg The register address to read from.
 * @return The byte read from the specified register.
 * Note: Slave address is hardcoded as 0xD0 for MPU6050 with AD0 low.
 */
unsigned char i2c_single_read(unsigned char reg);


// See pages 35-36 fo MPU6050 Datasheet
/**
 * @brief Write multiple bytes to consecutive registers of an I2C slave device.
 * @param reg The starting register address to write to.
 * @param data Pointer to the data buffer containing bytes to write.
 * @param length The number of bytes to write.
 * @return void
 * Note: Slave address is hardcoded as 0xD0 for MPU6050 with AD0 low.
 */
void i2c_bulk_write(unsigned char reg, unsigned char* data, unsigned char length);

/**
 * @brief Read multiple bytes from consecutive registers of an I2C slave device.
 * @param reg The starting register address to read from.
 * @param buffer Pointer to the buffer to store the read bytes.
 * @param length The number of bytes to read.
 * @return void
 * Note: Slave address is hardcoded as 0xD0 for MPU6050 with AD0 low.
 */
void i2c_bulk_read(unsigned char reg, unsigned char* buffer, unsigned char length);
#endif // I2C_H