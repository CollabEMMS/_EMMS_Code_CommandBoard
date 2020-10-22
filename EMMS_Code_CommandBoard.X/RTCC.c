/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include <stdlib.h>

#include "common.h"
#include "PowerMain.h"
#include "EEPROM.h"

#include "LEDControl.h"

// this is unusual to include a module's own header file
// it is done to include the struct definition
//#include "RTCCExternal.h"


/****************
 MACROS
 ****************/
#define RTCC_WRITE     0xDE
#define RTCC_READ      0xDF
#define RTCC_SECOND    0x00
#define RTCC_MINUTE    0x01
#define RTCC_HOUR      0x02
#define RTCC_WEEKDAY   0x03
#define RTCC_DAY       0x04
#define RTCC_MONTH     0x05
#define RTCC_YEAR      0x06
#define RTCC_PWRDNMIN  0x18
#define RTCC_PWRDNHOUR 0x19
#define RTCC_PWRDNDATE 0x1A
#define RTCC_PWRDNMTH  0x1B
#define RTCC_PWRUPMIN  0x1C
#define RTCC_PWRUPHOUR 0x1D
#define RTCC_PWRUPDATE 0x1E
#define RTCC_PWRUPMTH  0x1F

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
 *****************/

// RTCC Internal
bool rtccSetTime( struct date_time *setDateTime );

char BcdToDec( unsigned int val );
unsigned int DecToBcd( char val );

void rtccReadTime( struct date_time *readDateTime );

void rtccInternalInit( void );

// RTCC I2C
void rtccI2CInit( void );
void initI2Crtcc( void );
void startI2CClock( void );
void setI2CClockBuildTime( void );

void rtccI2CReadPowerTimes( struct date_time *timePowerFail, struct date_time *timePowerRestore );
void rtccI2CReadTime( struct date_time *readDateTime );
void rtccI2CSetTime( struct date_time *setDateTime );

void BeginSequentialReadI2C( char address );
char SequentialReadI2C( void );
char ReadI2CRegister( char address );

void IdleI2C( void );
void StartI2C( void ); //   i
void StopI2C( void );
void RestartI2C( void );
void NackI2C( void );
void AckI2C( void );
void WriteI2C( unsigned char byte );
unsigned int ReadI2C( void );
char BcdToDecI2C( char val );
char DecToBcdI2C( char val );



/****************
 CODE
 ****************/

void rtccInit( void )
{
    rtccInternalInit( );
    rtccI2CInit( );

}

void rtccCopyI2CTime( void )
{
    struct date_time currentDateTime;

    rtccI2CReadTime( &currentDateTime );
    rtccSetTime( &currentDateTime );

    rtccReadTime( &dateTime_global );

    return;
}




//***************
//  RTCC Internal
//***************

void rtccInternalInit( void )
{

    //does unlock sequence to enable write to RTCC, sets RTCWEN
    __builtin_write_RTCWEN( );

    //    RCFGCAL = 0b0010001100000000;
    RCFGCALbits.RTCEN = 0;
    RCFGCALbits.RTCWREN = 1;
    RCFGCALbits.RTCSYNC = 0;
    RCFGCALbits.HALFSEC = 0;
    RCFGCALbits.RTCOE = 0;
    RCFGCALbits.RTCPTR1 = 1;
    RCFGCALbits.RTCPTR0 = 1;
    RCFGCALbits.CAL = 0b00000000;

    //RTCPWC = 0b0000010100000000; // LPRC is clock source
    RTCPWCbits.PWCEN = 0;
    RTCPWCbits.PWCPOL = 0;
    RTCPWCbits.PWCCPRE = 0;
    RTCPWCbits.PWCSPRE = 0;
    RTCPWCbits.RTCCLK = 0b01;
    RTCPWCbits.RTCOUT = 0b01;

    RCFGCALbits.RTCEN = 1;
    RCFGCALbits.RTCWREN = 0; // Disable Writing

    return;
}

