/**
 * @file main.c
 * @brief Main entry point for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

//#include "../includes/main.h"
//#include "../includes/i2c.h"
//#include "../includes/accelerometer.h"
//#include "../includes/lights.h"
/**/
#include "main.h"
#include "i2c.h"
#include "accelerometer.h"
#include "lights.h"
#include <xc.h>
/**/
/**
 * @brief Configure PORTA as digital output for error indicator
 *        Configure PORTB for PWM/I2C (RB2-RB3: I2C, RB5: PWM)
 *        Configure PORTC for PWM (RC2: PWM Red)
 */
/**/
void configure_ports(void)
{
	// PORTA: RA0 as output for error indicator LED
	TRISA  = 0xFE;  // RA0 = output, RA1-RA7 = inputs
    ANSELA = 0x00;
	PORTA  = 0x00;  // Initialize to 0
	
	// PORTB and PORTC are configured by lights_init() for PWM
	// and by configure_ssp2_i2c() for I2C
}
/**/
/**
 * @brief Configure SSP2 module for I2C communication
 *        400 kHz I2C clock as specified in MPU-6050 datasheet
 */
/**/
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

   // Enable Internal Pull-ups for I2C lines on PORTB
   INTCON2bits.RBPU = 0;
}
/**/
/**/
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
	while (1)
	{
		// Read gyroscope data from all three axes
		acc_status = accelerometer_read_gyro(&gyro_data);
		
		if (acc_status == ACC_SUCCESS)
		{
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
		}
		else
		{
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
/**/

//#include <xc.h>

// ---------------------- CONFIGURATION BITS ----------------------
// (Adjust these if you use an external crystal or want PLL etc.)
//#pragma config FOSC = INTIO67   // Internal oscillator block, port function on RA6/RA7
//#pragma config PLLCFG = OFF
//#pragma config PRICLKEN = ON
//#pragma config FCMEN = OFF
//#pragma config IESO = OFF
//
//#pragma config PWRTEN = ON
//#pragma config BOREN = SBORDIS
//#pragma config BORV = 190
//
//#pragma config WDTEN = OFF
//#pragma config WDTPS = 32768
//
//#pragma config CCP2MX = PORTC1
//#pragma config PBADEN = OFF
//#pragma config CCP3MX = PORTB5
//#pragma config HFOFST = ON
//#pragma config T3CMX = PORTC0
//#pragma config P2BMX = PORTC0
//#pragma config MCLRE = EXTMCLR
//
//#pragma config STVREN = ON
//#pragma config LVP = OFF
//#pragma config XINST = OFF
//// ---------------------------------------------------------------
//
//#define _XTAL_FREQ 16000000UL   // must match oscillator config

//void initOsc(void) {
//    // Set internal oscillator to 16 MHz (IRCF = 111). SCS = 10 selects OSCCON IRC as system clock.
//    // Note: exact bit fields vary slightly by header; this is the commonly used pattern.
//    OSCCON = 0b01110010;   // IRCF2:0 = 111 (16MHz HFINTOSC), SCS1:SCS0 = 10 (internal oscillator block)
//    OSCTUNEbits.PLLEN = 0; // disable PLL (set =1 if you want to enable it and have hardware support)
//}

//void main(void) {
//    initOsc();
//
//    // Configure RA0 as digital output
//    ANSELAbits.ANSA0 = 0;         // disable analog on RA0 (ensure ANSELA exists in header)
//    TRISAbits.TRISA0 = 0;         // RA0 as output
//    LATAbits.LATA0 = 0;           // start LED OFF
//
//    while (1) {
//        LATAbits.LATA0 = 1;       // LED ON
//        __delay_ms(500);
//
//        LATAbits.LATA0 = 0;       // LED OFF
//        __delay_ms(500);
//    }
//}
