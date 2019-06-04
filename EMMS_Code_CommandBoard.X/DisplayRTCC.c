/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include "common.h"
#include "I2C_RTCC.h"

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
 *****************/
char BcdToDec( unsigned int val );
unsigned int DecToBcd( char val );

/****************
 CODE
 ****************/

/* readTime
 * reads time from RTCC, writes to global time variables
 */
void readTime( void )
{

    unsigned int temp1;
    unsigned int temp2;

    _RTCPTR = 0b11;

    temp1 = RTCVAL;
    timeYear = BcdToDec( temp1 );

    temp1 = RTCVAL;
    temp2 = temp1 >> 8;
    timeMonth = BcdToDec( temp2 );
    temp2 = temp1 & 0x00FF;
    timeDay = BcdToDec( temp2 );

    temp1 = RTCVAL;
    temp2 = temp1 & 0x00FF;
    timeHour = BcdToDec( temp2 );

    temp1 = RTCVAL;
    temp2 = temp1 >> 8;
    timeMinute = BcdToDec( temp2 );
    temp2 = temp1 & 0x00FF;
    timeSecond = BcdToDec( temp2 );

}

bool writeTime( char newYear, char newMonth, char newDay, char newHour, char newMinute, char newSecond )
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
    switch( newMonth )
    {
    case 4:
    case 6:
    case 9:
    case 11:
	if( newDay > 30 )
	    return -1;
	break;

    case 2:
	// check leap year - ignore century skips (year 2000))
	if( newYear % 4 )
	{
	    if( newDay > 28 )
		validDateTime = false;
	}
	else
	{
	    if( newDay > 29 )
		validDateTime = false;
	}
	break;
    }

    // put values into BCD
    tempYear = DecToBcd( newYear );
    tempMonth = DecToBcd( newMonth );
    tempDay = DecToBcd( newDay );
    tempHour = DecToBcd( newHour );
    tempMinute = DecToBcd( newMinute );
    tempSecond = DecToBcd( newSecond );

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
