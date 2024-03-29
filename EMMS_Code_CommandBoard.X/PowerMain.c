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
 This eventually is used to set the meter time
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

#define TIMER_ROLLOVER			4000000000	// maximum timer value before we force a rollover back to 0
#define TIMER_ROLLOVER_CHECK	100000000	// if the difference between the timer and our check is greater than this then our check time has rolled over and we need to wait


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
struct moduleInfo_struct moduleInfo_global[MODULE_COUNT];
struct energy_info energyUsed_global; // energy status of the system
struct date_time_struct dateTime_global; // the current date time - periodically pulled from RTCC
struct alarm_info_struct alarms_global; // alarms
unsigned long energyCycleAllocation_global; // how much energy is allocated per cycle
struct reset_time resetTime_global; // hour and minute of the cycle reset
unsigned int relayMode_global; // the relay mode
struct emergency_button emergencyButton_global; // emergency button enabled and allocation amount
struct calibration_struct calibrationFactors_global;   // the calibration factors


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
void initModuleInfo( void );
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

#if defined UART2_DEBUG_IDE || defined UART2_DEBUG_OUTPUT	
	// need to leave B0 and B1 alone for debug mode
	TRISBbits.TRISB2 = 1;
	TRISBbits.TRISB3 = 1;
	TRISBbits.TRISB4 = 1;
	TRISBbits.TRISB5 = 1;
	TRISBbits.TRISB6 = 1;
	TRISBbits.TRISB7 = 1;
	TRISBbits.TRISB8 = 1;
	TRISBbits.TRISB9 = 1;
	TRISBbits.TRISB10 = 1;
	TRISBbits.TRISB11 = 1;
	TRISBbits.TRISB12 = 1;
	TRISBbits.TRISB13 = 1;
	TRISBbits.TRISB14 = 1;
	TRISBbits.TRISB15 = 1;
#else
	TRISB = 0b1111111111111111; // 0b1111111111111111 = 0xFFFF
#endif	

	// set all ports to low by default to start
	PORTA = 0b0000000000000000; // this is equivalent to setting all of the individual bits

#if defined UART2_DEBUG_IDE || defined UART2_DEBUG_OUTPUT	
	// need to leave B0 and B1 alone for debug mode
	PORTBbits.RB2 = 0;
	PORTBbits.RB3 = 0;
	PORTBbits.RB4 = 0;
	PORTBbits.RB5 = 0;
	PORTBbits.RB6 = 0;
	PORTBbits.RB7 = 0;
	PORTBbits.RB8 = 0;
	PORTBbits.RB9 = 0;
	PORTBbits.RB10 = 0;
	PORTBbits.RB11 = 0;
	PORTBbits.RB12 = 0;
	PORTBbits.RB13 = 0;
	PORTBbits.RB14 = 0;
	PORTBbits.RB15 = 0;
#else
	PORTB = 0b0000000000000000; // 0b0000000000000000 = 0
#endif


	ledInit( );

	resetReportDisplay( );

	ledSetAllOff( );

	initModuleInfo( );

	commInit( );
//	commDebugPrintStringIndentln( 0, "\n\nStartup" );
	ledSetAll( 1, 0, 1, 1 );

	init( );
	ledSetAll( 1, 0, 0, 0 );

	// read in the EEPROM values that are used as global
	initReadGlobalsFromEEPROM( );
	ledSetAll( 1, 0, 0, 1 );

	powerOnCheckForAllocationReset( );
	ledSetAll( 1, 0, 1, 0 );

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

