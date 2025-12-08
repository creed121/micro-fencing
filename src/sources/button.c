/**
 * @file button.c
 * @brief Button driver for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#include "../includes/button.h"

// #include "./button.h"

#define _XTAL_FREQ 16000000UL   // CPU Frequency in Hz

unsigned char pr_arr[32] = {
    e5, b4, c5, d5,   c5, b4, a4, a4,
    c5, e5, d5, c5,   b4, b4, c5, d5,
    e5, c5, a4, a4, rest, d5, f5, a5,
    g5, f5, e5, c5,   e5, d5, c5, b4
};

unsigned char dura[32] = {
    6, 3, 3, 6, 3, 3, 6, 3,
    3, 6, 3, 3, 6, 3, 3, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    3, 6, 3, 3, 6, 3, 6, 3
};

// --------- Helper delays to avoid compile-time constant error ----------
void delay_ms_runtime(uint16_t ms)
{
    while (ms--) {
        __delay_ms(1);
    }
}

void delay_units(uint16_t units)
{
    while (units--) {
        __delay_ms(UNIT_MS);
    }
}
// ---------------------------------------------------------------------

void button_init(void)
{
    // Configure CCP5 as PWM (CCP5CON<3:0> = 1100 = 0x0C)
    CCP5CON = 0x0C;

    // Ensure CCP5 uses Timer4 as PWM timebase
    CCPTMRS1bits.C5TSEL = 0b10;

    // Timer4 prescaler 1:1
    T4CONbits.T4CKPS = 0b00;
    // Timer4 off for now
    T4CONbits.TMR4ON  = 0;

    // Clear PR4 and duty registers
    PR4 = 0;
    CCPR5L = 0;
}

void pwm_start(void)
{
    // Clear interrupt flag (yes PIR5) and start Timer4
    PIR5bits.TMR4IF = 0;
    T4CONbits.TMR4ON = 1;
}

void pwm_stop(void)
{
    T4CONbits.TMR4ON = 0;
    PR4 = 0;
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
            delay_units(dura[i]);
        } 
        else
        {
            // set PR4 to the note value and set 50% duty by CCPR5L = PR4/2
            PR4 = p;
            CCPR5L = (uint8_t)(PR4 >> 1); // 50% duty
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
