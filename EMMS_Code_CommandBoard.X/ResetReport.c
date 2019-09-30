/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include "common.h"
#include "LEDControl.h"


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
// static keyword make the variable visible only to this file
static int resetCodes_module;

/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
     should be marked
 *****************/

/****************
 CODE
 ****************/


// capture the code is separate from displaying the code.
// we capture first thing - before anything else is done
// then we set up the ports so we can use the output
// then we display the code

void resetCodeCapture( void )
{

    resetCodes_module = RCON;
    RCON = 0; // reset all codes for the next read

    return;
}

void resetReportDisplay( void )
{

    // set up the code into bits that are easy to use for individual codes
    bool b[16];

    // mask out the (power on reset) and the (brown out reset) and the (MCLR reset)
    // these are the resets that are shown for normal power outages
    int tempCodes;
    tempCodes = resetCodes_module & 0b1111111101111100;

    // only run reset lights if there is a code to show
    // of note - after programming the MCLR code will show (bit 7)
    // remove power and add power to see no codes show
    if( tempCodes > 0 )
    {

	for( int j = 0; j < 16; ++j )
	{
	    b [j] = 0 != (tempCodes & (1 << j));
	}

	for( int inx = 0; inx < 5; inx++ )
	{
	    ledRun( 100 );
	    ledRunUp( 25 );
	    ledSetAll( 0, 0, 0, 0 );
	    __delay_ms( 250 );
	    ledSetAll( b[0], b[1], b[2], b[3] );
	    __delay_ms( 1000 );
	    ledRunUp( 25 );
	    ledSetAll( b[4], b[5], b[6], b[7] );
	    __delay_ms( 1000 );
	    ledRunUp( 25 );
	    ledSetAllOff( );
	    __delay_ms( 250 );
	    ledSetAll( b[8], b[9], b[10], b[11] );
	    __delay_ms( 1000 );
	    ledRunUp( 25 );
	    ledSetAllOff( );
	    __delay_ms( 250 );
	    ledSetAll( b[12], b[13], b[14], b[15] );
	    __delay_ms( 1000 );
	    ledRunUp( 25 );
	    ledSetAllOff( );
	    __delay_ms( 250 );
	}

	for( int inx = 0; inx < 5; inx++ )
	{
	    ledSetAllOn( );
	    __delay_ms( 100 );
	    ledSetAllOff( );
	    __delay_ms( 100 );
	}
    }

}