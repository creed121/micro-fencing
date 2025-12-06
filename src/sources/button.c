/**
 * @file button.c
 * @brief Button driver for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#include "../includes/button.h"

// #include "./button.h"

#define _XTAL_FREQ 16000000UL   // CPU Frequency in Hz

// #pragma config FOSC = INTIO67, PLLCFG = OFF, PRICLKEN = ON, FCMEN = ON, PWRTEN = ON
// #pragma config BOREN = SBORDIS, BORV = 250, WDTEN = OFF, WDTPS = 4096, PBADEN = OFF
// #pragma config HFOFST = OFF, MCLRE = EXTMCLR, STVREN = ON, LVP = OFF, DEBUG = OFF

unsigned char pr_arr[32] = {
    e5, b4, c5, d5, c5, b4, a4, a4,
    c5, e5, d5, c5, b4, b4, c5, d5,
    e5, c5, a4, a4, rest, d5, f5, a5,
    g5, f5, e5, c5, e5, d5, c5, b4
};

unsigned char dura[32] = {
    6,3,3,6,3,3,6,3,3,6,3,3,6,3,3,6,
    6,6,6,6,6,6,6,3,6,3,3,6,3,6,3
};

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

void button_init(void)
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

        if (p == null)
        {
            // rest: stop PWM for the duration
            pwm_stop();
            delay_units(dura[i]);   // runtime delay using UNIT_MS chunks
        } 
        else
        {
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
    // Wait for active-low button press
    if (!PORTBbits.RB0)
    {
        // Play the melody once
        play_melody_once();
        // simple post-press debounce: wait until button released
        while (!PORTBbits.RB0)
        {
            delay_ms_runtime(10);
        }
    }

}
