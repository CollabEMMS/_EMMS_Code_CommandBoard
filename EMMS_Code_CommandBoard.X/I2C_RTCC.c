/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include "common.h"

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
unsigned char timeYear;
unsigned char timeMonth;
unsigned char timeDay;
unsigned char timeWeekday;
unsigned char timeHour;
unsigned char timeMinute;
unsigned char timeSecond;

unsigned char powerFailTimeMinute;
unsigned char powerFailTimeHour;
unsigned char powerFailTimeDay;
unsigned char powerFailTimeMonth;

unsigned char powerRestoreTimeMinute;
unsigned char powerRestoreTimeHour;
unsigned char powerRestoreTimeDay;
unsigned char powerRestoreTimeMonth;

// internal only

// not sorted yet

//char powerDownTime[12];
//char powerUpTime[12];





/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
     should be marked
 *****************/
void readTimeI2C( void );
void BeginSequentialReadI2C( char address );
char SequentialReadI2C( void );
void IdleI2C( void );
void StartI2C( void );
void StopI2C( void );
void RestartI2C( void );
void NackI2C( void );
void AckI2C( void );
void WriteI2C( unsigned char byte );
unsigned int ReadI2C( void );
char ReadI2CRegister( char address );
void readI2CTime( unsigned char* year, unsigned char* month, unsigned char* day,
		  unsigned char* hour, unsigned char* minute, unsigned char* second );



char BcdToDec( char val );
char DecToBcd( char val );
char BcdToDecI2C( char val );
char DecToBcdI2C( char val );

/****************
 CODE
 ****************/

void readTimeI2C( void )
{
    readI2CTime( &timeYear, &timeMonth, &timeDay, &timeHour, &timeMinute, &timeSecond );
}

// Read RTCC registers sequentially (fast)

void readI2CTime( unsigned char* year, unsigned char* month, unsigned char* day,
		  unsigned char* hour, unsigned char* minute, unsigned char* second )
{

    BeginSequentialReadI2C( RTCC_SECOND );

    char BCDSecond = SequentialReadI2C( ) & 0x7F; // Get Second
    char BCDMinute = SequentialReadI2C( ) & 0x7F; // Get Minute
    char BCDHour = SequentialReadI2C( ) & 0x3F; // Get Hour
    SequentialReadI2C( ); // Discard Weekday
    char BCDDay = SequentialReadI2C( ) & 0x3F; // Get Day
    char BCDMonth = SequentialReadI2C( ) & 0x1F; // Get Month
    char BCDYear = SequentialReadI2C( ); // Get Year

    StopI2C( );

    *year = BcdToDecI2C( BCDYear );
    *month = BcdToDecI2C( BCDMonth );
    *day = BcdToDecI2C( BCDDay );
    *hour = BcdToDecI2C( BCDHour );
    *minute = BcdToDecI2C( BCDMinute );
    *second = BcdToDecI2C( BCDSecond );

}

// Set time in RTCC registers

void setI2CTime( unsigned char year, unsigned char month, unsigned char day,
		 unsigned char hour, unsigned char minute, unsigned char second )
{

    char BCDSecond = DecToBcdI2C( second ),
	    BCDMinute = DecToBcdI2C( minute ),
	    BCDHour = DecToBcdI2C( hour ) & 0x3F, // 24-hour time
	    BCDDay = DecToBcdI2C( day ),
	    BCDMonth = DecToBcdI2C( month ),
	    BCDYear = DecToBcdI2C( year );

    if( year % 4 )
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

}

// Start RTCC in case it was stopped (dead or missing battery)

