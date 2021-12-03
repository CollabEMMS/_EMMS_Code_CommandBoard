/****************
 INCLUDES
 only include the header files that are required
 ****************/

#include "common.h"

#include "ResetReport.h"
#include "RTCC.h"
#include "MasterComm.h"
#include "LEDControl.h"
#include "EEPROM.h"


/****************
 MACROS
 ****************/

#define PORT_WRITE_RELAY PORTBbits.RB6    // Pin 15: RB6
#define PORT_READ_BUTTON_EMERGENCY PORTAbits.RA0    // Pin 2:  RA0


/*
 The following section sets a macros with the build date and time
 This eventually gets displayed on the UI and the power box version
 */

// below pulls the build date time into something that can be used
// Build date: __DATE__ -> "Mar  2 2015"
// Build time: __TIME__ -> "14:05:00"
#define BUILD_YEAR  (((__DATE__ [9] - 48) * 10) + (__DATE__ [10] - 48))
#define BUILD_MONTH (\
  __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
: __DATE__ [2] == 'b' ? 2 \
: __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
: __DATE__ [2] == 'y' ? 5 \
: __DATE__ [2] == 'l' ? 7 \
: __DATE__ [2] == 'g' ? 8 \
: __DATE__ [2] == 'p' ? 9 \
: __DATE__ [2] == 't' ? 10 \
: __DATE__ [2] == 'v' ? 11 \
: 12)

#define BUILD_DAY (__DATE__ [4] == ' ' ? (__DATE__ [5] - 48) \
: ((__DATE__ [4] - 48) * 10) + (__DATE__ [5] - 48))

#define BUILD_HOUR   (((__TIME__ [0] - 48) * 10) + (__TIME__ [1] - 48))
#define BUILD_MINUTE (((__TIME__ [3] - 48) * 10) + (__TIME__ [4] - 48))
#define BUILD_SECOND (((__TIME__ [6] - 48) * 10) + (__TIME__ [7] - 48))

#define CODE_REVISION 1

// the below line is commented out because we just hardcode a version for right now
//#define POWER_BOX_CODE_VERSION ((char[]) {__DATE__ [9], __DATE__ [10], (BUILD_MONTH / 10) + 48, (BUILD_MONTH % 10) + 48, (__DATE__ [4] == ' ' ? '0' : __DATE__ [4]), __DATE__ [5], (CODE_REVISION / 10) + 48, (CODE_REVISION % 10) + 48, 0})

// TODO - come up with a good versioning plan for the command board
#define POWER_BOX_CODE_VERSION "20190929"   // 7 characters show on the UI

// Time window where the oneshots can occur
// sometimes the program main loop takes more than 1 ms to run and we can inadvertantly skip if we are lookng for an exact match
#define ONESHOT_WINDOW 25






/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/
// external
unsigned long powerWatts_global;

// internal only
volatile unsigned long msTimer_module; // toggles to 1 every ms, resets to 0 at about the 0.5ms time

//external & internal
struct energy_info energyUsed_global; // energy status of the system
struct date_time dateTime_global; // the current date time - periodically pulled from RTCC
struct alarm_info alarms_global; // alarms
int energyAdd_global; // how much energy has been added to this cycle
unsigned long energyCycleAllocation_global; // how much energy is allocated per cycle
struct reset_time resetTime_global; // hour and minute of the cycle reset
bool relayActive_global; // is the relay active
struct emergency_button emergencyButton_global; // emergency button enabled and allocation amount

// TODO set version
// replace this with versioning plan
char powerBoxCodeVersion_global[9] = POWER_BOX_CODE_VERSION;



/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
  any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 ****************/

// internal only
void initOscillator( void );
void init( void );
void initPorts( void );
void initReadGlobalsFromEEPROM( void );
void powerOnCheckForAllocationReset( void );
void dailyResetCheck( void );
void dailyReset( void );
void readEmergencyButton( void );
void storeToEE( void );
void relayControl( void );
void initTimer( void );




// internal and external

/****************
 CODE
 ****************/

