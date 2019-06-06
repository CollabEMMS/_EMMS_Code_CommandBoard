/*
 TODO

 DONE
 fix NULL_CHAR
 ----make CHAR_NULL universally available somehow


 DONE - needs tested
 add UART handling
 ----first try - read from uart in recv function
 --------remove UART interrupt
 ----second try - leave uart interrupt - flag that char is available
 --------make sure interval is quick enough to catch all characters at 9600
 ----third try - have interrupt build receive buffer
 --------this sucks because of volatile

 --MAYBE incorporate uart large buffer as in UI code.


 fix UI communications
 ----must remove existing due to poor buffer bounds handling in the SharedCOmmunications
 --------grrr

 ---- need to rework how the UI updates all of its data
 --------maybe periodic full refresh
 --------are we able to send a trigger from the CommandBoard?
 ------------research this at least a little



 */

/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
//#include "Communications.h"
#include "I2C_RTCC.h"
#include "MasterComm.h"
#include "Delays.h"
#include "PowerUART.h"
#include "OC_PWM.h"
#include "EEPROM.h"

/****************
 MACROS
 ****************/
/*
 The following section sets a macros with the build date and time
 This eventually gets displayed on the UI and the power box version
 */

//#define POWER_BOX_CODE_VERSION "20190602a"

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

#define POWER_BOX_CODE_VERSION ((char[]) {__DATE__ [9], __DATE__ [10], (BUILD_MONTH / 10) + 48, (BUILD_MONTH % 10) + 48, (__DATE__ [4] == ' ' ? '0' : __DATE__ [4]), __DATE__ [5], (CODE_REVISION / 10) + 48, (CODE_REVISION % 10) + 48, 0})

/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/
// external
// internal only


//exernal & internal
bool lightsModeActive = false;


//FIX = this is clunky and may not work
char powerBoxCodeVersion[9] = "20190603";

long highAlloc;
long lowAlloc;

char emerButtonEnable;
int emerButtonEnergyAllocate;

int resetTimeHour;
int resetTimeMinute;

char isHigh = 0xFF;
char relayActive;


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
  any functions used internally and externally (prototype here and in the .h file)
     should be marked
 ****************/

void dailyResetPowerOnCheck( void );
void lightsMode( void );


// internal and external
void debugLEDSet( int LEDNum, bool On );
void debugLEDRotateA( int minLED, int maxLED );
void debugLEDRotateB( int minLED, int maxLED );
void debugLEDToggle( int LEDNum );

/****************
 CODE
 ****************/


/* Includes *******************************************************************/








//#define LEDS_FOR_DEBUG  // comment this line for normal operation (LEDS show power remaining)
// uncomment for using the LEDS for debugging


/* Main ***********************************************************************/

/* main
 * Initializes and runs through the main code that is repetitively called
 */

//unsigned long powerWatts = 0;
//unsigned long powerVolts = 0;
//unsigned long powerAmps = 0;

void init( void );
void initRTCCDisplay( void );
void initVars( void );
void setClock( void );
void dailyResetCheck( void );
void dailyReset( void );
void initPorts( void );
void enableInterrupts( void );
void readButton( void );
void relayControl( void );
void storeToEE( void );
void setHighLow( void );

void LEDrunup( int LEDrundelay )
{
    LED1_SET = 0;
    LED2_SET = 0;
    LED3_SET = 0;
    LED4_SET = 0;
    delayMS( LEDrundelay );
    LED1_SET = 1;
    LED2_SET = 0;
    LED3_SET = 0;
    LED4_SET = 0;
    delayMS( LEDrundelay );
    LED1_SET = 0;
    LED2_SET = 1;
    LED3_SET = 0;
    LED4_SET = 0;
    delayMS( LEDrundelay );
    LED1_SET = 0;
    LED2_SET = 0;
    LED3_SET = 1;
    LED4_SET = 0;
    delayMS( LEDrundelay );
    LED1_SET = 0;
    LED2_SET = 0;
    LED3_SET = 0;
    LED4_SET = 1;
    delayMS( LEDrundelay );

}

