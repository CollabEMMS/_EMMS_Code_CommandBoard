/* File:    Delays.c
 * Authors: Nathan Chaney
 *          Tommy Martin
 */

/* Includes *******************************************************************/

#include <p24FV32KA302.h>
#include "PowerPinDefinitions.h"
#include "ExternPowerDefinitions.h"
#include "ExternSharedDefinitions.h"


/* Functions ******************************************************************/

void initPowerLossDetection(void) {
    _TRISB3 = 1;// make pin 7 an input
    _CN7IE = 1;// pin 7 on the 28-pin PIC
    _CNIE = 1;// turn on CN interrupts
}

void goToSleep ( )
{
    // go to sleep

    // make all I/O inputs to keep from burning power
    // these should be set to keep any power from being used by external devices
    // if an output lights an LED, the LED will suck down the super cap
    TRISA = 0b1111111111111111;
    TRISB = 0b1111111111111111;


    // reset INT0 and set to trigger on rising edge
    IFS0bits.INT0IF = 0b0;
    IEC0bits.INT0IE = 0b0;
    INTCON2bits.INT0EP = 0b0;
    IEC0bits.INT0IE = 0b1;


    // enter sleep mode
    DSCONbits.DSEN = 0b1;
    Sleep();
}


/* Interrupts *****************************************************************/

void __attribute__((__interrupt__,__no_auto_psv__)) _CNInterrupt(void) {
    if(_RB3 == 0) {
        commandBuilder1("Lost", "Power", "0");
    } else {
        commandBuilder1("Got", "Power", "0");
    }

    _CNIF = 0;
}