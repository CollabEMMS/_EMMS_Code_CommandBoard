/* File:    PowerMain.c
 * Authors: Dan Baker
 *          Nathan Chaney
 *          Greg Talamo
 */


/* Includes *******************************************************************/

#include <xc.h>
#include <p24FV32KA302.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Communications.h"
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

#define LED1SET LATAbits.LATA2
#define LED2SET LATAbits.LATA3
#define LED3SET LATBbits.LATB4
#define LED4SET LATAbits.LATA4

#define LED1READ PORTAbits.RA2
#define LED2READ PORTAbits.RA3
#define LED3READ PORTBbits.RB4
#define LED4READ PORTAbits.RA4


//#define LEDS_FOR_DEBUG  // comment this line for normal operation (LEDS show power remaining)
// uncomment for using the LEDS for debugging


/* Main ***********************************************************************/

/* main
 * Initializes and runs through the main code that is repetitively called
 */

//unsigned long powerWatts = 0;
//unsigned long powerVolts = 0;
//unsigned long powerAmps = 0;

unsigned long tba_energyAllocation;
unsigned long tba_energyUsedLifetime;
unsigned long tba_energyUsedLastDayReset;
unsigned long tba_powerWatts;
unsigned long tba_energyUsedPreviousDay = 0;

void initRTCCDisplay(void);

int main(void)
{
    LED1_DIR = 0;
    LED2_DIR = 0;
    LED3_DIR = 0;
    LED4_DIR = 0;

    LED1SET = 0;
    LED2SET = 0;
    LED3SET = 0;
    LED4SET = 0;
        delayMS(100);
    LED1SET = 1;
    LED2SET = 0;
    LED3SET = 0;
    LED4SET = 0;
        delayMS(100);
    LED1SET = 0;
    LED2SET = 1;
    LED3SET = 0;
    LED4SET = 0;
        delayMS(100);
    LED1SET = 0;
    LED2SET = 0;
    LED3SET = 1;
    LED4SET = 0;
        delayMS(100);
    LED1SET = 0;
    LED2SET = 0;
    LED3SET = 0;
    LED4SET = 1;
        delayMS(100);
    LED1SET = 0;
    LED2SET = 0;
    LED3SET = 1;
    LED4SET = 0;
        delayMS(100);
    LED1SET = 0;
    LED2SET = 1;
    LED3SET = 0;
    LED4SET = 0;
        delayMS(100);
    LED1SET = 1;
    LED2SET = 0;
    LED3SET = 0;
    LED4SET = 0;
        delayMS(100);
    LED1SET = 0;
    LED2SET = 0;
    LED3SET = 0;
    LED4SET = 0;
        delayMS(100);

    
    

    for (int inx = 0; inx < 5; inx++)
    {
        LED1SET = 1;
        LED2SET = 1;
        LED3SET = 1;
        LED4SET = 1;
        delayMS(100);
        LED1SET = 0;
        LED2SET = 0;
        LED3SET = 0;
        LED4SET = 0;
        delayMS(100);
    }

    init();
    initRTCCDisplay();

    SPIMasterInit();


    LED1_DIR = 0;
    LED2_DIR = 0;
    LED3_DIR = 0;
    LED4_DIR = 0;

    for (int inx = 0; inx < 5; inx++)
    {
        LED1SET = 1;
        LED2SET = 1;
        LED3SET = 1;
        LED4SET = 1;
        delayMS(100);
        LED1SET = 0;
        LED2SET = 0;
        LED3SET = 0;
        LED4SET = 0;
        delayMS(100);
    }

    //    dailyResetPowerOnCheck();

    unsigned int timerCounterCommunications = 0;
    unsigned int timerCounterComFunctions = 0;
    unsigned int timerCounterLowPriority = 0;
    unsigned int timerCounterReadI2CRTCC = 0;
    unsigned int timerCounterReadIntRTCC = 0;

    bool runCommunications = false;
    bool runComFunctions = false;
    bool runLowPriority = false;
    bool runReadI2CRTCC = false;
    bool runReadIntRTCC = false;

#define TIMER_MS_COUNT		    2000    // timer count for one ms to pass (2000 - 1ms))
#define TIMER_DELAY_COMMUNICATIONS  4	    // time in ms to run function
#define TIMER_DELAY_LOW_PRIORITY    1000	    // time in ms to run function
#define TIMER_DELAY_READ_I2C_RTCC   60000   // time in ms to run function
#define TIMER_DELAY_READ_INT_RTCC   1000	    // time in ms to run function
#define TIMER_DELAY_COM_FUNCTIONS   1	    // time in ms to run function


    readTimeI2C();
    writeTime(timeYear, timeMonth, timeDay, timeHour, timeMinute, timeSecond);

    // check if we lost power over a reset time


    bool enabledSPI;

    while (1)
    {
        if (TMR1 > TIMER_MS_COUNT)
        {
            TMR1 = 0;
            timerCounterCommunications++;
            timerCounterComFunctions++;
            timerCounterLowPriority++;
            timerCounterReadI2CRTCC++;
            timerCounterReadIntRTCC++;
        }

        if (timerCounterCommunications >= TIMER_DELAY_COMMUNICATIONS)
        {
            runCommunications = true;
            timerCounterCommunications = 0;
        }

        if (timerCounterComFunctions >= TIMER_DELAY_COM_FUNCTIONS)
        {
            runComFunctions = true;
            timerCounterComFunctions = 0;
        }

        if (timerCounterLowPriority >= TIMER_DELAY_LOW_PRIORITY)
        {
            runLowPriority = true;
            timerCounterLowPriority = 0;
        }

        if (timerCounterReadI2CRTCC >= TIMER_DELAY_READ_I2C_RTCC)
        {
            runReadI2CRTCC = true;
            timerCounterReadI2CRTCC = 0;
        }

        if (timerCounterReadIntRTCC >= TIMER_DELAY_READ_INT_RTCC)
        {
            runReadIntRTCC = true;
            timerCounterReadIntRTCC = 0;
        }


        if (runCommunications == true)
        {
            timerCounterCommunications = 0;
            enabledSPI = communications();
            runCommunications = false;
            //enabledSPI = false;
        }

        if (enabledSPI == false)
        {
            readButton();
            storeToEE();

            if (runComFunctions == true)
            {
                timerCounterComFunctions = 0;
                commFunctions();
                runComFunctions = false;
            }

            if (runLowPriority == true)
            {
                timerCounterLowPriority = 0;

                updateLEDs();
                dailyResetCheck();
                relayControl();

                runLowPriority = false;
            }

            if (runReadIntRTCC == true)
            {
                timerCounterReadIntRTCC = 0;
                readTime();
                runReadIntRTCC = false;
            }

            if (runReadI2CRTCC == true)
            {
                timerCounterReadI2CRTCC = 0;
                readTimeI2C();
                writeTime(timeYear, timeMonth, timeDay, timeHour, timeMinute, timeSecond);
                runReadI2CRTCC = false;
            }
        }
    }

}