void LEDrundown( int LEDrundelay )
{

    LED1_SET = 0;
    LED2_SET = 0;
    LED3_SET = 0;
    LED4_SET = 1;
    delayMS( LEDrundelay );
    LED1_SET = 0;
    LED2_SET = 0;
    LED3_SET = 1;
    LED4_SET = 0;
    delayMS( LEDrundelay );
    LED1_SET = 0;
    LED2_SET = 1;
    LED3_SET = 0;
    LED4_SET = 0;
    delayMS( LEDrundelay );
    LED1_SET = 1;
    LED2_SET = 0;
    LED3_SET = 0;
    LED4_SET = 0;
    delayMS( LEDrundelay );
    LED1_SET = 0;
    LED2_SET = 0;
    LED3_SET = 0;
    LED4_SET = 0;
    delayMS( LEDrundelay );

}

void LEDrun( int LEDrundelay )
{
    LEDrunup( LEDrundelay );
    LEDrundown( LEDrundelay );
}

int main( void )
{

    //    powerBoxCodeVersion[0] = '2';
    //    powerBoxCodeVersion[1] = '0';
    //    powerBoxCodeVersion[2] = '1';
    //    powerBoxCodeVersion[3] = '9';
    //    powerBoxCodeVersion[4] = '0';
    //    powerBoxCodeVersion[5] = '6';
    //    powerBoxCodeVersion[6] = '0';
    //    powerBoxCodeVersion[7] = '3';
    //    powerBoxCodeVersion[8] = CHAR_NULL;

    int reset_bits;
    reset_bits = RCON;
    RCON = 0;

    bool b[16];
    //    reset_bits = 0b10000100;

    for( int j = 0; j < 16; ++j )
    {
	b [j] = 0 != (reset_bits & (1 << j));
    }

    LED1_DIR = 0;
    LED2_DIR = 0;
    LED3_DIR = 0;
    LED4_DIR = 0;

    for( int inx = 0; inx < 0; inx++ )
    {
	LEDrun( 100 );

	LEDrunup( 25 );
	LED1_SET = 0;
	LED2_SET = 0;
	LED3_SET = 0;
	LED4_SET = 0;
	delayMS( 250 );
	LED1_SET = b[0];
	LED2_SET = b[1];
	LED3_SET = b[2];
	LED4_SET = b[3];
	delayMS( 1000 );
	LEDrunup( 25 );
	LED1_SET = b[4];
	LED2_SET = b[5];
	LED3_SET = b[6];
	LED4_SET = b[7];
	delayMS( 1000 );
	LEDrunup( 25 );
	LED1_SET = 0;
	LED2_SET = 0;
	LED3_SET = 0;
	LED4_SET = 0;
	delayMS( 250 );

	LED1_SET = b[8];
	LED2_SET = b[9];
	LED3_SET = b[10];
	LED4_SET = b[11];
	delayMS( 1000 );
	LEDrunup( 25 );
	LED1_SET = 0;
	LED2_SET = 0;
	LED3_SET = 0;
	LED4_SET = 0;
	delayMS( 250 );

	LED1_SET = b[12];
	LED2_SET = b[13];
	LED3_SET = b[14];
	LED4_SET = b[15];
	delayMS( 1000 );
	LEDrunup( 25 );
	LED1_SET = 0;
	LED2_SET = 0;
	LED3_SET = 0;
	LED4_SET = 0;
	delayMS( 250 );
    }

    for( int inx = 0; inx < 5; inx++ )
    {
	LED1_SET = 1;
	LED2_SET = 1;
	LED3_SET = 1;
	LED4_SET = 1;
	delayMS( 100 );
	LED1_SET = 0;
	LED2_SET = 0;
	LED3_SET = 0;
	LED4_SET = 0;
	delayMS( 100 );
    }

    init( );
    initRTCCDisplay( );
    SPIMasterInit( );

    for( int inx = 0; inx < 5; inx++ )
    {
	LED1_SET = 1;
	LED2_SET = 1;
	LED3_SET = 1;
	LED4_SET = 1;
	delayMS( 10 );
	LED1_SET = 0;
	LED2_SET = 0;
	LED3_SET = 0;
	LED4_SET = 0;
	delayMS( 10 );
    }

    LED1_SET = 0;
    LED2_SET = 0;
    LED3_SET = 0;
    LED4_SET = 0;

    communications( true );
    dailyResetPowerOnCheck( );


    unsigned int timerCounterCommunications = 0;
    unsigned int timerCounterComFunctions = 0;
    unsigned int timerCounterLowPriority = 0;
    unsigned int timerCounterReadI2CRTCC = 0;
    unsigned int timerCounterReadIntRTCC = 0;
    unsigned int timerCounterLightsMode = 0;

    unsigned int timerCounterCommunicationsUART = 0;

    bool runCommunications = false;
    bool runComFunctions = false;
    bool runLowPriority = false;
    bool runReadI2CRTCC = false;
    bool runReadIntRTCC = false;
    bool runLightsMode = false;

    bool runCommunicationsUART = false;

#define TIMER_MS_COUNT		    2000    // timer count for one ms to pass (2000 - 1ms))
#define TIMER_HALF_MS_COUNT	    1000    // timer count for one half ms to pass (2000 - 1ms))
#define TIMER_DELAY_COMMUNICATIONS  0 //4	    // time in ms to run function
#define TIMER_DELAY_LOW_PRIORITY    2000 //1000	    // time in ms to run function
#define TIMER_DELAY_READ_I2C_RTCC   120000 //60000   // time in ms to run function
#define TIMER_DELAY_READ_INT_RTCC   2000 //1000	    // time in ms to run function
#define TIMER_DELAY_COM_FUNCTIONS   2 //1 	    // time in ms to run function
#define TIMER_DELAY_LIGHTS_MODE	    50 // time in ms to run function
#define TIMER_DELAY_COMMUNICATIONS_UART	    1	// half ms	    // time in ms to run function


    readTimeI2C( );
    writeTime( timeYear, timeMonth, timeDay, timeHour, timeMinute, timeSecond );

    // check if we lost power over a reset time


    bool enabledSPI = false;

    while( 1 )
    {
	if( TMR1 > TIMER_MS_COUNT )
	{
	    TMR1 = 0;
	    timerCounterCommunications++;
	    timerCounterComFunctions++;
	    timerCounterLowPriority++;
	    timerCounterReadI2CRTCC++;
	    timerCounterReadIntRTCC++;
	    timerCounterLightsMode++;

	    timerCounterCommunicationsUART++;
	}

	if( timerCounterCommunications >= TIMER_DELAY_COMMUNICATIONS )
	{
	    runCommunications = true;
	    timerCounterCommunications = 0;
	}

	if( timerCounterComFunctions >= TIMER_DELAY_COM_FUNCTIONS )
	{
	    runComFunctions = true;
	    timerCounterComFunctions = 0;
	}

	if( timerCounterLowPriority >= TIMER_DELAY_LOW_PRIORITY )
	{
	    runLowPriority = true;
	    timerCounterLowPriority = 0;
	}

	if( timerCounterReadI2CRTCC >= TIMER_DELAY_READ_I2C_RTCC )
	{
	    runReadI2CRTCC = true;
	    timerCounterReadI2CRTCC = 0;
	}

	if( timerCounterReadIntRTCC >= TIMER_DELAY_READ_INT_RTCC )
	{
	    runReadIntRTCC = true;
	    timerCounterReadIntRTCC = 0;
	}

	if( timerCounterLightsMode >= TIMER_DELAY_LIGHTS_MODE )
	{
	    runLightsMode = true;
	    timerCounterLightsMode = 0;
	}

	if( timerCounterCommunicationsUART >= TIMER_DELAY_COMMUNICATIONS_UART )
	{
	    runCommunicationsUART = true;
	    timerCounterCommunicationsUART = 0;
	}




	//	if( runCommunicationsUART == true )
	//	{
	//	    communicationsUART( );
	//	    runCommunicationsUART = false;
	//	}

	if( runLightsMode == true )
	{
	    lightsMode( );
	    runLightsMode = false;
	}

	if( runCommunications == true )
	{
	    timerCounterCommunications = 0;
	    //	    enabledSPI = communications( false );
	    communications( false );
	    runCommunications = false;
	    //enabledSPI = false;
	}

	if( enabledSPI == false )
	{
	    readButton( );
	    storeToEE( );


	    // FIX LIKELY DONE
	    // i think this just needs to go away
	    // it is not commented right now so the counter is zeroed
	    // no change needed to make this work - this is just extra code right now
	    if( runComFunctions == true )
	    {
		timerCounterComFunctions = 0;
		//		commFunctions( ); //UART processing
		runComFunctions = false;
	    }

	    if( runLowPriority == true )
	    {
		timerCounterLowPriority = 0;

		updateLEDs( );
		dailyResetCheck( );
		relayControl( );

		runLowPriority = false;
	    }

	    if( runReadIntRTCC == true )
	    {
		timerCounterReadIntRTCC = 0;
		readTime( );
		runReadIntRTCC = false;
	    }

	    if( runReadI2CRTCC == true )
	    {
		timerCounterReadI2CRTCC = 0;
		readTimeI2C( );
		writeTime( timeYear, timeMonth, timeDay, timeHour, timeMinute, timeSecond );
		runReadI2CRTCC = false;
	    }
	}
    }

}


