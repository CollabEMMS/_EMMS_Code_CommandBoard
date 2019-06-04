/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include "common.h"
#include "I2C_RTCC.h"
#include "MasterComm.h"

/****************
 MACROS
 ****************/

/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/
// external
// internal only


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
     should be marked
 ****************/

/****************
 CODE
 ****************/

void updateLEDs( void )
{

    int percent;

    unsigned long tempPowerUsed;

    if( LEDS_FOR_TESTING != true )
    {


	tempPowerUsed = (tba_energyUsedLifetime - tba_energyUsedLastDayReset);
	if( tba_energyAllocation > tempPowerUsed )
	{
	    percent = (100 * (tba_energyAllocation - tempPowerUsed)) / tba_energyAllocation;
	}
	else
	{
	    percent = 0;
	}

	//    percent = (100 * (powerAllocated - powerUsed)) / powerAllocated;

	// Update the 4 LEDs to show power remaining
	if( percent > 75 )
	{
	    LED1_SET = 1;
	    LED2_SET = 1;
	    LED3_SET = 1;
	    LED4_SET = 1;
	}
	else if( percent > 50 )
	{
	    LED1_SET = 1;
	    LED2_SET = 1;
	    LED3_SET = 1;
	    LED4_SET = 0;
	}
	else if( percent > 25 )
	{
	    LED1_SET = 1;
	    LED2_SET = 1;
	    LED3_SET = 0;
	    LED4_SET = 0;
	}
	else if( percent > 5 )
	{
	    LED1_SET = 1;
	    LED2_SET = 0;
	    LED3_SET = 0;
	    LED4_SET = 0;
	}
	else if( tba_energyAllocation > tempPowerUsed )
	{
	    static unsigned char lastTime = 255;

	    if( lastTime != timeSecond )
	    {
		if( LED1_READ == 1 )
		{
		    LED1_SET = 0;
		}
		else
		{
		    LED1_SET = 1;
		}
		lastTime = timeSecond;
	    }

	    LED2_SET = 0;
	    LED3_SET = 0;
	    LED4_SET = 0;
	}
	else
	{
	    LED1_SET = 0;
	    LED2_SET = 0;
	    LED3_SET = 0;
	    LED4_SET = 0;
	}

    }
    return;
}