void rtccReadTime( struct date_time *readDateTime )
{

    unsigned int temp1;
    unsigned int temp2;

    _RTCPTR = 0b11;

    temp1 = RTCVAL;
    readDateTime->year = BcdToDec( temp1 );

    temp1 = RTCVAL;
    temp2 = temp1 >> 8;
    readDateTime->month = BcdToDec( temp2 );
    temp2 = temp1 & 0x00FF;
    readDateTime->day = BcdToDec( temp2 );

    temp1 = RTCVAL;
    temp2 = temp1 & 0x00FF;
    readDateTime->hour = BcdToDec( temp2 );

    temp1 = RTCVAL;
    temp2 = temp1 >> 8;
    readDateTime->minute = BcdToDec( temp2 );
    temp2 = temp1 & 0x00FF;
    readDateTime->second = BcdToDec( temp2 );

    return;
}

bool rtccSetTime( struct date_time *setDateTime )
{
    bool validDateTime = true;

    unsigned int tempYear;
    unsigned int tempMonth;
    unsigned int tempDay;
    unsigned int tempHour;
    unsigned int tempMinute;
    unsigned int tempSecond;

    unsigned int tempMonthDay;
    unsigned int tempMinuteSecond;

    // this just makes sure the day of month is valid
    switch( setDateTime->month )
    {
	case 4:
	case 6:
	case 9:
	case 11:
	    if( setDateTime->day > 30 )
		return -1;
	    break;

	case 2:
	    // check leap year - ignore century skips (year 2000))
	    if( setDateTime->year % 4 )
	    {
		if( setDateTime->day > 28 )
		    validDateTime = false;
	    }
	    else
	    {
		if( setDateTime->day > 29 )
		    validDateTime = false;
	    }
	    break;
    }

    // put values into BCD
    tempYear = DecToBcd( setDateTime->year );
    tempMonth = DecToBcd( setDateTime->month );
    tempDay = DecToBcd( setDateTime->day );
    tempHour = DecToBcd( setDateTime->hour );
    tempMinute = DecToBcd( setDateTime->minute );
    tempSecond = DecToBcd( setDateTime->second );

    tempMonthDay = (tempMonth << 8) + tempDay;
    tempMinuteSecond = (tempMinute << 8) + tempSecond;


    _RTCEN = 0; // disable clock

    //does unlock sequence to enable write to RTCC, sets RTCWEN
    __builtin_write_RTCWEN( );

    _RTCPTR = 0b11; // decrements with read or write
    RTCVAL = tempYear; // set year
    RTCVAL = tempMonthDay; // set month, day
    RTCVAL = tempHour; // clear weekday, set hour
    RTCVAL = tempMinuteSecond; // set minute, second

    _RTCEN = 1; // enable clock

    _RTCWREN = 0; // Disable Writing

    return validDateTime;
}

char BcdToDec( unsigned int bcdNum )
{

    char decNum;
    decNum = (bcdNum / 16 * 10) + (bcdNum % 16);

    return decNum;
}

unsigned int DecToBcd( char decNum )
{
    unsigned int bcdNum;
    bcdNum = (decNum / 10 * 16) + (decNum % 10);

    return bcdNum;
}




//***************
//  RTCC I2C
//***************

void rtccI2CInit( void )
{
    initI2Crtcc( );
    startI2CClock( );

    setI2CClockBuildTime( );

    return;
}

void initI2Crtcc( void )
{

    TRISBbits.TRISB8 = 1; // I2C clock
    TRISBbits.TRISB9 = 1; // I2C data
    TRISAbits.TRISA7 = 1; // IC1 from MCP3909

    // TODO verify if the following I/O is actually used
    // B15 looks like it is used for SPI - verify circuit and remove this line if appropriate
    TRISBbits.TRISB15 = 0; // MCP3909 !MCLR  (this doesn't seem to be used))

    I2C1CONbits.A10M = 0; // Use 7-bit slave addresses
    I2C1CONbits.DISSLW = 1; // Disable Slew rate control
    I2C1CONbits.IPMIEN = 0; // should be set to 0 when master
    //IFS1bits.MI2C1IF = 0; // Disable Interrupt

    //BRG = ((FCY/FSCL)-(FCY/10,000,000)) -1
    // FCY - our program frequency
    // FSCL - desired I2C baud rate (100k)
    I2C1BRG = ((FCY / 100000UL) - (FCY / 10000000UL)) - 1;
    I2C1CONbits.I2CEN = 1; // Configures I2C pins as I2C (on pins 17 an 18)

    return;
}

