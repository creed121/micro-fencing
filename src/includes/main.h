/**
 * @file main.h
 * @brief Main header file for the Micro-Fencing project with pragma configurations.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */
#ifndef MAIN_H
#define MAIN_H

#include <xc.h>
#include "./accelerometer.h"
#include "./button.h"
#include "./lights.h"
#include "./i2c.h"

// Oscillator: HS oscillator at medium power (16 MHz)
#pragma config FOSC = INTIO67

// Power-up Timer: Enabled for stable startup
#pragma config PWRTEN = ON

// Port Configuration: PORTB as digital (disables analog), CCP routing
#pragma config PBADEN = OFF         // PORTB pins digital
#pragma config CCP2MX = PORTB3      // Use RB3 for CCP2 (Green PWM)
#pragma config CCP3MX = PORTB5      // Use RB5 for CCP3 (Blue PWM)

// Reset and Debug: MCLR enabled for programming, extended instructions OFF
#pragma config MCLRE = EXTMCLR  // MCLR pin enabled
#pragma config XINST = OFF      // Extended instructions OFF (SDCC compatibility)

// Optional but recommended: Stack protection, Watchdog OFF
#pragma config STVREN = ON      // Stack overflow reset
#pragma config WDTEN = OFF      // Watchdog timer disabled

// Add JavaDoc
void configure_osc(void);

/**
 * @brief Configure PORTA and PORTB for I/O operations.
 * 
 * PORTA Configuration:
 * - RA0: Output for error indicator LED
 * - RA1-RA7: Inputs (unused)
 * 
 * PORTB Configuration:
 * - RB2, RB3: SSP2 I2C (SCL2, SDA2)
 * - RB3: Also used as CCP2 PWM output (Green LED) when not I2C
 * - RB5: PWM Blue output (CCP3)
 * - Other PORTB pins: Inputs (unused)
 * 
 * PORTC Configuration:
 * - RC2: PWM Red output (CCP1 - hardwired, no config option)
 * 
 * Note: This is a hardware constraint - RB3 serves dual purpose but SSP2
 * takes priority for I2C communication. For PWM-only operation, use RC2/RB5.
 * 
 * @return void
 */
void configure_ports(void);

/**
 * @brief Configure SSP2 module for I2C Master mode.
 * 
 * Configuration:
 * - I2C Master mode (400 kHz clock)
 * - Fosc = 16 MHz
 * - SSP2ADD = 9 (for 400 kHz: Fosc / (4 * (SSP2ADD + 1)))
 * - Slew rate disabled for 400 kHz operation
 * 
 * @return void
 */
void configure_ssp2_i2c(void);

#endif // MAIN_H