int main( void )
{
	// capture reset code first before something overwrites the register
	resetCodeCapture( );

	initOscillator( );

	//disable all analog ports
	// if one is needed it must be individually activated
	// this is done here so that the LEDS will work correctly
	// if the analog ports are not explicitly disabled, they might be active
	//    which masks the port and makes it difficult to figure out what is wrong
	ANSA = 0b0000000000000000;
	ANSB = 0b0000000000000000;

	// set all ports as input by default
	TRISA = 0b1111111111111111; // this is equivalent to setting all of the individual bits
	TRISB = 0b1111111111111111; // 0b1111111111111111 = 0xFFFF

	// set all ports to low by default to start
	PORTA = 0b0000000000000000; // this is equivalent to setting all of the individual bits
	PORTB = 0b0000000000000000; // 0b0000000000000000 = 0

	ledInit( );

	resetReportDisplay( );

	ledSetAllOff( );

	init( );
	ledSetAll( 1, 0, 0, 0 );

	// read in the EEPROM values that are used as global
	initReadGlobalsFromEEPROM( );
	ledSetAll( 1, 0, 0, 1 );

	powerOnCheckForAllocationReset( );
	ledSetAll( 1, 0, 1, 0 );

	commInit( );
	ledSetAll( 1, 0, 1, 1 );


	// all good - send the all good signal
	for( int inx = 0; inx < 5; inx++ )
	{
		ledSetAllOn( );
		__delay_ms( 50 );
		ledSetAllOff( );
		__delay_ms( 50 );
	}

	ledSetAllOff( );

	rtccCopyI2CTime( );

	while( true )
	{

		commRunRoutine( );
		ledSetFrontEnergyRemain( );
		readEmergencyButton( );
		dailyResetCheck( );
		relayControl( );

		// in the following, code blocks {} are used
		// to encapsulate the oneShot variable
		// why
		// - otherwise we would need to declare separately named
		// one shots before the infinite for loop
		// the oneShot variables are local to each block
		// and self contained which makes the code leaner

		//      Test stub for a counter that is super helpful in this function for 
		//      looking at reliability and etcetera. Just throw it into a oneshot
		//        static int counter = 0;
		//        counter++;
		//        ledSetAll((counter>>3)%2, (counter>>2)%2, (counter>>1)%2, counter%2);


		// oneShot
		{
			static bool oneShot = false;

			if( ( msTimer_module % 500 ) <= ONESHOT_WINDOW )
			{
				if( oneShot == false )
				{
					rtccReadTime( &dateTime_global );
					oneShot = true;
				}
			}
			else
			{
				oneShot = false;
			}
		}

		// oneShot
		{
			static bool oneShot = false;


			if( ( msTimer_module % 20000 ) <= ONESHOT_WINDOW )
			{
				if( oneShot == false )
				{
					rtccCopyI2CTime( );
					oneShot = true;

				}
			}
			else
			{
				oneShot = false;
			}

		} //oneShot block

		// oneShot
		{
			static bool oneShot = false;

			if( ( msTimer_module % 120000 ) <= ONESHOT_WINDOW )
			{
				if( oneShot == false )
				{

					storeToEE( );
					oneShot = true;
				}
			}
			else
			{
				oneShot = false;
			}

		} // oneShot block

		// oneShot
		{
			static bool oneShot = false;


			if( ( msTimer_module % 50 ) <= ONESHOT_WINDOW )
			{
				if( oneShot == false )
				{
					ledSetFrontFindMe( );
					oneShot = true;

				}
			}
			else
			{
				oneShot = false;
			}

		} // oneShot block

	} //while (true)

}

void initOscillator( void )
{
	// set clock to fastest available - 16MHz

	// unlock the OSCON high byte
	OSCCON = 0x78; //0x78 = 0111 1000
	OSCCON = 0x9A; // 0x9A = 1001 1010
	// set bits in the OSCCON high byte
	OSCCONbits.NOSC = 0b001; // set fast RC OSC


	// unlock sequence for the OSCCON low byte
	OSCCON = 0x46; // 0x46 = 0100 0110
	OSCCON = 0x57; // 0x58 = 0101 0111
	// set bits in the OSCCON low byte
	// set the clock switch bit - this must be done immediately after the unlock sequence
	OSCCONbits.OSWEN = 1;

	// set clock divider to 1:1
	CLKDIVbits.RCDIV = 0b000;

	return;
}


// Overall Initialization for the whole command board

void init( void )
{


	// the LEDs help determine if we get stuck someplace during init
	// the I2C for rtcc is blocking and if something goes wrong there it could freeze


	ledSetAll( 0, 0, 0, 1 );
	initPorts( );
	ledSetAll( 0, 0, 1, 0 );
	initTimer( );
	ledSetAll( 0, 0, 1, 1 );

	rtccInit( );
	ledSetAll( 0, 1, 0, 0 );
	ledSetAll( 0, 1, 0, 1 );
	ledSetAll( 0, 1, 1, 0 );
	ledSetAll( 0, 1, 1, 1 );

	return;
}