// Start RTCC in case it was stopped (dead or missing battery)

void startI2CClock( void )
{
    // Preserve seconds value in case RTCC was already running
    char BCDSecond = ReadI2CRegister( RTCC_SECOND ) & 0x7F; // ZACH 12/2/2019 LAUNCHING PAD REQRITE THIS FUNCTION FOR PWRFAIL TESTING
    
    // here yee here yee we get the powerfail times 12/3/2019
    struct date_time timePowerFail;
	struct date_time timePowerRestore;

	rtccI2CReadPowerTimes( &timePowerFail, &timePowerRestore );
    // end here yee here yee
    
    
    // Disable Oscillator
    StartI2C( );
    WriteI2C( RTCC_WRITE ); // Device Address (RTCC) + Write Command
    WriteI2C( RTCC_SECOND ); // Set address to seconds
    WriteI2C( 0x00 ); // Disable oscillator and set seconds to 0
    IdleI2C( );
    StopI2C( );

    // Set Time
    StartI2C( );
    WriteI2C( RTCC_WRITE ); // Device Address (RTCC) + Write Command
    WriteI2C( RTCC_WEEKDAY ); // Set address to weekday
//    WriteI2C( 0x08 ); // Enable battery operation and set weekday to zero
    WriteI2C(0x00); // Clear flag
    IdleI2C( );
    StopI2C( );

    // Enable Oscillator
    StartI2C( );
    WriteI2C( RTCC_WRITE ); // Device Address (RTCC) + Write Command
    WriteI2C( RTCC_SECOND ); // Set address to seconds
    WriteI2C( 0x80 | BCDSecond ); // Enable oscillator and restore seconds
    IdleI2C( );
    StopI2C( );

    return;
}

