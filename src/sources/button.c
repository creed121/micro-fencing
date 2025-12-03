/**
 * @file button.c
 * @brief Button driver for the Micro-Fencing project.
 * @author Christopher Reed, Micah Baker, Lydia Knierim, Samuel Prusia
 * @date 2025-11
 */

#include <xc.h>

// #pragma config PLLCFG = OFF
// #pragma config PRICLKEN = ON
// #pragma config FCMEN = ON
// #pragma config IESO = OFF
// #pragma config PWRTEN = ON
// #pragma config BOREN = SBORDIS
// #pragma config BORV = 190
// #pragma config WDTEN = OFF
// #pragma config MCLRE = EXTMCLR
// #pragma config LVP = OFF
// #pragma config STVREN = ON
// #pragma config DEBUG = OFF

#define null 0x00 // rest
#define b3   0xFC  // PR2 = 252  (~246.94 Hz)
#define c4   0xEE  // PR2 = 238  (~261.63 Hz)
#define d4   0xD4  // PR2 = 212  (~293.66 Hz)
#define e4   0xBD  // PR2 = 189  (~329.63 Hz)
#define f4   0xB2  // PR2 = 178  (~349.23 Hz)
#define g4   0x9E  // PR2 = 158  (~392.00 Hz)  
#define a4   0x8D
#define b4   0x7D
#define c5   0x76  // hi
#define d5   0x69  // PR2 = 99   (~587.33 Hz)
#define e5   0x5e  // PR2 = 88   (~659.26 Hz)
#define f5   0x59  // PR2 = 90   (~698.46 Hz)
#define g5   0x4F  // PR2 = 79   (~783.99 Hz)
#define a5   0x46  // PR2 = 70   (~880.00 Hz)
#define b5   0x3E  // PR2 = 62   (~992.06 Hz)
#define c6   0x3B  // PR2 = 59   (~1041.67 Hz)

#define TONE_UNIT_MS 100  // base duration unit in milliseconds

unsigned char pr_arr[32] =
{
    e5, b4, c5, d5,   c5, b4, a4, a4,
    c5, e5, d5, c5,   b4, b4, c5, d5,
    e5, c5, a4, a4, null, d5, f5, a5,
    g5, f5, e5, c5,   e5, d5, c5, b4
};

unsigned char dura[32] =
{
    6, 3, 3, 6, 3, 3, 6, 3,
    3, 6, 3, 3, 6, 3, 3, 6,
    6, 6, 6, 6, 6, 6, 3, 6,
    3, 3, 6, 3, 6, 3, 3, 6
};

// void main(void)
// {
//     init_clock_io();
//     init_ccp4_pwm_rb0();

//     while (1)
//     {
//         play_sequence_on_button_press();
//         __delay_ms(30); // light idle delay
//     }
// }

void init_clock_io(void)
{
    // // Port directions
    // TRISA = 0xF0;   // RA0..RA3 outputs, RA4..RA7 inputs
    // TRISB = 0xFF;  
    // TRISC = 0x00;

    // // Disable analog functions on used ports
    // ANSELA = 0x00;
    // ANSELB = 0x00;
    // ANSELC = 0x00;

    // // Configure internal oscillator to 8 MHz (IRCF = 111)
    // OSCCONbits.IRCF = 0b111;
    // OSCCONbits.SCS = 0b00; // primary clock = system clock (INTOSC)
}

/* Configure Timer2 and CCP4 for PWM output on RB0 (default CCP4 pin)
   - PR2 controls PWM frequency (we write PR2 from pr_arr[] for tone)
   - CCPR4L holds upper 8 bits of duty (we set it to PR2/2 for ~50% duty)
*/
void init_ccp4_pwm_rb0(void)
{
    // Ensure CCP4 is mapped to RB0 (this is the default on PIC18F25K22).
    // If you previously changed APFCON, set it back to default. We won't touch APFCON here.
    // Make RB0 an output for PWM
    TRISBbits.TRISB0 = 0; // output: module will drive it when TMR2/PWM enabled

    // Ensure the pin is digital
    ANSELBbits.ANSB0 = 0;

    // Configure CCP4 module for PWM mode
    CCP4CON = 0x0C;  // PWM mode, DC4B bits = 0 (we use CCPR4L for coarse duty)

    // Timer2 configuration:
    // - prescaler 1:1 (T2CKPS = 0)
    // - Timer2 off for now, we'll start it when playing a note
    T2CONbits.T2CKPS = 0;   // prescaler 1
    T2CONbits.TMR2ON = 0;   // timer2 off initially
    T2CONbits.T2OUTPS = 0;  // postscaler 1

    // Clear initial registers
    PR2 = 0x00;
    CCPR4L = 0x00;
    // CCPTMRS1: select timer source mapping for CCP4/5 (0 => Timer2). Clear to be safe:
    CCPTMRS1 = 0x00;
}

/* Play the melody when RB0 button (active-low) is pressed.
   NOTE: This sample assumes you have an external pull-up on RB0 and the button shorts RB0 to GND when pressed.
   If you want internal pull-ups instead, enable OPTION_REGbits.nWPUEN = 0 and set WPUBbits.WPUB0 = 1.
*/
void play_sequence_on_button_press(void)
{
    // Make RB0 input for button read (if you wire button to same pin as speaker,
    // you must NOT do that — speaker and button must be separate pins.)
    // IMPORTANT: This code assumes the *button* is on RB0. If your speaker is on RB0,
    // put the button on a different input pin (e.g., RB4) or use RD1 for PWM.
    // For clarity: speaker must be on CCP4 pin (RB0) and the button must be on another pin.
    // The following reads RB0 as button; if RB0 is PWM pin for speaker, remove this read.
    unsigned char i;
    unsigned char p;
    unsigned char dur;
    TRISBbits.TRISB0 = 1; // set RB0 as input to read button (if you put your button there)

    // active-low pressed
    if (PORTBbits.RB0 == 0)
    {
        // When playing, configure RB0 as output so PWM can drive speaker
        TRISBbits.TRISB0 = 0;

        for (i = 0; i < sizeof(pr_arr); i++)
        {
            p = pr_arr[i];
            dur = dura[i];

            if (p == 0x00)
            {
                // rest: stop TMR2 and wait
                T2CONbits.TMR2ON = 0;
                __delay_ms((uint32_t)dur * TONE_UNIT_MS);
            }
            else
            {
                PR2 = p;                       // select pitch
                CCPR4L = PR2 >> 1;             // ~50% duty
                CCP4CON &= 0xCF;               // clear DC4B lower bits (coarse duty only)
                // Start timer2 -> PWM output active
                T2CONbits.TMR2ON = 1;

                // hold the note for dura * unit
                __delay_ms((uint32_t)dur * TONE_UNIT_MS);

                // short gap between notes
                T2CONbits.TMR2ON = 0;
                __delay_ms(TONE_UNIT_MS / 2);
            }
        }
        // ensure PWM is stopped and pin returned to input (if you want button there)
        T2CONbits.TMR2ON = 0;
        TRISBbits.TRISB0 = 1; // return to input to detect next press
    }
}
