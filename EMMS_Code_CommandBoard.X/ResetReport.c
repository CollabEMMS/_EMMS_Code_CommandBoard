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
	/*
	the codes are listed at the bottom of this file

	codes are numbered from highest numbered to lowest numbered
	in the result storage 0b0000000000000000
		the left most is bit 15 and the right most is bit 0
		the left most code is Trap Reset Flag
		the right most code is Power-on Reset Flag

	the codes are displayed on the 4 LEDs with flash patterns breaking the 16 codes into 4 groups
	 the red LED is the low bit in each group
	 LEDS run up/down sequence to designate the start of the reset code sequence
	 LEDS display the first 4 bits (0, 1, 2, 3)   0 is the red LED
	 LEDS run up
	 LEDS display second 4 bits (4, 5, 6, 7)	4 is the red LED
	 repeat till all bits are displayed in sequence
	 
	 note - most 4 bit segments will be all 0's (off) since typically only one code generates the reset
			
	 */

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
			b [j] = 0 != ( tempCodes & ( 1 << j ) );
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

/*
 RESET CODES FROM DOCUMENTATION  Section 7.0 - Resets - Page 70-71
 
bit 15 TRAPR: Trap Reset Flag bit
	1 = A Trap Conflict Reset has occurred
	0 = A Trap Conflict Reset has not occurred
 
bit 14 IOPUWR: Illegal Opcode or Uninitialized W Access Reset Flag bit
	1 = An illegal opcode detection, an illegal address mode or Uninitialized W register used as an Address Pointer caused a Reset
	0 = An illegal opcode or Uninitialized W Reset has not occurred
 
bit 13 SBOREN: Software Enable/Disable of BOR bit
	1 = BOR is turned on in software
	0 = BOR is turned off in software
 
bit 12 RETEN: Retention Sleep Mode control bit(3)
	1 = Regulated voltage supply provided solely by the Retention Regulator (RETEN) during Sleep
	0 = Regulated voltage supply provided by the main Voltage Regulator (VREG) during Sleep
 
bit 11 Unimplemented: Read as ?0?
 
bit 10 DPSLP: Deep Sleep Mode Flag bit
	1 = Deep Sleep has occurred
	0 = Deep Sleep has not occurred
 
bit 9 CM: Configuration Word Mismatch Reset Flag bit
	1 = A Configuration Word Mismatch Reset has occurred
	0 = A Configuration Word Mismatch Reset has not occurred
 
bit 8 PMSLP: Program Memory Power During Sleep bit
	1 = Program memory bias voltage remains powered during Sleep
	0 = Program memory bias voltage is powered down during Sleep and the Voltage Regulator enters Standby mode
 
bit 7 EXTR: External Reset (MCLR) Pin bit
	1 = A Master Clear (pin) Reset has occurred
	0 = A Master Clear (pin) Reset has not occurred
 
bit 6 SWR: Software Reset (Instruction) Flag bit
	1 = A RESET instruction has been executed
	0 = A RESET instruction has not been executed
 
bit 5 SWDTEN: Software Enable/Disable of WDT bit(2)
	1 = WDT is enabled
	0 = WDT is disabled
 
bit 4 WDTO: Watchdog Timer Time-out Flag bit
	1 = WDT time-out has occurred
	0 = WDT time-out has not occurred
 
bit 3 SLEEP: Wake-up from Sleep Flag bit
	1 = Device has been in Sleep mode
	0 = Device has not been in Sleep mode
 
bit 2 IDLE: Wake-up from Idle Flag bit
	1 = Device has been in Idle mode
	0 = Device has not been in Idle mode
 
bit 1 BOR: Brown-out Reset Flag bit
	1 = A Brown-out Reset has occurred (the BOR is also set after a POR)
	0 = A Brown-out Reset has not occurred
 
bit 0 POR: Power-on Reset Flag bit
	1 = A Power-up Reset has occurred
	0 = A Power-up Reset has not occurred
 
 */