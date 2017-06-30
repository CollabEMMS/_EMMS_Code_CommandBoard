/* File:    Power.c
 * Authors: Dan Baker
 *          Nathan Chaney
 */

/* Includes *******************************************************************/

#include <p24FV32KA302.h>
#include "ExternPowerDefinitions.h"
#include "ExternSharedDefinitions.h"
#include "PowerPinDefinitions.h"

// Must be less than   4294967296
//#define SCALING_FACTOR 3785065107 // about 1044W. Should be 1050W
//#define SCALING_FACTOR 1766363717 // about 1063W. Should be 1050W
//#define SCALING_FACTOR 1852829074 // about 1063W. Should be 1050W
//#define SCALING_FACTOR 1930534614 // about 1063W. Should be 1050W
//#define INCREMENT_MWH  858

extern unsigned long tba_energyAllocation;
extern unsigned long tba_energyUsedLifetime;
extern unsigned long tba_energyUsedLastDayReset;


/* Functions ******************************************************************/

//void initPWMeasurement(void) {
//    // Two input capture modules set up for 32 bit cascaded operation.
//    // This will allow the timer to run for about 2048 seconds.
//
//    // I found help on this at Microchip forums
//    // http://www.microchip.com/forums/m747137-print.aspx
//
//    while (IC1CON1bits.ICBNE) IC1BUF;
//    while (IC2CON1bits.ICBNE) IC2BUF;
//
//    IC1CON1 = 0x0000; // Initialize IC modules
//    IC2CON1 = 0x0000;
//    IC1CON2 = 0x0000;
//    IC2CON2 = 0x0000;
//
//    IC1CON2bits.IC32 = 0b1; // 32-bit timer mode
//    IC2CON2bits.IC32 = 0b1;
//
//    IC1CON1bits.IC1TSEL = 0b111; // Sync with Fosc / 2
//    IC2CON1bits.IC2TSEL = 0b111;
//
//    IC2CON1bits.ICM = 0b011; // Enable IC2, capture on falling edge
//    IC1CON1bits.ICM = 0b011; // Enable IC1, capture on falling edge
//
//    IPC0bits.IC1IP = 2; // Set interrupt priority
//    IFS0bits.IC1IF = 0; // Clear interrupt flag
//    IEC0bits.IC1IE = 1; // Enable IC1 interrupt
//
//    MCP_RESET = 1; // Turn on the MCP3909 by setting the !RESET pin
//
//}

//void zeroPower(void) {
//
//    DWORD_VAL newPulseWidth;
//
//    // No reason to zero if power is already zero.
//    if (!currentLoad) return;
//
//    // If this changes to a 1 before the end of zeroing,
//    // a pulse occurred during the zeroing routine
//    // and we need to skip zeroing this time.
//    interruptOccurred = 0;
//
//    // Use ICxTMR here to get immediate values without resetting
//    // Using ICxBUF won't work
//    // As if interrupt would fire right now
//    newPulseWidth.word.LW = IC1TMR;
//    newPulseWidth.word.HW = IC2TMR;
//    newPulseWidth.Val -= prevCapTime.Val;
//
//    // If the interrupt fired right now, it would read a pulse 15% longer
//    // than the previous pulse, meaning a 15% decrease in load. Since the
//    // interrupt did not fire yet, the load has decreased more than 15%,
//    // so we can safely deduct 15%. If no power is being consumed, we will
//    // get a zero load eventually.
//    if ((newPulseWidth.Val * 7) / 8 > pulseWidth.Val) {
//        // If an interrupt occurred since we
//        // calculated newPulseWidth, don't zero.
//        if (!interruptOccurred) {
//            currentLoad = (currentLoad * 7) / 8;
//            pulseWidth.Val = (pulseWidth.Val / 7) * 8;
//        }
//    }
//}

void relayControl(void)
{
    static unsigned char lastSecond;

    unsigned long tempEnergyUsed;

    tempEnergyUsed = tba_energyUsedLifetime - tba_energyUsedLastDayReset;

    if (timeSecond != lastSecond)
    {
        if (relayActive)
        {
            if (tempEnergyUsed < tba_energyAllocation)
            {
                RELAY = 1;
            }
            else
            {
                RELAY = 0;
                //		currentLoad = 0; // just set power to zero because relay is off
            }
        }
        lastSecond = timeSecond;
    }

    return;


    //
    //    // turn it off
    //    if ((timeSecondI2C != lastSecond) && (powerAllocated <= powerUsed) && (relayActive)) {
    //        RELAY = 0;
    //        // no reason to wait for zeroing if relay is off
    //        currentLoad = 0;
    //        lastSecond = timeSecondI2C;
    //    }
    //        // turn it on
    //    else if ((timeSecondI2C != lastSecond) && (powerAllocated > powerUsed)) {
    //        RELAY = 1;
    //        lastSecond = timeSecondI2C;
    //    }
}


/* Interrupts *****************************************************************/

/**
 * Input Capture Interrupt
 * Interrupt generated on for every period of logic input
 * signal. Read timer at the end of every period and calculate
 * period time.
 */
//void __attribute__((interrupt, no_auto_psv)) _IC1Interrupt(void) {
//
//    DWORD_VAL newCapTime;
//
//    // Read input capture buffers
//    // Use ICxBUF here, not ICxTMR
//    while (IC1CON1bits.ICBNE) newCapTime.word.LW = IC1BUF;
//    while (IC2CON1bits.ICBNE) newCapTime.word.HW = IC2BUF;
//
//    // Calculate latest pulse width
//    pulseWidth.Val = newCapTime.Val - prevCapTime.Val;
//    prevCapTime.Val = newCapTime.Val;
//
//    // Calculate current load from pulse width
//    currentLoad = SCALING_FACTOR / (pulseWidth.Val / 10);
//
//    // Account energy used by last pulse
//    powerUsedMW += INCREMENT_MWH;
//    powerUsed = powerUsedMW / 1000;
//
//    // Set a flag so we don't zero at the wrong time
//    interruptOccurred = 1;
//
//    IFS0bits.IC1IF = 0; // Clear interrupt flag
//}