void setI2CClockBuildTime( void )
{
    // when programming set the date/time to the build date and time of the program
    // Build date: __DATE__ -> "Mar  2 2015"
    //                          01234567890
    // Build time: __TIME__ -> "14:05:00"

    // if the build time is later than the current clock time
    // set the clock time to the build time
    // this helps to keep the computer from setting the clock if we are in an earlier timezone than the computer is configure for


    //get the year
    // last 4 digits of the __DATE__
    //    #define BUILD_YEAR  (((__DATE__ [9] - 48) * 10) + (__DATE__ [10] - 48))

    char tempInt[5]; // for pulling out text numbers to convert to an integer

    struct date_time buildDateTime;

    int tempIntValue;
    // __DATE__  and  __TIME__  contain the date the code was compiled

    // __DATE__ is a compiler macro which contains the date the program was compiled (built))
    // __DATE__ -> "Mar  2 2015"
    //              01234567890
    // convert into 3 integer variables for month, day, year

    // get the month
    // month abbreviations used by __DATE__
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    // check each character until we know what it is

    switch( __DATE__ [0] )
    {
	case 'J': //Jan Jun Jul
	    switch( __DATE__ [1] )
	    {
		case 'a': //Jan
		    tempIntValue = 1;
		    break;
		case 'u': //Jun Jul
		    switch( __DATE__ [2] )
		    {
			case 'n': // Jun
			    tempIntValue = 6;
			    break;
			case 'l': // Jul
			    tempIntValue = 7;
			    break;
		    }
		    break;

	    }
	    break;
	case 'F': //Feb
	    tempIntValue = 12;
	    break;
	case 'M': //Mar May
	    switch( __DATE__ [2] ) // skip checking the 2nd character [1] since they are the same anyway
	    {
		case 'r': // Mar
		    tempIntValue = 3;
		    break;
		case 'y': // May
		    tempIntValue = 5;
		    break;
	    }
	    break;
	case 'A': //Apr Aug
	    switch( __DATE__ [1] )
	    {
		case 'p': // Apr
		    tempIntValue = 4;
		    break;
		case 'u': // Aug
		    tempIntValue = 8;
		    break;
	    }
	    break;
	case 'S': //Sep
	    tempIntValue = 9;
	    break;
	case 'O': //Oct
	    tempIntValue = 10;
	    break;
	case 'N': //Nov
	    tempIntValue = 11;
	    break;
	case 'D': //Dec
	    tempIntValue = 12;
	    break;
    }

    buildDateTime.month = tempIntValue;


    // __DATE__ -> "Mar  2 2015"
    //              01234567890
    // get the day
    if( __DATE__ [4] == ' ' )
    {
	tempInt[0] = __DATE__ [4];
	tempInt[1] = CHAR_NULL;
    }
    else
    {
	tempInt[0] = __DATE__ [4];
	tempInt[1] = __DATE__ [5];
	tempInt[2] = CHAR_NULL;
    }

    tempIntValue = atoi( tempInt );
    buildDateTime.day = tempIntValue;


    // __DATE__ -> "Mar  2 2015"
    //              01234567890
    // get the year

    tempInt[0] = __DATE__ [7];
    tempInt[1] = __DATE__ [8];
    tempInt[2] = __DATE__ [9];
    tempInt[3] = __DATE__ [10];
    tempInt[4] = CHAR_NULL;

    tempIntValue = atoi( tempInt );
    buildDateTime.year = tempIntValue;



    // Build time: __TIME__ -> "14:05:00"
    //                          01234567
    // get hour
    tempInt[0] = __TIME__ [0];
    tempInt[1] = __TIME__ [1];
    tempInt[2] = CHAR_NULL;

    tempIntValue = atoi( tempInt );
    buildDateTime.hour = tempIntValue;


    // Build time: __TIME__ -> "14:05:00"
    //                          01234567
    // get minute
    tempInt[0] = __TIME__ [3];
    tempInt[1] = __TIME__ [4];
    tempInt[2] = CHAR_NULL;

    tempIntValue = atoi( tempInt );
    buildDateTime.minute = tempIntValue;


    // Build time: __TIME__ -> "14:05:00"
    //                          01234567
    // get second
    tempInt[0] = __TIME__ [6];
    tempInt[1] = __TIME__ [7];
    tempInt[2] = CHAR_NULL;

    tempIntValue = atoi( tempInt );
    buildDateTime.second = tempIntValue;

    // load the current time
    // this used to be used to compare the build time against the current time
    // it is left here in case we want to do something with it
    struct date_time currentDateTime;
    rtccI2CReadTime( &currentDateTime );


    // only used the build time to set the current time once after a compile and reprogram
    // the EEPROM is set to 0 on programming
    // then it is changed to non-zero after the clock is set using the build time

    bool rtccBuildIsSet;
    rtccBuildIsSet = eeReadRTCCIsSet( );

    if( rtccBuildIsSet != true )
    {

	// getting here means that we need to set the rtcc to the build time
	rtccI2CSetTime( &buildDateTime );
	eeWriteRTCCIsSet( true );
    }

    return;
}

void rtccI2CReadTime( struct date_time *readDateTime )
{

    // the date & time comes in as binary coded decimal (BCD)
    // once read it needs converted to plain decimal when stored to a char variable

    BeginSequentialReadI2C( RTCC_SECOND );

    char BCDSecond;
    char BCDMinute;
    char BCDHour;
    char BCDDay;
    char BCDMonth;
    char BCDYear;

    BCDSecond = SequentialReadI2C( ) & 0x7F; // Get Second
    BCDMinute = SequentialReadI2C( ) & 0x7F; // Get Minute
    BCDHour = SequentialReadI2C( ) & 0x3F; // Get Hour
    SequentialReadI2C( ); // Discard Weekday
    BCDDay = SequentialReadI2C( ) & 0x3F; // Get Day
    BCDMonth = SequentialReadI2C( ) & 0x1F; // Get Month
    BCDYear = SequentialReadI2C( ); // Get Year

    StopI2C( );

    readDateTime->year = BcdToDecI2C( BCDYear );
    readDateTime->month = BcdToDecI2C( BCDMonth );
    readDateTime->day = BcdToDecI2C( BCDDay );
    readDateTime->hour = BcdToDecI2C( BCDHour );
    readDateTime->minute = BcdToDecI2C( BCDMinute );
    readDateTime->second = BcdToDecI2C( BCDSecond );

    return;
}

// Set time in RTCC registers

