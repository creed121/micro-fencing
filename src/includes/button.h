#ifndef BUTTON_H
#define BUTTON_H

/**
 * @file button.h
 * @brief Header for button.c — button / PWM melody driver for Micro-Fencing project.
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------
 * Configuration Macros
 * ------------------------------------------------------------------ */

/** Tempo unit in milliseconds (compile-time constant).  
 *  Modify in project settings as needed.
 */
#define UNIT_MS 75

/** Note value definitions (PR2 register values for PWM frequency) */
#define null 0x00
#define b3  0xFC
#define c4  0xEE
#define d4  0xD4
#define e4  0xBD
#define f4  0xB2
#define g4  0x9E
#define a4  0x8D
#define b4  0x7D
#define c5  0x76
#define d5  0x69
#define e5  0x5E
#define f5  0x59
#define g5  0x4F
#define a5  0x46
#define b5  0x3E
#define c6  0x3B

/* ---------------------------------------------------------------------
 * External Melody Data
 * ------------------------------------------------------------------ */

extern unsigned char pr_arr[32];
extern unsigned char dura[32];
extern const uint8_t MELODY_LENGTH;

/* ---------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------ */

void button_init(void);
void pwm_start(void);
void pwm_stop(void);
void play_melody_once(void);
void button_code(void);

/* ---------------------------------------------------------------------
 * Delay Helpers
 * ------------------------------------------------------------------ */

void delay_ms_runtime(uint16_t ms);
void delay_units(uint16_t units);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_H */
