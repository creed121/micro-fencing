/**
 * @file main.c
 * @brief Main entry point for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#include "../includes/main.h"
#include "../includes/i2c.h"
#include "../includes/accelerometer.h"

/* ============================================================================
 * Port Configuration
 * ============================================================================ */
/**
 * @brief Configure PORTA as digital output for RGB LED (bits 0-2)
 *        Configure PORTB as input for SSP2 I2C (bits 2-3: SCL2, SDA2)
 */
void configure_ports(void)
{
    // PORTA: Bits 0-2 as outputs (for RGB)
    // Set RA0, RA1, RA2 as outputs
    TRISA = 0xF8;  // TRISA[7:3] = 1 (input), TRISA[2:0] = 0 (output)
    PORTA = 0x00;  // Initialize all to 0
    
    // PORTB: Bits 2-3 configured for SSP2 (I2C)
    // SSP2SCL = RB2, SSP2SDA = RB3
    // Configure as digital I/O (not analog)
    ANSELB = 0x00;  // All PORTB pins digital
    TRISB = 0xFF;   // All PORTB as inputs (SSP2 controls I2C lines)
}

/**
 * @brief Configure SSP2 module for I2C communication
 *        400 kHz I2C clock as specified in MPU-6050 datasheet
 */
void configure_ssp2_i2c(void)
{
    // Assuming 16 MHz oscillator
    // For 400 kHz I2C: Baud Rate = Fosc / (4 * (SSP2ADD + 1))
    // 400000 = 16000000 / (4 * (SSP2ADD + 1))
    // SSP2ADD = 9
    
    SSP2ADD = 9;
    
    // SSP2CON1: Configure as I2C Master mode
    // SSPM3:SSPM0 = 1000 (I2C Master mode)
    SSP2CON1 = 0x28;  // SSPEN = 1 (enable), SSPM = 1000
    
    // SSP2CON2: Initialize
    SSP2CON2 = 0x00;
    
    // SSP2STAT: Configure slew rate
    SSP2STAT = 0x80;  // SMP = 1 (slew rate disabled for 400 kHz)
}

/**
 * @brief Output RGB values to PORTA (bits 0-2)
 *        RA0 = Red, RA1 = Green, RA2 = Blue
 */
void output_rgb_to_porta(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char porta_value = 0x00;
    
    // Set RA0 if Red component is > 127
    if (r > 127) {
        porta_value |= 0x01;  // RA0 = 1
    }
    
    // Set RA1 if Green component is > 127
    if (g > 127) {
        porta_value |= 0x02;  // RA1 = 1
    }
    
    // Set RA2 if Blue component is > 127
    if (b > 127) {
        porta_value |= 0x04;  // RA2 = 1
    }
    
    PORTA = porta_value;
}

/* ============================================================================
 * Main Function
 * ============================================================================ */
int main(void)
{
    acc_error_t acc_status;
    gyro_data_t gyro_data;
    moving_avg_t speed_avg;
    unsigned int speed;
    unsigned int avg_speed;
    unsigned char r, g, b;
    
    // Configure I/O ports
    configure_ports();
    
    // Configure SSP2 for I2C communication
    configure_ssp2_i2c();
    
    // Initialize accelerometer
    acc_status = accelerometer_init();
    if (acc_status != ACC_SUCCESS) {
        // Initialization failed; flash LED as error indicator
        while (1) {
            PORTA = 0x01;  // Red LED on
            // Delay loop (simplified)
            unsigned int delay;
            for (delay = 0; delay < 30000; delay++);
            
            PORTA = 0x00;  // Red LED off
            for (delay = 0; delay < 30000; delay++);
        }
    }
    
    // Initialize moving average buffer
    accelerometer_reset_moving_avg(&speed_avg);
    
    // Main loop: continuously read gyro and update LED color
    while (1) {
        // Read gyroscope data from all three axes
        acc_status = accelerometer_read_gyro(&gyro_data);
        
        if (acc_status == ACC_SUCCESS) {
            // Calculate angular velocity magnitude
            speed = accelerometer_calculate_magnitude(&gyro_data);
            
            // Update moving average with new speed measurement
            accelerometer_update_moving_avg(&speed_avg, speed);
            
            // Get current moving average (0 if buffer not full yet)
            avg_speed = accelerometer_get_moving_avg(&speed_avg);
            
            // Map averaged speed to RGB color
            accelerometer_speed_to_color(avg_speed, &r, &g, &b);
            
            // Output RGB values to PORTA
            output_rgb_to_porta(r, g, b);
        } else {
            // I2C error; turn off LED
            PORTA = 0x00;
        }
        
        // Small delay to prevent I2C bus saturation
        // Adjust based on your desired sampling rate
        unsigned int delay;
        for (delay = 0; delay < 5000; delay++);
    }
    
    return 0;
}