void rtccI2CSetTime( struct date_time *setDateTime )
{

    char BCDSecond = DecToBcdI2C( setDateTime->second ),
	    BCDMinute = DecToBcdI2C( setDateTime->minute ),
	    BCDHour = DecToBcdI2C( setDateTime->hour ) & 0x3F, // 24-hour time
	    BCDDay = DecToBcdI2C( setDateTime->day ),
	    BCDMonth = DecToBcdI2C( setDateTime->month ),
	    BCDYear = DecToBcdI2C( setDateTime->year );

    if( setDateTime->year % 4 )
	BCDMonth &= 0x1F; // Is not leap year
    else
	BCDMonth |= 0x20; // Is leap year

    // Disable Oscillator
    StartI2C( );
    WriteI2C( RTCC_WRITE ); // Device Address (RTCC) + Write Command
    WriteI2C( RTCC_SECOND ); // Set address to seconds
    WriteI2C( 0x00 ); // Disable oscillator and set seconds to 0
    IdleI2C( );
    StopI2C( );

    // Set Time
    StartI2C( );
    WriteI2C( RTCC_WRITE ); // Device Address (RTCC) + Write Command
    WriteI2C( RTCC_MINUTE ); // Set address to minutes
    WriteI2C( BCDMinute ); // Send Minute
    WriteI2C( BCDHour ); // Send Hour
    WriteI2C( 0x08 ); // Enable battery operation and set weekday to zero
    WriteI2C( BCDDay ); // Send Date
    WriteI2C( BCDMonth ); // Send Month
    WriteI2C( BCDYear ); // Send Year
    IdleI2C( );
    StopI2C( );

    // Enable Oscillator
    StartI2C( );
    WriteI2C( RTCC_WRITE ); // Device Address (RTCC) + Write Command
    WriteI2C( RTCC_SECOND ); // Set address to seconds
    WriteI2C( 0x80 | BCDSecond ); // Enable oscillator and set seconds
    IdleI2C( );
    StopI2C( );

    return;
}


// Set up sequential read from MCP7940, starting at given address

void BeginSequentialReadI2C( char address )
{
    StartI2C( );
    WriteI2C( RTCC_WRITE ); // Enter Write Mode
    WriteI2C( address ); // Beginning address for reading
    IdleI2C( );
    RestartI2C( );
    WriteI2C( RTCC_READ ); // Enter Read Mode
    while( I2C1STATbits.ACKSTAT ) // Wait for ACK from slave
    {
    }

    return;
}

// Read next register

char SequentialReadI2C( void )
{
    while( I2C1STATbits.TRSTAT ) // Wait for bus to be idle
    {
    }

    I2C1CONbits.RCEN = 1; // Gives control of clock to Slave device

    while( !I2C1STATbits.RBF ) // Waits for register to fill up
    {
    }

    AckI2C( ); // Ask for next byte

    return I2C1RCV; // Returns data
}

// Read one byte from given memory address

char ReadI2CRegister( char address ) // ZACH TRY USING THiS METHOD TO READ THINGS
{
    StartI2C( );
    WriteI2C( RTCC_WRITE ); // Enter Write Mode
    WriteI2C( address ); // Set address for reading
    IdleI2C( );
    RestartI2C( );
    WriteI2C( RTCC_READ ); // Enter Read Mode
    char returnValue = ReadI2C( );
    StopI2C( );

    return returnValue;
}

// Read PowerDown and PowerUp time from RTCC into global strings

