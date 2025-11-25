/**
 * @file lights.c
 * @brief LED control driver using hardware PWM for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#include "../includes/lights.h"

static unsigned char current_r = 0;
static unsigned char current_g = 0;
static unsigned char current_b = 0;

static unsigned char fade_target_r = 0;
static unsigned char fade_target_g = 0;
static unsigned char fade_target_b = 0;
static unsigned char fade_steps_remaining = 0;
static unsigned char fade_total_steps = 0;

/**
 * @brief Initialize PWM modules for RGB LED control.
 * 
 * Configuration Details:
 * - Fosc = 16 MHz
 * - Timer2 Prescaler = 16 (Fosc/4/16 = 250 kHz)
 * - PR2 = 249 (250 kHz / 250 = 1 kHz PWM frequency)
 * - PWM Resolution: 8-bit (0-255 duty cycle)
 * 
 * CCP Module Setup:
 * - CCP1 (Red):   RC2, mode = PWM (1100)
 * - CCP2 (Green): RB3, mode = PWM (1100)
 * - CCP3 (Blue):  RB5, mode = PWM (1100)
 */
void lights_init(void)
{
	// Configure Timer2 for PWM time base
	// T2CON: Prescaler = 16 (bits 1:0 = 11 means 1:16)
	T2CON = 0x06;  // Timer2 ON, Prescaler 1:16, Postscaler 1:1
	
	// Set PWM period (PR2)
	// PWM Freq = Fosc / (4 * Prescaler * (PR2 + 1))
	// 1000 = 16MHz / (4 * 16 * (PR2 + 1))
	// PR2 = 249
	PR2 = 249;
	
	// Configure PORTC, PORTB as digital outputs
	TRISC = 0xFB;  // RC2 (CCP1) as output
	TRISB = 0xD7;  // RB3 (CCP2) and RB5 (CCP3) as outputs
	
	// Configure CCP1 (Red) on RC2
	// CCP1CON: mode = PWM (1100), DCxB = 00
	CCP1CON = 0x0C;  // PWM mode
	CCPR1L = 0;      // Initialize duty cycle to 0
	
	// Configure CCP2 (Green) on RB3
	// CCP2CON: mode = PWM (1100), DCxB = 00
	CCP2CON = 0x0C;  // PWM mode
	CCPR2L = 0;      // Initialize duty cycle to 0
	
	// Configure CCP3 (Blue) on RB5
	// CCP3CON: mode = PWM (1100), DCxB = 00
	CCP3CON = 0x0C;  // PWM mode
	CCPR3L = 0;      // Initialize duty cycle to 0
	
	// Initialize fade state
	fade_steps_remaining = 0;
}

/**
 * @brief Set the RGB LED color using PWM duty cycles.
 */
void lights_set_color(unsigned char r, unsigned char g, unsigned char b)
{
	// Update current color
	current_r = r;
	current_g = g;
	current_b = b;
	
	// Reset fade state
	fade_steps_remaining = 0;
	
	// Set PWM duty cycles (CCPR1L, CCPR2L, CCPR3L hold upper 8 bits)
	// For 8-bit resolution, LSBs are 0
	CCPR1L = r;  // Red
	CCPR2L = g;  // Green
	CCPR3L = b;  // Blue
}

/**
 * @brief Set only the red LED brightness.
 */
void lights_set_red(unsigned char brightness)
{
	current_r = brightness;
	fade_steps_remaining = 0;
	CCPR1L = brightness;
}

/**
 * @brief Set only the green LED brightness.
 */
void lights_set_green(unsigned char brightness)
{
	current_g = brightness;
	fade_steps_remaining = 0;
	CCPR2L = brightness;
}

/**
 * @brief Set only the blue LED brightness.
 */
void lights_set_blue(unsigned char brightness)
{
	current_b = brightness;
	fade_steps_remaining = 0;
	CCPR3L = brightness;
}

/**
 * @brief Turn off all RGB LEDs.
 */
void lights_off(void)
{
	current_r = 0;
	current_g = 0;
	current_b = 0;
	fade_steps_remaining = 0;
	
	CCPR1L = 0;
	CCPR2L = 0;
	CCPR3L = 0;
}

/**
 * @brief Fade RGB LED to a target color over time.
 * 
 * Linear interpolation fade from current to target color.
 * Call this function repeatedly in main loop to achieve smooth fade.
 * 
 * Example usage:
 *   if (lights_fade_to(255, 0, 0, 20)) {
 *       // Fade in progress
 *       // Small delay here
 *   } else {
 *       // Fade complete
 *   }
 */
unsigned char lights_fade_to(unsigned char target_r,
							unsigned char target_g,
							unsigned char target_b,
							unsigned char steps)
{
	// If fade not in progress, initialize new fade
	if (fade_steps_remaining == 0)
	{
		fade_target_r = target_r;
		fade_target_g = target_g;
		fade_target_b = target_b;
		fade_total_steps = steps;
		fade_steps_remaining = steps;
		
		// If steps is 0, just set color immediately
		if (steps == 0)
		{
			lights_set_color(target_r, target_g, target_b);
			return 0;
		}
	}
	
	if (fade_steps_remaining == 0)
	{
		return 0;  // Fade complete
	}
	
	// Calculate intermediate color using linear interpolation
	// new_value = current + (target - current) * (steps_done / total_steps)
	signed int r_diff = (signed int)fade_target_r - (signed int)current_r;
	signed int g_diff = (signed int)fade_target_g - (signed int)current_g;
	signed int b_diff = (signed int)fade_target_b - (signed int)current_b;
	
	unsigned char steps_done = fade_total_steps - fade_steps_remaining;
	
	// Calculate interpolated values
	// Using integer arithmetic to avoid floating point
	unsigned char new_r = current_r + (unsigned char)((r_diff * (signed int)steps_done) / (signed int)fade_total_steps);
	unsigned char new_g = current_g + (unsigned char)((g_diff * (signed int)steps_done) / (signed int)fade_total_steps);
	unsigned char new_b = current_b + (unsigned char)((b_diff * (signed int)steps_done) / (signed int)fade_total_steps);
	
	lights_set_color(new_r, new_g, new_b);
	
	fade_steps_remaining--;
	
	return (fade_steps_remaining > 0) ? 1 : 0;
}