/* Functions ******************************************************************/

/* init
 * Calls each individual initialization method
 */
void init( void )
{
    initI2C( );
    startClock( );
    setClock( );
    initPorts( );
    initVars( );
    readI2CPowerTimes( );
    //    initPWMeasurement( );
    initUART( );
    //    initOC_PWM();
    enableInterrupts( );


    // FIX NEEDS IMPLEMENTED
    // need to figure out ow to issue commands from outside communications
    // we make a function that is exposed that we can call
    // commCommandRebootNow( );



    //    commandBuilder1( "Reboot", "Now", "0" );
    //    commandBuilder1( "Reboot", "Now", "0" );
}

/* initVars
 * Initializes variables to their starting values (usually 0)
 */
void initVars( void )
{

    EEreadAll( );

    // set up highAlloc and lowAlloc
    if( (highAlloc == lowAlloc) && (tba_energyAllocation != 0) )
    {
	highAlloc = tba_energyAllocation;
	lowAlloc = (tba_energyAllocation * 3) / 4;
    }

    setHighLow( );

}

/* setClocks()
 * At boot time, check which RTCC has the most recent time and synchronize them.
 * If this is a new build of the code, use the build time to set both RTCCs.
 */
void setClock( void )
{
    unsigned char year, month, day, hour, minute, second;
    readI2CTime( &year, &month, &day, &hour, &minute, &second );

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
    if( buildDate > I2CDate )
    {
	// Correct for the 17 seconds it takes to program the PIC
	second = BUILD_SECOND + 17;
	minute = BUILD_MINUTE;
	hour = BUILD_HOUR;
	if( second > 59 )
	{
	    minute++;
	    if( minute > 59 )
		hour++;
	}

	setI2CTime( BUILD_YEAR, BUILD_MONTH, BUILD_DAY, hour, minute % 60, second % 60 );
    }
}