void rtccI2CReadPowerTimes( struct date_time *timePowerFail, struct date_time *timePowerRestore )
{
    // the MCP7704 time chip automatically stores the following:
    //  	time the power failed
    //		time the power was restored
    // this retrieves both

    // the time chip power fail/restore registers are cleared at the end of this function

    // TODO clear the PWRFAIL bit in the clock to allow new power fail time to be stored
    // the register must be cleared for a new power fail/restore time to be stored
    // this could be why we do not see the allocation check on power on working right

    // to prevent use of global variables we will store the time stamps retrieved
    // from the registers in static variables
    // the first time this function is called we retrieve the data from the clock chip
    //	    this will always be the case after a power fail/restore because the PIC restarts
    //  then the clock chip registers are cleared
    // the second and later times this function is called we simply return what is in the static variables

    static bool firstRun = true;
    
    static char PWDN;
    static char PWDNHOUR;
    // TODO impliment AM/PM and Military timekeeping
    static char powerFailAMPM; // 0 = PM 1 = AM
    static char powerFailIsMilitary; // 0 = 12 Hour Format 1 = 24 Hour format
    
    static char powerFailTimeMinuteTens;
    static char powerFailTimeMinute;
    static char powerFailTimeHour;
    static char powerFailTimeHourTens;
    static char powerFailTimeDay;
    static char powerFailTimeDayTens;
    
    static char powerFailTimeMonth;
    static char powerFailTimeMonthTens;

    static char powerRestoreTimeMinute;
    static char powerRestoreTimeHour;
    static char powerRestoreTimeDay;
    static char powerRestoreTimeMonth;


    if( firstRun == true )
    {
	firstRun = false;
	// TODO RTCC I2C Read - document better how this works
    PWDN = ReadI2CRegister( RTCC_PWRDNMIN ) &               0b01111111;
    PWDNHOUR = ReadI2CRegister ( RTCC_PWRDNHOUR ) &         0b01111111;
    powerFailTimeMonth = ReadI2CRegister( RTCC_PWRDNMTH ) & 0b00011111;
    powerFailTimeDay = ReadI2CRegister( RTCC_PWRDNDATE ) &  0b00111111;
    
    // Get month tens first before overwrite
    powerFailTimeMonthTens = powerFailTimeMonth >> 4;
    powerFailTimeMonthTens = powerFailTimeMonthTens + 48; // dont need bcd because its either 0 or 1

    powerFailTimeMonth = powerFailTimeMonth & 0b00001111; // removes tens place
    powerFailTimeMonth = BcdToDecI2C(powerFailTimeMonth) + 48; // to ASCII
    
    // Get month tens first before overwrite
    powerFailTimeDayTens = powerFailTimeDay >> 4;
    powerFailTimeDayTens = BcdToDecI2C(powerFailTimeDayTens) + 48; // dont need bcd because its either 0 or 1
    
    // Get date tens first before overwrite
    powerFailTimeDay = powerFailTimeDay & 0b00001111; // Rmoves tens place
    powerFailTimeDay = BcdToDecI2C(powerFailTimeDay) + 48; // To ASCII
    
    // Grabs tens place from Minute Register
    powerFailTimeMinuteTens = PWDN &  0b01110000;
    powerFailTimeMinuteTens = powerFailTimeMinuteTens >> 4; // Shift to the right 4 times to get tens
    powerFailTimeMinuteTens = BcdToDecI2C(powerFailTimeMinuteTens) + 48; // goes from binary number to ASCII number
    
    // Grabs ones place from Minute Register
    powerFailTimeMinute = PWDN &  0b00001111;
    powerFailTimeMinute = BcdToDecI2C(powerFailTimeMinute) + 48; // goes from binary number to ASCII number
    
    // Grabs tens place from Hour Register
    powerFailTimeHourTens = PWDN &  0b00110000;
    powerFailTimeHourTens = powerFailTimeHourTens >> 4; // Shift 4 to calculate properly
//    powerFailTimeHourTens = powerFailTimeHourTens + 48; // goes from binary number to ASCII number
    
    // Grabs ones place from Hour Register
    powerFailTimeHour = PWDN &  0b00001111;
    powerFailTimeHour = 0 + 48; // goes from binary number to ASCII number
    
    
    //
    
    /////////
//	BeginSequentialReadI2C( RTCC_PWRDNMIN );
//
//	powerFailTimeMinute = SequentialReadI2C( ) & 0x7F;
//	powerFailTimeHour = SequentialReadI2C( ) & 0x3F;
//	powerFailTimeDay = SequentialReadI2C( ) & 0x3F;
//	powerFailTimeMonth = SequentialReadI2C( ) & 0x1F;
//
//	powerRestoreTimeMinute = SequentialReadI2C( ) & 0x7F;
//	powerRestoreTimeHour = SequentialReadI2C( ) & 0x3F;
//	powerRestoreTimeDay = SequentialReadI2C( ) & 0x3F;
//	powerRestoreTimeMonth = SequentialReadI2C( ) & 0x1F;
    /////////
    
    // VARIABLES SET IN HERE POPULATE DOWN TO DISPLAY
	StopI2C( );

	// TODO clear the PWRFAIL bit in the clock to allow new power fail time to be stored

    }
    ////Confirmed to work here, variables put into struct display properly, problem is above.
//    timePowerFail->minuteTens = BcdToDecI2C(powerFailTimeMinuteTens);
    timePowerFail->minute = powerFailTimeMinute;
    timePowerFail->minuteTens = powerFailTimeMinuteTens;
    timePowerFail->hour = powerFailTimeHour;
    timePowerFail->hourTens = powerFailTimeHourTens;
    timePowerFail->day = powerFailTimeDay;
    timePowerFail->dayTens = powerFailTimeDayTens;
    timePowerFail->month = powerFailTimeMonth;
    timePowerFail->monthTens = powerFailTimeMonthTens;
    timePowerFail->second = 0;
    timePowerFail->year = 0;

    timePowerRestore->minute = BcdToDecI2C(powerRestoreTimeMinute);
    timePowerRestore->hour = BcdToDecI2C(powerRestoreTimeHour);
    timePowerRestore->day = BcdToDecI2C(powerRestoreTimeDay);
    timePowerRestore->month = BcdToDecI2C(powerRestoreTimeMonth);
    timePowerRestore->second = 0;

    // check if we changed years - at least note it in the year
    if( powerRestoreTimeMonth < powerFailTimeMonth )
    {
	timePowerRestore->year = 1;
    }
    else
    {
	timePowerRestore->year = 0;
    }

    timePowerRestore->year = 0;
    return;
}


