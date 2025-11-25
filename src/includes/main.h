/**
 * @file main.h
 * @brief Main header file for the Micro-Fencing project with pragma configurations.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#ifndef MAIN_H
#define MAIN_H

#include <pic18f25k22.h>

/* ============================================================================
 * Configuration Bits (for PIC18F25K22)
 * ============================================================================ */
#pragma config FOSC = HSMP      // HS oscillator (medium power)
#pragma config PLLCFG = OFF     // PLL disabled
#pragma config PRICLKEN = ON    // Primary oscillator enabled
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor disabled
#pragma config IESO = OFF       // Internal/External Switchover disabled
#pragma config PWRTEN = ON      // Power-up Timer enabled
#pragma config BOREN = OFF      // Brown-out Reset disabled
#pragma config WDTEN = OFF      // Watchdog Timer disabled
#pragma config WDTPS = 32768    // Watchdog Postscaler 1:32768
#pragma config CCP2MX = PORTBE  // CCP2 on RB3
#pragma config PBADEN = OFF     // PORTB pins configured as digital I/O
#pragma config CCP3MX = PORTB5  // CCP3/PWM3 on RB5
#pragma config HFOFST = ON      // HFINTOSC output on CLKOUT disabled
#pragma config T3CMX = PORTB5   // Timer3 Gate on RB5
#pragma config P2BMX = PORTB5   // ECCP2B (PWM2B) on RB5
#pragma config MCLRE = EXTMCLR  // MCLR pin enabled
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset enabled
#pragma config LVP = OFF        // Low Voltage Programming disabled
#pragma config XINST = OFF      // Extended Instruction Set disabled
#pragma config DEBUG = OFF      // Debugger disabled
#pragma config CP0 = OFF        // Code Protect Block 0 disabled
#pragma config CP1 = OFF        // Code Protect Block 1 disabled
#pragma config CP2 = OFF        // Code Protect Block 2 disabled
#pragma config CP3 = OFF        // Code Protect Block 3 disabled
#pragma config CPB = OFF        // Boot Block Code Protect disabled
#pragma config CPD = OFF        // Data EEPROM Code Protect disabled
#pragma config WRT0 = OFF       // Block 0 Write Protect disabled
#pragma config WRT1 = OFF       // Block 1 Write Protect disabled
#pragma config WRT2 = OFF       // Block 2 Write Protect disabled
#pragma config WRT3 = OFF       // Block 3 Write Protect disabled
#pragma config WRTB = OFF       // Boot Block Write Protect disabled
#pragma config WRTD = OFF       // Data EEPROM Write Protect disabled
#pragma config EBTR0 = OFF      // Table Read Protect Block 0 disabled
#pragma config EBTR1 = OFF      // Table Read Protect Block 1 disabled
#pragma config EBTR2 = OFF      // Table Read Protect Block 2 disabled
#pragma config EBTR3 = OFF      // Table Read Protect Block 3 disabled
#pragma config EBTRB = OFF      // Boot Block Table Read Protect disabled

/* ============================================================================
 * Function Prototypes
 * ============================================================================ */

/**
 * @brief Configure PORTA and PORTB for I/O operations.
 * 
 * PORTA Configuration:
 * - RA0, RA1, RA2: Outputs for RGB LED control
 * - RA3-RA7: Inputs (unused)
 * 
 * PORTB Configuration:
 * - RB2, RB3: SSP2 I2C (SCL2, SDA2) - configured as inputs
 * - Other PORTB pins: Inputs (unused)
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

/**
 * @brief Output RGB values to PORTA (bits 0-2).
 * 
 * Pin Mapping:
 * - RA0: Red channel (output high if r > 127)
 * - RA1: Green channel (output high if g > 127)
 * - RA2: Blue channel (output high if b > 127)
 * 
 * @param r Red component value (0-255)
 * @param g Green component value (0-255)
 * @param b Blue component value (0-255)
 * @return void
 */
void output_rgb_to_porta(unsigned char r, unsigned char g, unsigned char b);

#endif  // MAIN_H
