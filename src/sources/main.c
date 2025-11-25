/**
 * @file main.c
 * @brief Main entry point for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#include "../includes/main.h"
#include "../includes/i2c.h"
#include "../includes/accelerometer.h"
#include "../includes/lights.h"

/* ============================================================================
 * Port Configuration
 * ============================================================================ */
/**
 * @brief Configure PORTA as digital output for error indicator
 *        Configure PORTB for PWM/I2C (RB2-RB3: I2C, RB5: PWM)
 *        Configure PORTC for PWM (RC2: PWM Red)
 */
void configure_ports(void)
{
    // PORTA: RA0 as output for error indicator LED
    TRISA = 0xFE;  // RA0 = output, RA1-RA7 = inputs
    PORTA = 0x00;  // Initialize to 0
    
    // PORTB and PORTC are configured by lights_init() for PWM
    // and by configure_ssp2_i2c() for I2C
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

/* ============================================================================
 * Main Function
 * ============================================================================ */
int main(void)
{
   acc_error_t acc_status;
   gyro_data_t gyro_data;
   moving_avg_t speed_avg;
   unsigned int delay;
   unsigned int speed;
   unsigned int avg_speed;
   unsigned char r, g, b;
    
   // Configure I/O ports
   configure_ports();
   
   // Configure SSP2 for I2C communication
   configure_ssp2_i2c();
    
   // Initialize PWM for RGB LED control
   lights_init();
    
   // Initialize accelerometer
   acc_status = accelerometer_init();
   if (acc_status != ACC_SUCCESS)
   {
      // Initialization failed; flash error indicator on RA0
      lights_off();
      while (1)
      {
         PORTA = 0x01;  // Red error indicator on RA0
         // Delay loop (simplified)
         for (delay = 0; delay < 30000; delay++);
            
         PORTA = 0x00;  // Error indicator off
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
            
            // Set RGB LED color using PWM
            lights_set_color(r, g, b);
            
            // Clear error indicator
            PORTA = 0x00;
        } else {
            // I2C error; turn off LED and set error indicator
            lights_off();
            PORTA = 0x01;
        }
        
        // Small delay to prevent I2C bus saturation
        // Adjust based on your desired sampling rate
        for (delay = 0; delay < 5000; delay++);
    }
    
    return 0;
}