void startClock( void )
{
    // Preserve seconds value in case RTCC was already running
    char BCDSecond = ReadI2CRegister( RTCC_SECOND ) & 0x7F;

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
    WriteI2C( 0x08 ); // Enable battery operation and set weekday to zero
    IdleI2C( );
    StopI2C( );

    // Enable Oscillator
    StartI2C( );
    WriteI2C( RTCC_WRITE ); // Device Address (RTCC) + Write Command
    WriteI2C( RTCC_SECOND ); // Set address to seconds
    WriteI2C( 0x80 | BCDSecond ); // Enable oscillator and restore seconds
    IdleI2C( );
    StopI2C( );
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
    while( I2C1STATbits.ACKSTAT ); // Wait for ACK from slave
}

// Read next register

char SequentialReadI2C( void )
{
    while( I2C1STATbits.TRSTAT ); // Wait for bus to be idle
    I2C1CONbits.RCEN = 1; // Gives control of clock to Slave device
    while( !I2C1STATbits.RBF ); // Waits for register to fill up
    AckI2C( ); // Ask for next byte
    return I2C1RCV; // Returns data
}

// Read one byte from given memory address

char ReadI2CRegister( char address )
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

void readI2CPowerTimes( void )
{

    // the MCP7904 time chip automatically stores the times
    // the power is lost and then restored
    // this retrieves both

    // powerDown is the time the power was lost
    // powerUp is the time the power was restored

    BeginSequentialReadI2C( RTCC_PWRDNMIN );

    powerFailTimeMinute = SequentialReadI2C( ) & 0x7F;
    powerFailTimeHour = SequentialReadI2C( ) & 0x3F;
    powerFailTimeDay = SequentialReadI2C( ) & 0x3F;
    powerFailTimeMonth = SequentialReadI2C( ) & 0x1F;

    powerRestoreTimeMinute = SequentialReadI2C( ) & 0x7F;
    powerRestoreTimeHour = SequentialReadI2C( ) & 0x3F;
    powerRestoreTimeDay = SequentialReadI2C( ) & 0x3F;
    powerRestoreTimeMonth = SequentialReadI2C( ) & 0x1F;

    StopI2C( );


    // FIX DONE
    // these functions hide in SharedCommunications
    // they do not have bounds checking
    //    stringCopy("HH:MM:DD:MM", powerDownTime);
    //    stringCopy("HH:MM:DD:MM", powerUpTime);
    // are they even needed?
    // I don't think they are needed or there is another way to populate the variables
    // look below

    //    powerDownTime[0] = 'H';
    //    powerDownTime[1] = 'H';
    //    powerDownTime[2] = ':';
    //    powerDownTime[3] = 'M';
    //    powerDownTime[4] = 'M';
    //    powerDownTime[5] = ':';
    //    powerDownTime[6] = 'D';
    //    powerDownTime[7] = 'D';
    //    powerDownTime[8] = ':';
    //    powerDownTime[9] = 'M';
    //    powerDownTime[10] = 'M';
    //    powerDownTime[11] = CHAR_NULL;
    //
    //    powerUpTime[0] = 'H';
    //    powerUpTime[1] = 'H';
    //    powerUpTime[2] = ':';
    //    powerUpTime[3] = 'M';
    //    powerUpTime[4] = 'M';
    //    powerUpTime[5] = ':';
    //    powerUpTime[6] = 'D';
    //    powerUpTime[7] = 'D';
    //    powerUpTime[8] = ':';
    //    powerUpTime[9] = 'M';
    //    powerUpTime[10] = 'M';
    //    powerUpTime[11] = CHAR_NULL;
    //
    //
    //    powerDownTime[ 0] = (powerDownHour >> 4) + 0x30;
    //    powerDownTime[ 1] = (powerDownHour) + 0x30;
    //    powerDownTime[ 3] = (powerDownMinute >> 4) + 0x30;
    //    powerDownTime[ 4] = (powerDownMinute) + 0x30;
    //    powerDownTime[ 6] = (powerDownDay >> 4) + 0x30;
    //    powerDownTime[ 7] = (powerDownDay) + 0x30;
    //    powerDownTime[ 9] = (powerDownMonth >> 4) + 0x30;
    //    powerDownTime[10] = (powerDownMonth) + 0x30;
    //    powerDownTime[11] = CHAR_NULL;
    //
    //    powerUpTime [ 0] = (powerUpHour >> 4) + 0x30;
    //    powerUpTime [ 1] = (powerUpHour) + 0x30;
    //    powerUpTime [ 3] = (powerUpMinute >> 4) + 0x30;
    //    powerUpTime [ 4] = (powerUpMinute) + 0x30;
    //    powerUpTime [ 6] = (powerUpDay >> 4) + 0x30;
    //    powerUpTime [ 7] = (powerUpDay) + 0x30;
    //    powerUpTime [ 9] = (powerUpMonth >> 4) + 0x30;
    //    powerUpTime [10] = (powerUpMonth) + 0x30;
    //    powerUpTime [11] = CHAR_NULL;
}

