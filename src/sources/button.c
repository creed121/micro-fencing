/**
 * @file button.c
 * @brief Button driver for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

// File: main.c
// Compiler: XC8 (MPLAB X)
// Target: PIC18F25K22
// Purpose: Play melody on CCP5 (RA4) when button on RB0 is pressed.
// Note: This is the "paste-in-fix" version that avoids "inline delay argument must be constant".

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define _XTAL_FREQ 800000UL   // <-- Set this to your CPU frequency (Hz). Original code used 1 MHz.

#pragma config FOSC = INTIO67, PLLCFG = OFF, PRICLKEN = ON, FCMEN = ON, PWRTEN = ON
#pragma config BOREN = SBORDIS, BORV = 250, WDTEN = OFF, WDTPS = 4096, PBADEN = OFF
#pragma config HFOFST = OFF, MCLRE = EXTMCLR, STVREN = ON, LVP = OFF, DEBUG = OFF

// NOTE: UNIT_MS must be a compile-time constant so __delay_ms(UNIT_MS) is valid inside the helper.
#define UNIT_MS 75  // tempo unit in milliseconds. Tweak for tempo (50 = faster, 150 = slower).

// Your note definitions
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
#define e5  0x5e
#define f5  0x59
#define g5  0x4F
#define a5  0x46
#define b5  0x3E
#define c6  0x3B

unsigned char pr_arr[32] = {
    e5, b4, c5, d5, c5, b4, a4, a4,
    c5, e5, d5, c5, b4, b4, c5, d5,
    e5, c5, a4, a4, null, d5, f5, a5,
    g5, f5, e5, c5, e5, d5, c5, b4
};

unsigned char dura[32] = {
    6,3,3,6,3,3,6,3,3,6,3,3,6,3,3,6,
    6,6,6,6,6,6,6,3,6,3,3,6,3,6,3
};

const uint8_t MELODY_LENGTH = 32;

// --------- Helper delays to avoid compile-time constant error ----------
void delay_ms_runtime(uint16_t ms)
{
    // Calls __delay_ms(1) repeatedly — safe and simple.
    while (ms--) {
        __delay_ms(1);
    }
}

void delay_units(uint16_t units)
{
    // UNIT_MS is a compile-time constant macro, so __delay_ms(UNIT_MS) is allowed.
    while (units--) {
        __delay_ms(UNIT_MS);
    }
}
// ---------------------------------------------------------------------

// PWM/IO helpers
void init_osc(void)
{
    // Use internal oscillator setting as in your original project.
    OSCCON = 0x30; // selects internal oscillator (1 MHz for these OSCCON bits)
}

void init_io(void)
{
    // RA4 (CCP5) output
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;

    // Make ports digital
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;

    // Button on RB0 (active low)
    TRISBbits.TRISB0 = 1;

    // Enable weak pull-ups on PORTB if you want (device-specific)
    INTCON2bits.RBPU = 0;
}

void init_pwm_ccp5(void)
{
    // Configure CCP5 as PWM (CCP5CON<3:0> = 1100 = 0x0C)
    CCP5CON = 0x0C;

    // Ensure CCP4/5 use Timer2 as PWM timebase
    CCPTMRS1 = 0x00;

    // Timer2 prescaler 1:1, Timer2 off for now
    T2CONbits.T2CKPS = 0b00;
    T2CONbits.TMR2ON  = 0;

    // clear PR2 and duty registers
    PR2 = 0;
    CCPR5L = 0;
}

void pwm_start(void)
{
    PIR1bits.TMR2IF = 0;
    T2CONbits.TMR2ON = 1;
}

void pwm_stop(void)
{
    T2CONbits.TMR2ON = 0;
    PR2 = 0;
    CCPR5L = 0;
    LATAbits.LATA4 = 0;
}

void play_melody_once(void)
{
    uint8_t i;
    for (i = 0; i < MELODY_LENGTH; i++) {
        uint8_t p = pr_arr[i];

        if (p == null) {
            // rest: stop PWM for the duration
            pwm_stop();
            delay_units(dura[i]);   // runtime delay using UNIT_MS chunks
        } else {
            // set PR2 to the note value and set 50% duty by CCPR5L = PR2/2
            PR2 = p;
            CCPR5L = (uint8_t)(PR2 >> 1); // 50% duty
            pwm_start();
            delay_units(dura[i]);   // runtime delay
            // small gap between notes
            pwm_stop();
            delay_units(1);         // short pause (1 unit)
        }
    }
}

void main(void)
{
    init_osc();
    init_io();
    init_pwm_ccp5();

    while (1) {
        // Wait for active-low button press
        
            if (PORTBbits.RB0 == 0) {
                // Play the melody once
                play_melody_once();
                // simple post-press debounce: wait until button released
                while (PORTBbits.RB0 == 0) { delay_ms_runtime(10); }
                delay_ms_runtime(50);
            }
        
    }
}