/* Functions ******************************************************************/

/* init
 * Calls each individual initialization method
 */
void init(void)
{


    initI2C();

    startClock();
    setClock();

    initPorts();

    initVars();
    readI2CPowerTimes();
    //    initPWMeasurement( );
    initUART();
    //    initOC_PWM();
    enableInterrupts();
    commandBuilder1("Reboot", "Now", "0");
    commandBuilder1("Reboot", "Now", "0");
}

/* initVars
 * Initializes variables to their starting values (usually 0)
 */
void initVars(void)
{


    EEreadAll();

    // set up highAlloc and lowAlloc
    if ((highAlloc == lowAlloc) && (tba_energyAllocation != 0))
    {
        highAlloc = tba_energyAllocation;
        lowAlloc = (tba_energyAllocation * 3) / 4;
    }

    setHighLow();
}

/* setClocks()
 * At boot time, check which RTCC has the most recent time and synchronize them.
 * If this is a new build of the code, use the build time to set both RTCCs.
 */
void setClock(void)
{
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
    if (buildDate > I2CDate)
    {
        // Correct for the 17 seconds it takes to program the PIC
        second = BUILD_SECOND + 17;
        minute = BUILD_MINUTE;
        hour = BUILD_HOUR;
        if (second > 59)
        {
            minute++;
            if (minute > 59)
                hour++;
        }

        setI2CTime(BUILD_YEAR, BUILD_MONTH, BUILD_DAY, hour, minute % 60, second % 60);
    }
}

