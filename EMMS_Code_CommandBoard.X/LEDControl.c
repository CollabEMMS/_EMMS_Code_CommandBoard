/***************
NOTE
 
 DO NOT use regular LED functions for debugging purposes
 Use the "Test" functions for debug purposes
 The "Test" functions can be enabled and disabled using the LEDS_FOR_DEBUG macro found in common.h
 
 Using regular LED functions for debugging purposes will break the Bar Graph and can make future debugging difficult
 
 *****************/


/****************
 INCLUDES
 only include the header files that are required
 ****************/

#include "common.h"

#include "PowerMain.h"
#include "RTCC.h"
#include "MasterComm.h"


/****************
 MACROS
 ****************/

//#define LEDS_FOR_DEBUG false   // if true will not allow energy remain to update LEDS so leds can be used for testing and debugging

#define LED1_DIR TRISAbits.TRISA2
#define LED2_DIR TRISAbits.TRISA3
#define LED3_DIR TRISBbits.TRISB4
#define LED4_DIR TRISAbits.TRISA4

#define LED1_SET LATAbits.LATA2
#define LED2_SET LATAbits.LATA3
#define LED3_SET LATBbits.LATB4
#define LED4_SET LATAbits.LATA4

#define LED1_READ PORTAbits.RA2
#define LED2_READ PORTAbits.RA3
#define LED3_READ PORTBbits.RB4
#define LED4_READ PORTAbits.RA4



/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/
// external

bool ledFindMeActive_global = false;


// internal only


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 ****************/

// internal only
void ledSet( int ledNum, int setValue );
void ledSetOn( int ledNum );
void ledSetOff( int ledNum );
void ledToggle( int ledNum );


// internal and external
void ledSetAll( int led1Value, int led2Value, int led3Value, int led4Value );
void ledSetAllOn( void );
void ledSetAllOff( void );

/****************
 CODE
 ****************/

// Sets all of the LED I/O pins to be outputs from the pic
void ledInit( void )
{
	LED1_DIR = 0;
	LED2_DIR = 0;
	LED3_DIR = 0;
	LED4_DIR = 0;

	return;
}

// Set an individual LED to be on or off
void ledSet( int ledNum, int setValue )
{
	switch( ledNum )
	{
		case 1:
			LED1_SET = setValue;
			break;
		case 2:
			LED2_SET = setValue;
			break;
		case 3:
			LED3_SET = setValue;
			break;
		case 4:
			LED4_SET = setValue;
			break;
	}

	return;
}

// Set a specific LED to be on
void ledSetOn( int ledNum )
{
	int setValue = 1;

	ledSet( ledNum, setValue );

	return;
}

// Set a specific LED to be off
void ledSetOff( int ledNum )
{
	int setValue = 0;

	ledSet( ledNum, setValue );

	return;
}

// The same as ledSetOn, but only does something when the meter is in a debug mode
void ledTestSetOn( int ledNum )
{
	if( LEDS_FOR_DEBUG == true )
	{
		ledSetOn( ledNum );
	}

	return;
}

// The same as ledSetOff, but only does something when the meter is in a debug mode
void ledTestSetOff( int ledNum )
{
	if( LEDS_FOR_DEBUG == true )
	{
		ledSetOff( ledNum );
	}

	return;
}

// Set all of the LEDs at once
void ledSetAll( int led1Value, int led2Value, int led3Value, int led4Value )
{
	ledSet( 1, led1Value );
	ledSet( 2, led2Value );
	ledSet( 3, led3Value );
	ledSet( 4, led4Value );

	return;
}

// Same as ledSetAll, but only does this while the meter is in it's debug mode
void ledTestSetAll( int led1Value, int led2Value, int led3Value, int led4Value )
{
	if( LEDS_FOR_DEBUG == true )
	{
		ledSetAll( led1Value, led2Value, led3Value, led4Value );
	}

	return;
}

// Turn all the LEDs on
void ledSetAllOn( void )
{
	ledSetAll( 1, 1, 1, 1 );

	return;
}

// Turn all of the LEDs off
void ledSetAllOff( void )
{
	ledSetAll( 0, 0, 0, 0 );

	return;
}

// Turn all lights on, but only in debug mode
void ledTestSetAllOn( void )
{
	if( LEDS_FOR_DEBUG == true )
	{
		ledSetAllOn( );
	}

	return;
}

// Turn all lights off, but only in debug mode
void ledTestSetAllOff( void )
{
	if( LEDS_FOR_DEBUG == true )
	{
		ledSetAllOff( );

	}

	return;
}

// Toggle a particular LED (on->off or off->on)
void ledToggle( int ledNum )
{

	int getValue;
	int setValue;

	switch( ledNum )
	{
		case 1:
			getValue = LED1_READ;
			break;
		case 2:
			getValue = LED2_READ;
			break;
		case 3:
			getValue = LED3_READ;
			break;
		case 4:
			getValue = LED4_READ;
			break;

	}

	if( getValue == 0 )
	{
		setValue = 1;
	}
	else
	{
		setValue = 0;
	}

	ledSet( ledNum, setValue );

	return;
}

// Toggles an LED, but only if the test mode is on
void ledTestToggle( int ledNum )
{
	if( LEDS_FOR_DEBUG == true )
	{
		ledToggle( ledNum );
	}

	return;
}

