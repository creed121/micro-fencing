/**
 * @file button.c
 * @brief Button driver for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#include "../includes/button.h"

// #include "./button.h"

#define _XTAL_FREQ 16000000UL   // CPU Frequency in Hz

unsigned char notes[MELODY_LENGTH] = {
    e5, b4, c5, d5, c5, b4, a4
};

unsigned char dura[MELODY_LENGTH] = {
    6, 3, 3, 6, 3, 3, 6
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
    CCPTMRS1 = 0x04;

    // Timer4 prescaler 1:16, w/ Timer4 off
    T4CON = 0x02;

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
}

void play_melody_once(void)
{
    uint8_t i;
    for (i = 0; i < MELODY_LENGTH; i++) {
        uint8_t curr_note = notes[i];

        // set PR4 to the note value and set 50% duty by CCPR5L = PR4/2
        PR4 = curr_note;
        CCPR5L = (uint8_t)(PR4 >> 1); // 50% duty
        pwm_start();
        delay_units(dura[i]);   // runtime delay
        // small gap between notes
        pwm_stop();
        delay_units(1);         // short pause (1 unit)
    }
}

void button_code(void)
{
    // Wait for low-active button press
    if (!PORTBbits.RB0)
    {
        // Simple debounce
        delay_ms_runtime(20);
        if (!PORTBbits.RB0)
        {
            play_melody_once();
        }
    }
}