// Wait for bus to be idle

void IdleI2C( void )
{
    while( I2C1STATbits.TRSTAT )
    {
    }

    return;
}

// Generate an I2C start condition

void StartI2C( void )
{
    I2C1CONbits.SEN = 1; // Generate start condition
    while( I2C1CONbits.SEN ) // Wait for start
    {
    }

    //return(I2C1STATbits.S);   // Optional - return status
    return;
}

// Generate an I2C stop condition

void StopI2C( void )
{
    I2C1CONbits.PEN = 1; // Generate stop condition
    while( I2C1CONbits.PEN ) // Wait for stop
    {
    }

    //return(I2C1STATbits.P);   // Optional - return status
    return;
}

// Generate an I2C restart condition

void RestartI2C( void )
{
    I2C1CONbits.RSEN = 1; // Generate restart
    while( I2C1CONbits.RSEN ) // Wait for restart
    {
    }

    //return(I2C1STATbits.S);   // Optional - return status
    return;
}

// Generate an I2C not acknowledge condition

void NackI2C( void )
{
    I2C1CONbits.ACKDT = 1;
    I2C1CONbits.ACKEN = 1;
    while( I2C1CONbits.ACKEN )
    {
    }

    return;
}

// Generate an I2C acknowledge condition

void AckI2C( void )
{
    I2C1CONbits.ACKDT = 0;
    I2C1CONbits.ACKEN = 1;
    while( I2C1CONbits.ACKEN )
    {
    }

    return;
}

// **********
// Set up I2C module

// Transmit one byte

void WriteI2C( unsigned char byte )
{
    while( I2C1STATbits.TRSTAT ) // Wait for bus to be idle
    {
    }

    I2C1TRN = byte; // Load byte to I2C1 Transmit buffer
    while( I2C1STATbits.TBF ) // Wait for data transmission
    {
    }

    return;
}

// Read one byte from bus

unsigned int ReadI2C( void )
{

    while( I2C1STATbits.TRSTAT ) // Wait for bus to be idle
    {
    }

    I2C1CONbits.ACKDT = 1; // Prepares to send NACK
    I2C1CONbits.RCEN = 1; // Gives control of clock to Slave device
    while( !I2C1STATbits.RBF ) // Waits for register to fill up
    {
    }

    I2C1CONbits.ACKEN = 1; // Sends NACK or ACK set above
    while( I2C1CONbits.ACKEN ) // Waits till ACK is sent (hardware reset)
    {
    }

    return I2C1RCV; // Returns data
}

char BcdToDecI2C( char val )
{
    char dec;

    dec = (val / 16 * 10) + (val % 16);

    return dec;
}

char DecToBcdI2C( char val )
{
    char bcd;

    bcd = (val / 10 * 16) + (val % 10);

    return bcd;
}