// Sets up ports for a few I/O functions

void initPorts( void )
{

	// ports specific to a peripheral  (SPI, I2C) are initialized
	//	in the functions that initialize the peripheral

	TRISAbits.TRISA0 = 1; // emergency button
	TRISAbits.TRISA3 = 0; // buzzer
	TRISBbits.TRISB6 = 0; // relay control

	// TODO - check emergency button for the pull up/down resistor
	// is there one implemented on the circuit board?
	// or do we use the internal one on the PIC
	// the following line implements the one in the PIC
	//CNPD1bits.CN2PDE = 1; // pull down resistor for pin 2 for emergency button

	return;
}

// Pull information from the stored memory

void initReadGlobalsFromEEPROM( void )
{
	energyCycleAllocation_global = eeReadEnergyCycleAllocationNew( );
	emergencyButton_global = eeReadEmergencyButtonNew( );
	alarms_global = eeReadAlarmNew( );
	resetTime_global = eeReadResetTimeNew( );
	energyUsed_global = eeReadTotalsNew( );
	energyUsed_global.lifetime = eeReadEnergyUsedNew( );
	relayActive_global = eeReadRelayNew( );

	return;
}

// set up a timer that ticks off every ms
// it will use an interrupt to tick off the ms

void initTimer( void )
{

	IEC0bits.T1IE = 0; // disable timer 1 interrupt
	T1CON = 0; // disable and reset timer to known state

	msTimer_module = 0;

	T1CONbits.TCS = 0; // use the instruction cycle clock
	T1CONbits.T1ECS = 0b00; // use LPRC and clock source
	T1CONbits.TGATE = 0; // disable gated timer
	T1CONbits.TCKPS = 0b00; // select 1:1 prescalar
	T1CONbits.TSYNC = 0; // sync external clock input
	TMR1 = 0; // clear the timer register

	// set timer to tick at predictable rate using MACRO

	PR1 = ( FCY / 1000 ) - 1; // interrupt at 1ms interval (note the -1, this is to )
	// always load TIME -1 to make timer more accurate - one count is added due to timer internals
	//  16Mhz = 16,000,000   / 1000   = 16,000.  If at 16Mhz we interval every 16,000 clock cycles we get 1ms timer

	IPC0bits.T1IP = 0x04; // interrupt priority level
	IFS0bits.T1IF = 0; // clear timer interrupt flag
	IEC0bits.T1IE = 1; // enable interrupt
	T1CONbits.TON = 1; // enable timer

	return;
}

void __attribute__( ( __interrupt__, __no_auto_psv__ ) ) _T1Interrupt( void )
{

	TMR1 = 0; // reset the accumulator
	IFS0bits.T1IF = 0; // reset the interrupt flag

	// timer is designed to interrupt at 0.001s (1ms)

	// the following variable must be declared as 'volatile'
	// this means the value can change unexpectedly, even in the middle
	// of an operation with the variable
	// because this interrupt can trigger at any time
	msTimer_module++; // increment every 1ms

	// control our timer rollover to prevent overflow
	// not critical that we do this, but it is more controlled than an overflow
	if( msTimer_module >= 4000000000 )
	{
		msTimer_module = 0;
	}

	return;
}

// this function compares the down time against the reset time and
// resets the allocation if the reset time is passes during the power outage

