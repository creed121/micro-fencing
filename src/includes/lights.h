/**
 * @file lights.h
 * @brief LED control driver using PWM for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */
#ifndef LIGHTS_H
#define LIGHTS_H

#include <xc.h>

/**
 * PWM Channels:
 * - Red:   CCP1 on RC2 (PWM1) - hardwired, no config option
 * - Green: CCP2 on RB3 (PWM2) - configured via CCP2MX = PORTB3
 * - Blue:  CCP3 on RB5 (PWM3) - configured via CCP3MX = PORTB5
 * 
 * Timer2 is used as the PWM time base for all CCP modules
 * Frequency: 1 kHz (adjustable via PR2 and Timer2 prescaler)
 * Resolution: 8-bit PWM (0-255 duty cycle)
 */

/**
 * @brief Initialize PWM modules for RGB LED control.
 * 
 * Configures:
 * - Timer2 as PWM time base (1 kHz, 8-bit resolution)
 * - CCP1 (Red) on RC2
 * - CCP2 (Green) on RB3
 * - CCP3 (Blue) on RB5
 * 
 * @return void
 */
void lights_init(void);

/**
 * @brief Set the RGB LED color using PWM duty cycles.
 * 
 * @param r Red component (0-255, where 0=off, 255=full brightness)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @return void
 */
void lights_set_color(unsigned char r, unsigned char g, unsigned char b);

/**
 * @brief Set only the red LED brightness.
 * 
 * @param brightness Red PWM duty cycle (0-255)
 * @return void
 */
void lights_set_red(unsigned char brightness);

/**
 * @brief Set only the green LED brightness.
 * 
 * @param brightness Green PWM duty cycle (0-255)
 * @return void
 */
void lights_set_green(unsigned char brightness);

/**
 * @brief Set only the blue LED brightness.
 * 
 * @param brightness Blue PWM duty cycle (0-255)
 * @return void
 */
void lights_set_blue(unsigned char brightness);

/**
 * @brief Turn off all RGB LEDs.
 * 
 * Sets all PWM duty cycles to 0.
 * 
 * @return void
 */
void lights_off(void);

#endif // LIGHTS_H
