/* File:    PowerMain.c
 * Authors: Dan Baker
 *          Nathan Chaney
 *          Greg Talamo
 */


/* Includes *******************************************************************/

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <p24FV32KA302.h>

#include "PowerDefinitions.h"
#include "PowerPinDefinitions.h"
#include "SharedDefinitions.h"

/* Ensure that SharedCommunication.c is excluded from the source build path!
 * SharedCommunication.c must be included LAST in your main source file
 * like this to ensure the compiler builds the correct variant. */
#define COMM_INCLUDED
#include "SharedCommunication.c"

#define LED1_DIR TRISAbits.TRISA2
#define LED2_DIR TRISAbits.TRISA3
#define LED3_DIR TRISBbits.TRISB4
#define LED4_DIR TRISAbits.TRISA4

#define LED1 LATAbits.LATA2
#define LED2 LATAbits.LATA3
#define LED3 LATBbits.LATB4
#define LED4 LATAbits.LATA4

#define LEDS_FOR_DEBUG  // comment this line for normal operation (LEDS show power remaining)
// uncomment for using the LEDS for debugging


/* Main ***********************************************************************/

/* main
 * Initializes and runs through the main code that is repetitively called
 */
int main(void) {

    LED1_DIR = 0;
    LED2_DIR = 0;
    LED3_DIR = 0;
    LED4_DIR = 0;

    int inx;
    for( inx = 0; inx < 5; inx++)
    {
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 1;
        delayMS(100);
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        delayMS(100);
    }
    delayMS(1000);

    init();
    
    LED1_DIR = 0;
    LED2_DIR = 0;
    LED3_DIR = 0;
    LED4_DIR = 0;

    for( inx = 0; inx < 5; inx++)
    {
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 1;
        delayMS(100);
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        delayMS(100);
    }
    delayMS(1000);
    
    
    //int hb = 0;


    static unsigned char lowPriorityCounter = 0;

    for (;;) {
    // Only do these tasks every 100 - 150 ms
        if (!lowPriorityCounter++) {
            readTime();
            updateLEDs();
            dailyReset();
            zeroPower();
            relayControl();

//            if (hb == 0) {
//                hb = 1;
//                LED1 = 1;
//            } else {
//                hb = 0;
//                LED1 = 0;
//            }
        }

        commFunctions();

        readButton();
        storeToEE();
    }

}


/* Functions ******************************************************************/

/* init
 * Calls each individual initialization method
 */
void init(void) {


    initI2C();
    //            LED1 = 1;
    //            LED2 = 1;

    startClock();
    setClock();
    LED3 = 1;
    initPorts();
    //            LED4 = 1;
    initVars();
    readI2CPowerTimes();
    initPWMeasurement();
    initUART();
    initOC_PWM();
    enableInterrupts();
    commandBuilder1("Reboot", "Now", "0");
    commandBuilder1("Reboot", "Now", "0");
}

/* initVars
 * Initializes variables to their starting values (usually 0)
 */
void initVars(void) {
    resetTime = 59;
    currentLoad = 0;
    powerUsedMW = 0;
    powerUsed = 0;

    EEreadAll();

    // set up highAlloc and lowAlloc
    if ((highAlloc == lowAlloc) && (powerAllocated != 0)) {
        highAlloc = powerAllocated;
        lowAlloc = (powerAllocated * 3) / 4;
    }

    setHighLow();
}

/* setClocks()
 * At boot time, check which RTCC has the most recent time and synchronize them.
 * If this is a new build of the code, use the build time to set both RTCCs.
 */
void setClock(void) {
    unsigned char year, month, day, hour, minute, second;
    readI2CTime(&year, &month, &day, &hour, &minute, &second);

    // Minute  6 bits // << 0
    // Hour    5 bits // << 6
    // Day     5 bits // << 11
    // Month   4 bits // << 16
    // Year    7 bits // << 20
    // The entire date and time fits in a 32-bit long for comparison
    unsigned long
    buildDate = ((unsigned long) BUILD_YEAR << 20) +
            ((unsigned long) BUILD_MONTH << 16) +
            ((unsigned long) BUILD_DAY << 11) +
            ((unsigned long) BUILD_HOUR << 6) + BUILD_MINUTE,

            I2CDate = ((unsigned long) year << 20) +
            ((unsigned long) month << 16) +
            ((unsigned long) day << 11) +
            ((unsigned long) hour << 6) + minute;

    // Find most recent date/time
    if (buildDate > I2CDate) {
        // Correct for the 17 seconds it takes to program the PIC
        second = BUILD_SECOND + 17;
        minute = BUILD_MINUTE;
        hour = BUILD_HOUR;
        if (second > 59) {
            minute++;
            if (minute > 59)
                hour++;
        }

        setI2CTime(BUILD_YEAR, BUILD_MONTH, BUILD_DAY, hour, minute % 60, second % 60);
    }
}

