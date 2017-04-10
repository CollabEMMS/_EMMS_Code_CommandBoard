#include <xc.h>
#include <p24FV32KA302.h>
#include "ExternSharedDefinitions.h"
#include "ExternPowerDefinitions.h"

#define LED1_DIR TRISAbits.TRISA2
#define LED2_DIR TRISAbits.TRISA3
#define LED3_DIR TRISBbits.TRISB4
#define LED4_DIR TRISAbits.TRISA4

#define LED1 LATAbits.LATA2
#define LED2 LATAbits.LATA3
#define LED3 LATBbits.LATB4
#define LED4 LATAbits.LATA4

void initOC_PWM(void) {
//    OC2CON1 = 0; // Clear OC2 control registers
//    OC2CON2 = 0; // Clear OC2 control registers
//
//    // Duty cycle = On-time / Period
//    OC2R  = 0;   // On-time
//    OC2RS = 110; // Period
//
//    OC2CON2bits.SYNCSEL = 0b11111; // Self-synchronize
//    OC2CON1bits.OCTSEL  = 0b111;   // Clock source: system clock
//    OC2CON1bits.OCM     = 0b110;   // Edge-aligned mode


//    LED1_DIR = 0;
//    LED2_DIR = 0;
//    LED3_DIR = 0;
//    LED4_DIR = 0;

}
// Set OC2R to any percent value 0 to 100
void updateLEDs(void) {
//    OC2R = (timeSecond % 11) * 10;
    //OC2R = (100 * (powerAllocated - powerUsed)) / powerAllocated;

    int percent;
    percent = (100 * (powerAllocated - powerUsed)) / powerAllocated;

#ifndef LEDS_FOR_DEBUG
//    return;
    
    // Update the 4 LEDs to show power remaining
    if (percent > 75) {
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 1;
    } else if (percent > 50) {
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 0;
    } else if (percent > 25) {
        LED1 = 1;
        LED2 = 1;
        LED3 = 0;
        LED4 = 0;
    } else if (percent > 0) {
        LED1 = 1;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
    } else {
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
    }

#endif

    return;
}