void powerOnCheckForAllocationReset( void )
{
	// the clock chip saves the timestamp for when power failed
	// the clock chip saves the timestamp for when the power is restored

	// It starts by finding the time when the next reset would have occurred after 
	// an outage and then checks to see if this happened before or after the power
	// was restored

	// This function seems to be working well as of March 2, 2021 but may still
	// have a few bugs --Ben Weaver

	struct date_time timePowerFail;
	struct date_time timePowerRestore;

	getResetTimes( &timePowerFail, &timePowerRestore );

	unsigned char resetTempTimeMonth;
	unsigned char resetTempTimeDay;
	unsigned char resetTempTimeYear;

	int failMinute = ( timePowerFail.minute - 0x30 ) + 10 * ( timePowerFail.minuteTens - 0x30 );
	int failHour = ( timePowerFail.hour - 0x30 ) + 10 * ( timePowerFail.hourTens - 0x30 );
	int failDay = ( timePowerFail.day - 0x30 ) + 10 * ( timePowerFail.dayTens - 0x30 );
	int failMonth = ( timePowerFail.month - 0x30 ) + 10 * ( timePowerFail.monthTens - 0x30 );

	int restoreMinute = ( timePowerRestore.minute - 0x30 ) + 10 * ( timePowerRestore.minuteTens - 0x30 );
	int restoreHour = ( timePowerRestore.hour - 0x30 ) + 10 * ( timePowerRestore.hourTens - 0x30 );
	int restoreDay = ( timePowerRestore.day - 0x30 ) + 10 * ( timePowerRestore.dayTens - 0x30 );
	int restoreMonth = ( timePowerRestore.month - 0x30 ) + 10 * ( timePowerRestore.monthTens - 0x30 );



	// determine which day the reset was to occur in relation to the day the power went out

	// if the power fail time was after the reset time then that means
	// that the next reset time is actually tomorrow since it has already occurred today
	// add one to the day we are looking for with the reset
	if( failHour > resetTime_global.hour )
	{
		resetTempTimeDay = ( failDay + 1 );
	}
	else if( failHour == resetTime_global.hour )
	{
		if( failMinute > resetTime_global.minute )
		{
			resetTempTimeDay = ( failDay + 1 );
		}
		else
		{
			resetTempTimeDay = failDay;
		}
	}
	else
	{
		resetTempTimeDay = failDay;
	}


	// now that we have the day, we need to check the month
	// account for day rollover in the month
	resetTempTimeMonth = failMonth;

	// account for days in the month
	switch( failMonth )
	{
			// first check is for 31 days
			// these case statements fall through until the break
		case 1: //  Jan
		case 3: //  Mar
		case 5: //  May
		case 7: //  Jul
		case 8: //  Aug
		case 10: // Oct
		case 12: // Dec
			if( resetTempTimeDay > 31 )
			{
				resetTempTimeDay = 1;
				resetTempTimeMonth++;
			}
			break;

			// second check is for 28/29 days
		case 2: //  Feb - check for leap year
			// technically we should check each century as well (no leap year)
			// bit it is very unlikely that the meter will be running then, so skip check for now)
			if( ( dateTime_global.year % 4 ) == 0 )
			{
				if( resetTempTimeDay >= 29 )
				{
					resetTempTimeDay = 1;
					resetTempTimeMonth++;
				}
			}
			else
			{
				if( resetTempTimeDay >= 28 )
				{
					resetTempTimeDay = 1;
					resetTempTimeMonth++;
				}
			}
			break;
			// third check is for 30 days
		case 4: //  Apr
		case 6: //  Jun
		case 9: //  Sept
		case 11: // Nov
			if( resetTempTimeDay > 30 )
			{
				resetTempTimeDay = 1;
				resetTempTimeMonth++;
			}
			break;
	}

	// did the month roll over into the next year?
	resetTempTimeYear = 0;
	if( resetTempTimeMonth > 12 )
	{
		resetTempTimeMonth = 1;
		resetTempTimeYear = 1;
	}

	bool resetNeeded;

	// Comparing to see if power was restored before or after the expected reset
	// Check date (resets if later month, later day in month, or new year)
	if( ( restoreMonth > resetTempTimeMonth ) ||
	 ( ( restoreMonth == resetTempTimeMonth ) && ( restoreDay > resetTempTimeDay ) ) ||
	 ( restoreMonth < resetTempTimeMonth )
	 )
	{

		resetNeeded = true;

		// Checking the time of day to see if the power came on after the reset
	}
	else if( restoreDay == resetTempTimeDay )
	{
		int tempRestoreTimeCode = restoreHour * 60 + restoreMinute;
		int tempResetTimeCode = resetTime_global.hour * 60 + resetTime_global.minute;

		if( tempRestoreTimeCode >= tempResetTimeCode )
		{
			resetNeeded = true;
		}
		else
		{
			resetNeeded = false;
		}
	}
	else
	{
		resetNeeded = false;
	}

	if( resetNeeded == true )
	{
		dailyReset( );
	}

	return;
}

void dailyResetCheck( void )
{
	//  are we at the reset time (hour and minute)
	//	don't check seconds because there is a potential that we skip over a second
	//	in between calling this function

	// make sure this function runs only once for each reset

	static bool resetComplete = false;

	if( ( dateTime_global.minute == resetTime_global.minute ) && ( dateTime_global.hour == resetTime_global.hour ) )
	{
		if( resetComplete == false )
		{
			dailyReset( );
			resetComplete = true;
		}
	}
	else
	{
		resetComplete = false;
	}

	return;
}