void dailyReset(void) {
    if (((timeHour == resetHour) && (timeMinute == resetMinute) && (timeSecond == 0) && resetFlag)) {
        resetFlag = 0;
        reset = 0;
        totalUsed += powerUsed + extraPower;
        previousDayUsed = powerUsed + extraPower;
        powerUsedMW = 0;
        powerUsed = 0;
        extraPower = 0;
        setRemoteStats();
        EEwriteTotals();
    } else if (!resetFlag && timeSecond) {
        resetFlag = 0xFF;
    }
}

/* initPorts
 * initializes ports for I/O
 * disables Int0 interrupt
 */
void initPorts(void) {

    // Pin 1:  RA5
    // Pin 2:  RA0
    // Pin 3:  RA1
    // Pin 4:  RB0
    // Pin 5:  RB1
    // Pin 6:  RB2
    // Pin 7:  RA2
    // Pin 8:  RA3
    // Pin 9:  RB4
    // Pin 10: RA4
    // Pin 11: RB7
    // Pin 12: RB8
    // Pin 13: RB9
    // Pin 14: Vcap
    // Pin 15: RB12
    // Pin 16: RB13
    // Pin 17: RB14
    // Pin 18: RB15
    // Pin 19: Vss
    // Pin 20: Vdd

    //Everything starts as an output, make input if needed
    //OSCCON = 0b0000000010100101;
    ANSA = 0x0000;
    ANSB = 0x0000;
    //PORTA = 0b1111111111111111;
    PORTA = 0x0000;
    PORTB = 0x0000;

    // 1 for input, 0 for output
    // initialize all to input (for safety - don't launch the missile)
    TRISA = 0xFFFF;
    TRISB = 0xFFFF;

    /* TRIS setup for 20 pin PIC
    _TRISB1 = 1;  // UART
    _TRISB7 = 1;  // power sense
    _TRISB12 = 1; // power sense
    _TRISA1 = 0;  // RS485 send/recieve toggle

    _TRISA3 = 1;  // no connection
    _TRISA0 = 1;  // emergency button
    _TRISB8 = 0;  // relay
    _TRISB14 = 0; // relay
    _TRISA4 = 1;  // oscillator
    _TRISB4 = 1;  // oscillator
     */

    _TRISA0 = 1; // emergency button
    _TRISB0 = 0; // U2TX
    _TRISB1 = 1; // U2RX
    _TRISB2 = 1; // U1RX
    _TRISA3 = 0; // buzzer
    _TRISB7 = 0; // U1TX
    _TRISB8 = 1; // I2C clock
    _TRISB9 = 1; // I2C data
    _TRISA7 = 1; // IC1 from MCP3909
    _TRISB11 = 0; // bar graph
    _TRISB6 = 0; // relay control
    _TRISB15 = 0; // MCP3909 !MCLR
    // 9 pins still available for GPIO


    // disable int0 interrupt, just in case it initializes enabled
    _INT0IE = 0;
}

void enablePullDownResistors(void) {

    _CN2PDE = 1; // pin 2 for emergency button

}

void enableInterrupts(void) {
    INTCON1 |= 0b1000000000000000;
}

void disableInterrupts(void) {
    INTCON1 &= 0b0111111111111111;
}

void readButton(void) {
    static unsigned char lastSecond = 0;

    if (BTN_EMER && emerButtonEnable && (lastSecond != timeSecond)) {

        if ((powerUsedMW / 1000) > emerButtonAlloc) {
            powerUsedMW = powerUsedMW - (((long) emerButtonAlloc) * 1000);


        } else {
            powerUsedMW = 0;
            powerUsed = 0;
        }
        lastSecond = timeSecond;
    }

}

void storeToEE(void) {
    if ((timeSecond == 30) && EEflag) {
        //        if ((timeHour == 0) && (timeMinute == 0)) {
        //            EEwriteDate();
        //            EEflag = 0;
        //        }
        //
        if ((timeMinute % 15) == 1 || (powerUsed >= powerAllocated)) {
            EEwritePowerUsed();
            EEflag = 0;
        }
    } else {
        EEflag = 1;
    }
}

void setHighLow() {
    if (isHigh) {
        powerAllocated = highAlloc;
    } else {
        powerAllocated = lowAlloc;
    }
}