// Displays a specific sequence of lights
void ledRunUp( int ledRunDelay )
{

	ledSetAll( 0, 0, 0, 0 );
	__delay_ms( ledRunDelay );
	ledSetAll( 1, 0, 0, 0 );
	__delay_ms( ledRunDelay );
	ledSetAll( 0, 1, 0, 0 );
	__delay_ms( ledRunDelay );
	ledSetAll( 0, 0, 1, 0 );
	__delay_ms( ledRunDelay );
	ledSetAll( 0, 0, 0, 1 );
	__delay_ms( ledRunDelay );
}

// Displays a specific sequence of lights
void ledRunDown( int ledRunDelay )
{

	ledSetAll( 0, 0, 0, 1 );
	__delay_ms( ledRunDelay );
	ledSetAll( 0, 0, 1, 0 );
	__delay_ms( ledRunDelay );
	ledSetAll( 0, 1, 0, 0 );
	__delay_ms( ledRunDelay );
	ledSetAll( 1, 0, 0, 0 );
	__delay_ms( ledRunDelay );
	ledSetAll( 0, 0, 0, 0 );
	__delay_ms( ledRunDelay );

}

// Displays a specific sequence of lights
void ledRun( int ledRunDelay )
{
	ledRunUp( ledRunDelay );
	ledRunDown( ledRunDelay );
}

// Displays a basic bar graph of the remaining energy on the LEDs
void ledSetFrontEnergyRemain( void )
{
	int percent;

	unsigned long tempPowerUsed;

	if( (LEDS_FOR_DEBUG != true) && (ledFindMeActive_global == false) )
	{
		tempPowerUsed = (energyUsed_global.lifetime - energyUsed_global.lastReset);
		if( energyCycleAllocation_global > tempPowerUsed )
		{
			percent = (100 * (energyCycleAllocation_global - tempPowerUsed)) / energyCycleAllocation_global;
		}
		else
		{
			percent = 0;
		}

		// Update the 4 LEDs to show power remaining
		if( percent > 75 )
		{
			ledSetAll( 1, 1, 1, 1 );
		}
		else if( percent > 50 )
		{
			ledSetAll( 1, 1, 1, 0 );

		}
		else if( percent > 25 )
		{
			ledSetAll( 1, 1, 0, 0 );
		}
		else if( percent > 5 )
		{
			ledSetAll( 1, 0, 0, 0 );
		}
		else if( energyCycleAllocation_global > tempPowerUsed ) // this is less than the last percentage check (5% right now))
		{
			// for the last little bit toggle the last led (RED) each second
			static unsigned char lastTime = 255;

			if( lastTime != dateTime_global.second )
			{
				ledToggle( 1 );
				lastTime = dateTime_global.second;
			}

			ledSetOff( 2 );
			ledSetOff( 3 );
			ledSetOff( 4 );

		}
		else
		{
			ledSetAll( 0, 0, 0, 0 );
		}

	}

	return;
}

// Made for finding a specific meter when multiple are set up. Mostly for wifi use
void ledSetFrontFindMe( void )
{
	static int status = 0;

	if( (LEDS_FOR_DEBUG != true) && (ledFindMeActive_global == true) )
	{
		switch( status )
		{
			case 0:
				ledSetAll( 1, 0, 0, 0 );
				status++;
				break;
			case 1:
				ledSetAll( 0, 1, 0, 0 );
				status++;
				break;
			case 2:
				ledSetAll( 0, 0, 1, 0 );
				status++;
				break;
			case 3:
				ledSetAll( 0, 0, 0, 1 );
				status++;
				break;
			case 4:
				ledSetAll( 0, 0, 1, 0 );
				status++;
				break;
			case 5:
				ledSetAll( 0, 1, 0, 0 );
				status = 0;
				break;
			default:
				status = 0;
				break;
		}
	}

	return;
}

// Display an 8-bit character as two 4-bit values (flashes high bits, then low bits on LEDs)
void ledTestShowChar( char showMe )
{
	if( LEDS_FOR_DEBUG == true )
	{
		bool b[8];

		for( int j = 0; j < 8; ++j )
		{
			b [j] = 0 != (showMe & (1 << j));
		}

		for( int inx = 0; inx < 3; inx++ )
		{
			ledSetAllOn( );
			__delay_ms( 250 );
			ledSetAllOff( );
			__delay_ms( 250 );
		}


		ledSetAll( b[7], b[6], b[5], b[4] );
		__delay_ms( 2500 );
		ledSetAll( b[3], b[2], b[1], b[0] );
		__delay_ms( 2500 );


		ledSetAllOn( );
		__delay_ms( 250 );
		ledSetAllOff( );
	}

	return;

}

// Similar to ledShowChar, but shows the first 8-bits of an integer. Should be used with the lower one too
void ledTestShowIntH( int showMe )
{
	if( LEDS_FOR_DEBUG == true )
	{
		unsigned char upper;

		upper = showMe & 0xFF;
		ledTestShowChar( showMe );
	}

	return;

}

// Similar to ledShowChar, but shows the last 8-bits of an integer. Should be used with the upper one too
void ledTestShowIntL( int showMe )
{
	if( LEDS_FOR_DEBUG == true )
	{
		unsigned char lower;

		lower = showMe >> 8;
		ledTestShowChar( showMe );
	}

	return;
}