//void dailyResetPowerOnCheck(void)
//{
//    // right now the powerDown and powerUp is not working correctly
//    // don't try to reset during a power outage for now
//    
//    
//    
//    
//    // on startup check to see if we need to reset the power allocation
//
//    unsigned char resetMonth;
//    unsigned char resetDay;
//
//
//    // determine which day the reset was to occurr in relation to the day the power went out
//    if (powerDownHour > resetHour)
//    {
//        resetDay = powerDownDay + 1;
//    }
//    else if (powerDownHour == resetHour)
//    {
//        if (powerDownMinute > resetMinute)
//        {
//            resetDay = powerDownDay + 1;
//        }
//    }
//    else
//    {
//        resetDay = powerDownDay;
//    }
//    resetMonth = powerDownMonth;
//
//    // account for days in the month
//    switch (powerDownMonth)
//    {
//        case 1:
//        case 3:
//        case 5:
//        case 7:
//        case 8:
//        case 10:
//        case 12:
//            if (resetDay > 31)
//            {
//                resetDay = 1;
//                resetMonth++;
//            }
//            break;
//
//        case 2:
//            if ((timeYear % 4) == 0)
//            {
//                if (resetDay >= 29)
//                {
//                    resetDay = 1;
//                    resetMonth++;
//                }
//            }
//            else
//            {
//                if (resetDay >= 28)
//                {
//                    resetDay = 1;
//                    resetMonth++;
//                }
//            }
//            break;
//        case 4:
//        case 6:
//        case 9:
//        case 11:
//            if (resetDay > 30)
//            {
//                resetDay = 1;
//                resetMonth++;
//            }
//            break;
//    }
//
//    // no more than 12 months
//    // recognized glitch here if power is out and reset over the new year
//    if (resetMonth > 12)
//    {
//        resetMonth = 1;
//    }
//
//    // turn the information into numbers that we can easily compare
//    unsigned long powerUpTime;
//    unsigned long resetTime;
//    unsigned long powerDownTime;
//
//    powerUpTime = (powerUpMonth * 1000000) + (powerUpDay * 10000) + (powerUpHour * 100) + (powerUpMinute);
//    resetTime = (resetMonth * 1000000) + (resetDay * 10000) + (resetHour * 100) + (resetMinute);
//    powerDownTime = (powerDownMonth * 1000000) + (powerDownDay * 10000) + (powerDownHour * 100) + (powerDownMinute);
//
//
//    if (powerUpTime > resetTime)
//    {
//        dailyReset();
//    }
//
//
//    switch (powerDownMonth)
//    {
//        case 0:
//            LED1SET = 0;
//            LED2SET = 0;
//            LED3SET = 0;
//            LED4SET = 0;
//        case 1:
//            LED1SET = 1;
//            LED2SET = 0;
//            LED3SET = 0;
//            LED4SET = 0;
//        case 2:
//            LED1SET = 0;
//            LED2SET = 1;
//            LED3SET = 0;
//            LED4SET = 0;
//        case 3:
//            LED1SET = 1;
//            LED2SET = 1;
//            LED3SET = 0;
//            LED4SET = 0;
//        case 4:
//            LED1SET = 0;
//            LED2SET = 0;
//            LED3SET = 1;
//            LED4SET = 0;
//        case 5:
//            LED1SET = 1;
//            LED2SET = 0;
//            LED3SET = 1;
//            LED4SET = 0;
//        case 6:
//            LED1SET = 0;
//            LED2SET = 1;
//            LED3SET = 1;
//            LED4SET = 0;
//        case 7:
//            LED1SET = 1;
//            LED2SET = 1;
//            LED3SET = 1;
//            LED4SET = 0;
//        case 8:
//            LED1SET = 0;
//            LED2SET = 0;
//            LED3SET = 0;
//            LED4SET = 1;
//        case 9:
//            LED1SET = 1;
//            LED2SET = 0;
//            LED3SET = 0;
//            LED4SET = 1;
//        case 10:
//            LED1SET = 0;
//            LED2SET = 1;
//            LED3SET = 0;
//            LED4SET = 1;
//        case 11:
//            LED1SET = 1;
//            LED2SET = 1;
//            LED3SET = 0;
//            LED4SET = 1;
//        case 12:
//            LED1SET = 0;
//            LED2SET = 0;
//            LED3SET = 1;
//            LED4SET = 1;
//        case 13:
//            LED1SET = 1;
//            LED2SET = 0;
//            LED3SET = 1;
//            LED4SET = 1;
//        case 14:
//            LED1SET = 0;
//            LED2SET = 1;
//            LED3SET = 1;
//            LED4SET = 1;
//        default:
//            LED1SET = 1;
//            LED2SET = 1;
//            LED3SET = 1;
//            LED4SET = 1;
//    }
//
//    return;
//}

void dailyResetCheck(void)
{
    static bool resetComplete = false;

    if ((timeMinute == resetMinute) && (timeHour == resetHour))
    {
        if (resetComplete == false)
        {
            dailyReset();
            resetComplete = true;
        }
    }
    else
    {

        resetComplete = false;
    }

}

