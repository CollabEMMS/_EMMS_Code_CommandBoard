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

#define LED1_READ PORTAbits.RA2
#define LED2_READ PORTAbits.RA3
#define LED3_READ PORTBbits.RB4
#define LED4_READ PORTAbits.RA4


extern unsigned long tba_energyAllocation;
extern unsigned long tba_energyUsedLifetime;
extern unsigned long tba_energyUsedLastDayReset;

void updateLEDs(void)
{

    int percent;

    unsigned long tempPowerUsed;

    tempPowerUsed = (tba_energyUsedLifetime - tba_energyUsedLastDayReset);
    if (tba_energyAllocation > tempPowerUsed)
    {
        percent = (100 * (tba_energyAllocation - tempPowerUsed)) / tba_energyAllocation;
    }
    else
    {
        percent = 0;
    }

    //    percent = (100 * (powerAllocated - powerUsed)) / powerAllocated;

    // Update the 4 LEDs to show power remaining
    if (percent > 75)
    {
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 1;
    }
    else if (percent > 50)
    {
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 0;
    }
    else if (percent > 25)
    {
        LED1 = 1;
        LED2 = 1;
        LED3 = 0;
        LED4 = 0;
    }
    else if (percent > 5)
    {
        LED1 = 1;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
    }
    else if (tba_energyAllocation > tempPowerUsed)
    {
        static unsigned char lastTime = 255;

        if (lastTime != timeSecond)
        {
            if (LED1_READ == 1)
            {
                LED1 = 0;
            }
            else
            {
                LED1 = 1;
            }
            lastTime = timeSecond;
        }

        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
    }
    else
    {
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
    }


    return;
}