void dailyResetPowerOnCheck( void )
{
    // the clock chip saves the timestamp for when power failed
    // the clock chip saves the timestamp for when the power is restored

    // this function compares the down time against the reset time and
    // resets the allocation
    //

    // this needs verified
    // right now the powerDown and powerUp is not working correctly
    // don't try to reset during a power outage for now




    // on startup check to see if we need to reset the power allocation

    unsigned char resetTimeMonth;
    unsigned char resetTimeDay;


    // determine which day the reset was to occurr in relation to the day the power went out
    if( powerFailTimeHour > resetTimeHour )
    {
	resetTimeDay = powerFailTimeDay + 1;
    }
    else if( powerFailTimeHour == resetTimeHour )
    {
	if( powerFailTimeMinute > resetTimeMinute )
	{
	    resetTimeDay = powerFailTimeDay + 1;
	}
    }
    else
    {
	resetTimeDay = powerFailTimeDay;
    }
    resetTimeMonth = powerFailTimeMonth;

    // account for days in the month
    switch( powerFailTimeMonth )
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
	if( resetTimeDay > 31 )
	{
	    resetTimeDay = 1;
	    resetTimeMonth++;
	}
	break;

    case 2:
	if( (timeYear % 4) == 0 )
	{
	    if( resetTimeDay >= 29 )
	    {
		resetTimeDay = 1;
		resetTimeMonth++;
	    }
	}
	else
	{
	    if( resetTimeDay >= 28 )
	    {
		resetTimeDay = 1;
		resetTimeMonth++;
	    }
	}
	break;
    case 4:
    case 6:
    case 9:
    case 11:
	if( resetTimeDay > 30 )
	{
	    resetTimeDay = 1;
	    resetTimeMonth++;
	}
	break;
    }

    // no more than 12 months
    // recognized glitch here if power is out and reset over the new year
    if( resetTimeMonth > 12 )
    {
	resetTimeMonth = 1;
    }

    // turn the information into numbers that we can easily compare
    unsigned long powerFailTimeSerial;
    unsigned long resetTimeSerial;
    unsigned long powerRestoreTimeSerial;
    // some of these variables need to be global so we can send them to the UI


    powerFailTimeSerial = (powerRestoreTimeMonth * 1000000) + (powerRestoreTimeDay * 10000) + (powerRestoreTimeHour * 100) + (powerRestoreTimeMinute);
    resetTimeSerial = (resetTimeMonth * 1000000) + (resetTimeDay * 10000) + (resetTimeHour * 100) + (resetTimeMinute);
    powerRestoreTimeSerial = (powerFailTimeMonth * 1000000) + (powerFailTimeDay * 10000) + (powerFailTimeHour * 100) + (powerFailTimeMinute);


    if( powerFailTimeSerial > resetTimeSerial )
    {
	dailyReset( );
    }

    return;
}