void dailyReset(void)
{

    //    static bool resetComplete = false;

    //    if ((timeMinute == resetMinute) && (timeHour == resetHour))
    //    {
    //        if (resetComplete == false)
    //        {
    tba_energyUsedPreviousDay = tba_energyUsedLifetime - tba_energyUsedLastDayReset;
    tba_energyUsedLastDayReset = tba_energyUsedLifetime;

    EEwriteTotals();
    EEwritePowerUsed();
    setRemoteStats();

    // reset allocation to what is stored
    EEreadPowerAlloc();


    //            resetComplete = true;
    //
    //        }
    //    }
    //    else
    //    {
    //        resetComplete = false;
    //    }

    return;

    //    if( ( ( timeHour == resetHour ) && ( timeMinute == resetMinute ) && ( timeSecond == 0 ) && resetFlag ) )
    //    {
    //	resetFlag = 0;
    //	reset = 0;
    //	totalUsed += powerUsed + extraPower;
    //	previousDayUsed = powerUsed + extraPower;
    //	powerUsedMW = 0;
    //	powerUsed = 0;
    //	extraPower = 0;
    //	setRemoteStats( );
    //	EEwriteTotals( );
    //    }
    //    else if( !resetFlag && timeSecond )
    //    {
    //	resetFlag = 0xFF;
    //    }
}

/* initPorts
 * initializes ports for I/O
 * disables Int0 interrupt
 */
void initPorts(void)
{

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

void enablePullDownResistors(void)
{

    _CN2PDE = 1; // pin 2 for emergency button

}

void enableInterrupts(void)
{

    INTCON1 |= 0b1000000000000000;
}

void disableInterrupts(void)
{

    INTCON1 &= 0b0111111111111111;
}

void readButton(void)
{
    static unsigned char buttonEmergencyComplete = 0;
    static unsigned char buttonTimeSecond = 255;

    if (emerButtonEnable)
    {
        if (BTN_EMER)
        {
            if (buttonEmergencyComplete == 0)
            {
                if (buttonTimeSecond != timeSecond)
                {
                    tba_energyAllocation += emerButtonAlloc;
                    buttonEmergencyComplete = 1;
                    buttonTimeSecond = timeSecond;
                }
            }
        }
        else
        {
            if (buttonTimeSecond != timeSecond)
            {

                buttonEmergencyComplete = 0;
            }
        }
    }

    return;

    //    static unsigned char lastSecond = 0;
    //    static unsigned char lastButton = 0;
    //
    //    if( emerButtonEnable )
    //    {
    //	if( BTN_EMER )
    //	{
    //	    if( lastButton == 0 )
    //	    {
    //		if( lastSecond != timeSecond )
    //		{
    //		    lastSecond = timeSecond;
    //
    //		    tba_energyAllocation += emerButtonAlloc;
    //		}
    //		lastButton = 1;
    //	    }
    //
    //	}
    //	else
    //	{
    //	    lastButton = 0;
    //	}
    //    }
    //
    //    return;

    //
    //    if( BTN_EMER && emerButtonEnable && ( lastSecond != timeSecond ) )
    //    {
    //
    //	if( ( powerUsedMW / 1000 ) > emerButtonAlloc )
    //	{
    //	    powerUsedMW = powerUsedMW - ( ( ( long ) emerButtonAlloc ) * 1000 );
    //
    //
    //	}
    //	else
    //	{
    //	    powerUsedMW = 0;
    //	    powerUsed = 0;
    //	}
    //	lastSecond = timeSecond;
    //    }

}

void storeToEE(void)
{
    static unsigned char eeComplete = 0;

          if( ( timeMinute == 0 ) || ( timeMinute == 15 ) || ( timeMinute == 30 ) || ( timeMinute == 45 ) )
//    if ((timeMinute % 2) == 0)
    {
        if (eeComplete == 0)
        {
            EEwritePowerUsed();
            eeComplete = 1;
        }
    }
    else
    {

        eeComplete = 0;
    }

    //    if( ( timeSecond == 30 ) && EEflag )
    //    {
    //	//        if ((timeHour == 0) && (timeMinute == 0)) {
    //	//            EEwriteDate();
    //	//            EEflag = 0;
    //	//        }
    //	//
    //	if( ( timeMinute % 15 ) == 1 || ( powerUsed >= powerAllocated ) )
    //	{
    //	    EEwritePowerUsed( );
    //	    EEflag = 0;
    //	}
    //    }
    //    else
    //    {
    //	EEflag = 1;
    //    }
}

void setHighLow()
{
    if (isHigh)
    {
        //	powerAllocated = highAlloc;
        tba_energyAllocation = highAlloc;
    }
    else
    {
        //	powerAllocated = lowAlloc;

        tba_energyAllocation = lowAlloc;
    }
}

void initRTCCDisplay(void)
{

    //does unlock sequence to enable write to RTCC, sets RTCWEN
    __builtin_write_RTCWEN();

    RCFGCAL = 0b0010001100000000;
    RTCPWC = 0b0000010100000000; // LPRC is clock source

    _RTCEN = 1;

    _RTCWREN = 0; // Disable Writing
}