//	commDebugPrintStringIndentln( 0, "\nStartup Complete\n\n" );

	while( true )
	{

		commRunRoutine( );
		ledSetFrontEnergyRemain( );
		readEmergencyButton( );
		dailyResetCheck( );
		relayControl( );

		// in the following, code blocks {} are used
		// to encapsulate the nextRunTime variable
		// why
		// - otherwise we would need to declare separately named
		// nextRunTime variables outside the while(true) loop
		// the nextRunTime variables are local to each block
		// and self contained which makes the code leaner
		//
		// in the test to see if the timer is triggered we need to check for timer rollover
		// if the difference is really large then we rolled over and need to wait until the internal timer rolls over as well


		// isolate nextRunTime block
		{
			static unsigned long nextRunTime = 0;

			if( ( msTimer_module > nextRunTime ) && ( ( msTimer_module - nextRunTime ) < TIMER_ROLLOVER_CHECK ) )
			{
				nextRunTime = msTimer_module + 500;
				if( nextRunTime >= TIMER_ROLLOVER )
				{
					nextRunTime -= TIMER_ROLLOVER;
				}

				rtccReadTime( &dateTime_global );

			}

		} // isolate nextRunTime block


		// isolate nextRunTime block
		{
			static unsigned long nextRunTime = 0;

			if( ( msTimer_module > nextRunTime ) && ( ( msTimer_module - nextRunTime ) < TIMER_ROLLOVER_CHECK ) )
			{
				nextRunTime = msTimer_module + 20000;
				if( nextRunTime >= TIMER_ROLLOVER )
				{
					nextRunTime -= TIMER_ROLLOVER;
				}

				rtccCopyI2CTime( );

			}

		} // isolate nextRunTime block


		// isolate nextRunTime block
		{

			static unsigned long nextRunTime = 0;

			if( ( msTimer_module > nextRunTime ) && ( ( msTimer_module - nextRunTime ) < TIMER_ROLLOVER_CHECK ) )
			{
				nextRunTime = msTimer_module + 120000;
				if( nextRunTime >= TIMER_ROLLOVER )
				{
					nextRunTime -= TIMER_ROLLOVER;
				}

				storeToEE( );

			}

		} // isolate nextRunTime block

		// isolate nextRunTime block
		{
			static unsigned long nextRunTime = 0;

			if( ( msTimer_module > nextRunTime ) && ( ( msTimer_module - nextRunTime ) < TIMER_ROLLOVER_CHECK ) )
			{
				nextRunTime = msTimer_module + 50;
				if( nextRunTime >= TIMER_ROLLOVER )
				{
					nextRunTime -= TIMER_ROLLOVER;
				}

				ledSetFrontFindMe( );

			}

		} // isolate nextRunTime block

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

void initModuleInfo( void )
{
	for( int inx = 0; inx < MODULE_COUNT; inx++ )
	{
		for( int jnx = 0; jnx < MODULE_INFO_SIZE_SMALL; jnx++ )
		{
			moduleInfo_global[inx].info0[jnx] = CHAR_NULL;
			moduleInfo_global[inx].info1[jnx] = CHAR_NULL;
			moduleInfo_global[inx].info2[jnx] = CHAR_NULL;
			moduleInfo_global[inx].info3[jnx] = CHAR_NULL;
			moduleInfo_global[inx].info4[jnx] = CHAR_NULL;
		}

		for( int jnx = MODULE_INFO_SIZE_SMALL; jnx < MODULE_INFO_SIZE_LARGE; jnx++ )
		{
			moduleInfo_global[inx].info4[jnx] = CHAR_NULL;
		}

		moduleInfo_global[inx].info0[0] = 'N';
		moduleInfo_global[inx].info0[1] = 'o';
		moduleInfo_global[inx].info0[2] = 'n';
		moduleInfo_global[inx].info0[3] = 'e';
		moduleInfo_global[inx].info0[4] = '-';
		moduleInfo_global[inx].info0[5] = inx + 48;
	}

	strcpy2( moduleInfo_global[0].info0, MODULE_INFO_THIS_0 );
	strcpy2( moduleInfo_global[0].info1, MODULE_INFO_THIS_1 );
	strcpy2( moduleInfo_global[0].info2, MODULE_INFO_THIS_2 );
	strcpy2( moduleInfo_global[0].info3, MODULE_INFO_THIS_3 );
	strcpy2( moduleInfo_global[0].info4, MODULE_INFO_THIS_4 );

	return;
}

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
    // this seems to be working fine with this comments - likely can delete these lines of code
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

    // energyUser_global is set by two different calls to eeprom on purpose
	energyUsed_global = eeReadTotalsNew( );
	energyUsed_global.lifetime = eeReadEnergyUsedNew( );

	relayMode_global = eeReadRelayNew( );
    calibrationFactors_global = eeReadCalibrationNew();
    
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
	if( msTimer_module >= TIMER_ROLLOVER )
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

	struct date_time_struct timePowerDown;
	struct date_time_struct timePowerUp;

	rtccGetPowerTimes( &timePowerDown, &timePowerUp );

	unsigned char resetTempTimeMonth;
	unsigned char resetTempTimeDay;
	unsigned char resetTempTimeYear;


	// determine which day the reset was to occur in relation to the day the power went out

	// if the power fail time was after the reset time then that means
	// that the next reset time is actually tomorrow since it has already occurred today
	// add one to the day we are looking for with the reset
	if( timePowerDown.hour > resetTime_global.hour )
	{
		resetTempTimeDay = ( timePowerDown.day + 1 );
	}
	else if( timePowerDown.hour == resetTime_global.hour )
	{
		if( timePowerDown.minute > resetTime_global.minute )
		{
			resetTempTimeDay = ( timePowerDown.day + 1 );
		}
		else
		{
			resetTempTimeDay = timePowerDown.day;
		}
	}
	else
	{
		resetTempTimeDay = timePowerDown.day;
	}

	// now that we have the day, we need to check the month
	// account for day rollover in the month
	resetTempTimeMonth = timePowerDown.month;

	// account for days in the month
	switch( timePowerDown.month )
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
	if( ( timePowerUp.month > resetTempTimeMonth ) ||
	 ( ( timePowerUp.month == resetTempTimeMonth ) && ( timePowerUp.day > resetTempTimeDay ) ) ||
	 ( timePowerUp.month < resetTempTimeMonth )
	 )
	{

		resetNeeded = true;

		// Checking the time of day to see if the power came on after the reset
	}
	else if( timePowerUp.day == resetTempTimeDay )
	{
		int tempRestoreTimeCode = timePowerUp.hour * 60 + timePowerUp.minute;
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
		switch( relayMode_global )
		{
			case 0: // Off

				PORT_WRITE_RELAY = 0;

				break;

			case 1: // On

				PORT_WRITE_RELAY = 1;

				break;

			case 2: // Auto

				if( tempEnergyUsed < energyCycleAllocation_global )
				{
					PORT_WRITE_RELAY = 1;
				}
				else
				{
					PORT_WRITE_RELAY = 0;
					// the power (watts) is driven by the power sense board
					// it is not easy to manually change it to zero when the relay
					// turns off
					// this means that the power will 'decay' in the power sense module
					// until it eventually reaches zero
				}

				break;
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