void dailyResetCheck( void )
{

    static bool resetComplete = false;

    if( (timeMinute == resetTimeMinute) && (timeHour == resetTimeHour) )
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

}

void dailyReset( void )
{

    //    static bool resetComplete = false;

    //    if ((timeMinute == resetMinute) && (timeHour == resetHour))
    //    {
    //        if (resetComplete == false)
    //        {
    tba_energyUsedPreviousDay = tba_energyUsedLifetime - tba_energyUsedLastDayReset;
    tba_energyUsedLastDayReset = tba_energyUsedLifetime;
    emerAllocation = 0;

    EEwriteTotals( );
    EEwriteEnergyUsed( );

    // FIX NEEDS IMPLEMENTED
    // for right now just let the UI naturally update itself
    // expose functions from communications
    // commCommandSendStats();

    //    setRemoteStats( );
    // this is in the old SharedCommunications
    //    char charEnergyUsedLifetime[12];
    //    char charEnergyUsedPreviousDay[12];
    //
    //    ultoa( charEnergyUsedLifetime, tba_energyUsedLifetime, 10 );
    //    ultoa( charEnergyUsedPreviousDay, tba_energyUsedPreviousDay, 10 );
    //
    //    commandBuilder2( "Set", "Stat", charEnergyUsedLifetime, charEnergyUsedPreviousDay );


    // reset allocation to what is stored
    EEreadEnergyAlloc( );


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
void initPorts( void )
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

    LED1_DIR = 0;
    LED2_DIR = 0;
    LED3_DIR = 0;
    LED4_DIR = 0;

    return;
}

void enablePullDownResistors( void )
{

    _CN2PDE = 1; // pin 2 for emergency button

}

void enableInterrupts( void )
{

    INTCON1 |= 0b1000000000000000;
}

void disableInterrupts( void )
{

    INTCON1 &= 0b0111111111111111;
}