// Wait for bus to be idle

void IdleI2C( void )
{
    while( I2C1STATbits.TRSTAT );
}

// Generate an I2C start condition

void StartI2C( void )
{
    I2C1CONbits.SEN = 1; // Generate start condition
    while( I2C1CONbits.SEN ); // Wait for start
    //return(I2C1STATbits.S);   // Optional - return status
}

// Generate an I2C stop condition

void StopI2C( void )
{
    I2C1CONbits.PEN = 1; // Generate stop condition
    while( I2C1CONbits.PEN ); // Wait for stop
    //return(I2C1STATbits.P);   // Optional - return status
}

// Generate an I2C restart condition

void RestartI2C( void )
{
    I2C1CONbits.RSEN = 1; // Generate restart
    while( I2C1CONbits.RSEN ); // Wait for restart
    //return(I2C1STATbits.S);   // Optional - return status
}

// Generate an I2C not acknowledge condition

void NackI2C( void )
{
    I2C1CONbits.ACKDT = 1;
    I2C1CONbits.ACKEN = 1;
    while( I2C1CONbits.ACKEN );
}

// Generate an I2C acknowledge condition

void AckI2C( void )
{
    I2C1CONbits.ACKDT = 0;
    I2C1CONbits.ACKEN = 1;
    while( I2C1CONbits.ACKEN );
}

// Set up I2C module

void initI2C( void )
{
    I2C1CONbits.A10M = 0; // Use 7-bit slave addresses
    I2C1CONbits.DISSLW = 1; // Disable Slew rate control
    I2C1CONbits.IPMIEN = 0; // should be set to 0 when master
    //IFS1bits.MI2C1IF = 0; // Disable Interupt

    I2C1BRG = 0x4E; // If Fcy = 8 Mhz this will set the baud to 100 khz
    I2C1CONbits.I2CEN = 1; // Configures I2C pins as I2C (on pins 17 an 18)
}

// Transmit one byte

void WriteI2C( unsigned char byte )
{
    while( I2C1STATbits.TRSTAT ); // Wait for bus to be idle
    I2C1TRN = byte; // Load byte to I2C1 Transmit buffer
    while( I2C1STATbits.TBF ); // Wait for data transmission
}

// Read one byte from bus

unsigned int ReadI2C( void )
{
    while( I2C1STATbits.TRSTAT ); // Wait for bus to be idle
    I2C1CONbits.ACKDT = 1; // Prepares to send NACK
    I2C1CONbits.RCEN = 1; // Gives control of clock to Slave device
    while( !I2C1STATbits.RBF ); // Waits for register to fill up
    I2C1CONbits.ACKEN = 1; // Sends NACK or ACK set above
    while( I2C1CONbits.ACKEN ); // Waits till ACK is sent (hardware reset)
    return I2C1RCV; // Returns data
}

char BcdToDecI2C( char val )
{
    return (val / 16 * 10) + (val % 16);
}

char DecToBcdI2C( char val )
{
    return (val / 10 * 16) + (val % 10);
}