// Resets the allocation

void dailyReset( void )
{

	energyUsed_global.previousCycleUsed = energyUsed_global.lifetime - energyUsed_global.lastReset;
	energyUsed_global.lastReset = energyUsed_global.lifetime;
	energyAdd_global = 0;

	eeWriteEnergyTotalsNew( energyUsed_global );

	eeWriteEnergyLifetimeNew( energyUsed_global.lifetime );

	// reset allocation to what is stored in EEPROM
	// this makes sure everything is in sync
	energyCycleAllocation_global = eeReadEnergyCycleAllocationNew( );

	return;
}

// When the emergency button is pressed, add allocation (if enabled)

void readEmergencyButton( void )
{

#define EMERGENCY_BUTTON_TIMER 250      // length of time in ms to wait between button presses
	static bool oneShot = false; // keep from reading multiple presses being read for only one press
	static unsigned long buttonTimer; // allow holding the button to repeat, but controlled

	if( emergencyButton_global.enabled == true )
	{
		if( PORT_READ_BUTTON_EMERGENCY == 1 )
		{
			if( oneShot == false )
			{
				energyCycleAllocation_global += emergencyButton_global.energyAmount;
				buttonTimer = msTimer_module + EMERGENCY_BUTTON_TIMER; // wait between presses
				oneShot = true;
			}
			else
			{
				// msTimer can roll over back to 0 - need to account for that
				if( ( msTimer_module >= buttonTimer ) || ( ( buttonTimer - msTimer_module ) > EMERGENCY_BUTTON_TIMER ) )
				{
					oneShot = false;
				}
			}
		}
		else
		{
			oneShot = false;
		}
	}

	return;
}

// Stores all values back into the EEPROM storage

void storeToEE( void )
{

	eeWriteEnergyLifetimeNew( energyUsed_global.lifetime );

	return;
}

// Runs the relay based on allocation

void relayControl( void )
{
	static unsigned char lastSecond;

	unsigned long tempEnergyUsed;

	tempEnergyUsed = energyUsed_global.lifetime - energyUsed_global.lastReset;

	if( lastSecond != dateTime_global.second )
	{
		if( relayActive_global )
		{
			if( tempEnergyUsed < ( energyCycleAllocation_global + energyAdd_global ) )
			{
				PORT_WRITE_RELAY = 1;
			}
			else
			{
				PORT_WRITE_RELAY = 0;
				// the power (watts) is driven by the power sens board
				// it is not easy to manually change it to zero when the relay
				// turns off
				// this means that the power will 'decay' in the power sense module
				// until it eventually reaches zero
			}
		}
		lastSecond = dateTime_global.second;
	}

	return;
}



/******************
 The following are error traps which will prevent the PIC from resetting most of the time
 They simply return back to the program
 This doesn't solve the problem, but it prevents the meter from resetting
 There is a potential that some data corruption may have occurred, but most of the time it will work itself out
 ***********************/
#if ERROR_TRAP_INTERRUPTS_ACTIVE == true

void __attribute__( ( interrupt, no_auto_psv ) ) _OscillatorFail( void )
{
	INTCON1bits.OSCFAIL = 0; //Clear the trap flag

	return;
}

void __attribute__( ( interrupt, no_auto_psv ) ) _AddressError( void )
{
	// if lEDS are put in debug mode one can count or toggle LEDS to show one of these routines was entered

	// debug methods
	// place a debug breakpoint in this function to stop execution
	// then line-by-line execute to get back to the function which caused the error
	// it will return to the next executing line
	// to get the actual line where the error occurred, you need to use the getErrLoc function
	//  getErrLoc is in an assembly ASM file
	//  this is a file which needs to be added to the project
	// then it can retrieve the actual instruction where the error occurred 
	// the errLoc return is a function pointer which can be called to return to the line where the error occurred

	//		errLoc = getErrLoc();	// get the location of error function


	INTCON1bits.ADDRERR = 0; //Clear the trap flag

	//	errLoc();		// return to error function directly
	// it is preferred to return by RETFIE instruction. 

	return;
}

void __attribute__( ( interrupt, no_auto_psv ) ) _StackError( void )
{
	INTCON1bits.STKERR = 0; //Clear the trap flag

	return;
}

void __attribute__( ( interrupt, no_auto_psv ) ) _MathError( void )
{
	INTCON1bits.MATHERR = 0; //Clear the trap flag

	return;
}

#endif