void readButton( void )
{
    static unsigned char buttonEmergencyComplete = 0;
    static unsigned char buttonTimeSecond = 255;

    if( emerButtonEnable )
    {
	if( BTN_EMER )
	{
	    if( buttonEmergencyComplete == 0 )
	    {
		if( buttonTimeSecond != timeSecond )
		{
		    tba_energyAllocation += emerButtonEnergyAllocate;
		    buttonEmergencyComplete = 1;
		    buttonTimeSecond = timeSecond;
		}
	    }
	}
	else
	{
	    if( buttonTimeSecond != timeSecond )
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

void storeToEE( void )
{
    static unsigned char eeComplete = 0;

    //TESTING
    //    if( (timeMinute == 0) || (timeMinute == 15) || (timeMinute == 30) || (timeMinute == 45) )
    if( (timeSecond == 0) )
    {
	if( eeComplete == 0 )
	{
	    EEwriteEnergyUsed( );
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

void setHighLow( )
{
    if( isHigh )
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

void initRTCCDisplay( void )
{

    //does unlock sequence to enable write to RTCC, sets RTCWEN
    __builtin_write_RTCWEN( );

    RCFGCAL = 0b0010001100000000;
    RTCPWC = 0b0000010100000000; // LPRC is clock source

    _RTCEN = 1;

    _RTCWREN = 0; // Disable Writing
}

void lightsMode( )
{
    static int status = 0;

    if( (LEDS_FOR_TESTING != true) && (lightsModeActive == true) )
    {
	switch( status )
	{
	case 0:
	    LED1_SET = 1;
	    LED2_SET = 0;
	    LED3_SET = 0;
	    LED4_SET = 0;
	    status++;
	    break;
	case 1:
	    LED1_SET = 0;
	    LED2_SET = 1;
	    LED3_SET = 0;
	    LED4_SET = 0;
	    status++;
	    break;
	case 2:
	    LED1_SET = 0;
	    LED2_SET = 0;
	    LED3_SET = 1;
	    LED4_SET = 0;
	    status++;
	    break;
	case 3:
	    LED1_SET = 0;
	    LED2_SET = 0;
	    LED3_SET = 0;
	    LED4_SET = 1;
	    status++;
	    break;
	case 4:
	    LED1_SET = 0;
	    LED2_SET = 0;
	    LED3_SET = 1;
	    LED4_SET = 0;
	    status++;
	    break;
	case 5:
	    LED1_SET = 0;
	    LED2_SET = 1;
	    LED3_SET = 0;
	    LED4_SET = 0;
	    status = 0;

	    break;
	default:
	    status = 0;
	    break;

	}
    }
}

void debugLEDSet( int LEDNum, bool On )
{
    int ledState;

    if( LEDS_FOR_TESTING == true )
    {

	if( On == true )
	{
	    ledState = 1;
	}
	else
	{
	    ledState = 0;
	}

	switch( LEDNum )
	{
	case 1:
	    LED1_SET = ledState;
	    break;

	case 2:
	    LED2_SET = ledState;
	    break;
	case 3:
	    LED3_SET = ledState;
	    break;
	case 4:
	    LED4_SET = ledState;
	    break;
	}
    }

    return;
}

void debugLEDRotateA( int minLED, int maxLED )
{
    static int pos = 0;

    if( pos == 0 )
    {
	pos = minLED;
    }

    pos++;
    if( pos > maxLED )
    {
	pos = minLED;
    }

    for( int inx = minLED; inx <= maxLED; inx++ )
    {
	debugLEDSet( inx, false );
    }
    debugLEDSet( pos, true );


}

void debugLEDRotateB( int minLED, int maxLED )
{
    static int pos = 0;

    if( pos == 0 )
    {
	pos = minLED;
    }

    pos++;
    if( pos > maxLED )
    {
	pos = minLED;
    }

    for( int inx = minLED; inx <= maxLED; inx++ )
    {
	debugLEDSet( inx, false );
    }
    debugLEDSet( pos, true );


}

void debugLEDToggle( int LEDNum )
{

    int set = 0;

    switch( LEDNum )
    {
    case 1:
	if( LED1_READ == 1 )
	{
	    set = 0;
	}
	else
	{
	    set = 1;
	}
	LED1_SET = set;
	break;
    case 2:
	if( LED2_READ == 1 )
	{
	    set = 0;
	}
	else
	{
	    set = 1;
	}
	LED2_SET = set;
	break;
    case 3:
	if( LED3_READ == 1 )
	{
	    set = 0;
	}
	else
	{
	    set = 1;
	}
	LED3_SET = set;
	break;
    case 4:
	if( LED4_READ == 1 )
	{
	    set = 0;
	}
	else
	{
	    set = 1;
	}
	LED4_SET = set;
	break;

    }
    return;

}

