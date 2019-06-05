/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include "common.h"
#include "MasterComm.h"
#include "PowerMain.h"

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
char passwordSet[6];

// internal only

// not yet sorted
int __attribute__( (space( eedata )) ) EEpassword0 = '1';
int __attribute__( (space( eedata )) ) EEpassword1 = '2';
int __attribute__( (space( eedata )) ) EEpassword2 = '3';
int __attribute__( (space( eedata )) ) EEpassword3 = '4';
int __attribute__( (space( eedata )) ) EEpassword4 = '1';
int __attribute__( (space( eedata )) ) EEpassword5 = '2';
int __attribute__( (space( eedata )) ) EEpowerAlloc = 500;
//int __attribute__ ((space(eedata))) EEyear = 14;
//int __attribute__ ((space(eedata))) EEmonth = 1;
//int __attribute__ ((space(eedata))) EEday = 1;
int __attribute__( (space( eedata )) ) EEemerButton = 0;
int __attribute__( (space( eedata )) ) EEresetTime = 700;
int __attribute__( (space( eedata )) ) EEaudibleAlarm = 0;
int __attribute__( (space( eedata )) ) EEalarmOnePower = 0;
int __attribute__( (space( eedata )) ) EEalarmTwoPower = 0;
int __attribute__( (space( eedata )) ) EEtotalUsedH = 0;
int __attribute__( (space( eedata )) ) EEtotalUsedL = 0;
int __attribute__( (space( eedata )) ) EEpreviousDayUsedH = 0;
int __attribute__( (space( eedata )) ) EEpreviousDayUsedL = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed1H = 0xFF;
int __attribute__( (space( eedata )) ) EEpowerUsed1L = 0xFF;
int __attribute__( (space( eedata )) ) EEpowerUsed2H = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed2L = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed3H = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed3L = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed4H = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed4L = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed5H = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed5L = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed6H = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed6L = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed7H = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed7L = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed8H = 0;
int __attribute__( (space( eedata )) ) EEpowerUsed8L = 0;
int __attribute__( (space( eedata )) ) EEhighLow = 0xFF;
int __attribute__( (space( eedata )) ) EErelay = 0xFF;

/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
     should be marked
 *****************/
unsigned char findPowerToRead( void );
unsigned char findPowerToWrite( void );

void EEreadAlarm( void );
void EEreadPassword( void );
void EEreadEmerButton( void );
void EEreadResetTime( void );


// not yet sorted
void EEreadEnergyAlloc( void );
void EEreadTotals( void );
void EEreadPowerUsed( void );
//void EEreadHL( void );
void EEreadRelay( void );

/****************
 CODE
 ****************/
void EEreadAll( void )
{
    EEreadPassword( );
    EEreadEnergyAlloc( );
    //    EEreadDate();
    EEreadEmerButton( );
    EEreadResetTime( );
    EEreadAlarm( );
    EEreadTotals( );
    EEreadPowerUsed( );
    //    EEreadHL( );
    EEreadRelay( );
}

void EEreadPassword( void )
{
    while( _WR ) continue;

    TBLPAG = __builtin_tblpage( &EEpassword0 );
    unsigned int offset = __builtin_tbloffset( &EEpassword0 );
    passwordSet[0] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword1 );
    offset = __builtin_tbloffset( &EEpassword1 );
    passwordSet[1] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword2 );
    offset = __builtin_tbloffset( &EEpassword2 );
    passwordSet[2] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword3 );
    offset = __builtin_tbloffset( &EEpassword3 );
    passwordSet[3] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword4 );
    offset = __builtin_tbloffset( &EEpassword4 );
    passwordSet[4] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword5 );
    offset = __builtin_tbloffset( &EEpassword5 );
    passwordSet[5] = (char) __builtin_tblrdl( offset );
}

void EEwritePassword( void )
{

    // char 0
    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword0 );
    unsigned int offset = __builtin_tbloffset( &EEpassword0 );
    __builtin_tblwtl( offset, passwordSet[0] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 1
    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword1 );
    offset = __builtin_tbloffset( &EEpassword1 );
    __builtin_tblwtl( offset, passwordSet[1] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 2
    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword2 );
    offset = __builtin_tbloffset( &EEpassword2 );
    __builtin_tblwtl( offset, passwordSet[2] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 3
    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword3 );
    offset = __builtin_tbloffset( &EEpassword3 );
    __builtin_tblwtl( offset, passwordSet[3] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 4
    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword4 );
    offset = __builtin_tbloffset( &EEpassword4 );
    __builtin_tblwtl( offset, passwordSet[4] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 5
    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword5 );
    offset = __builtin_tbloffset( &EEpassword5 );
    __builtin_tblwtl( offset, passwordSet[5] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );
}

void EEreadEnergyAlloc( void )
{
    while( _WR ) continue;

    TBLPAG = __builtin_tblpage( &EEpowerAlloc );
    unsigned int offset = __builtin_tbloffset( &EEpowerAlloc );
    //    powerAllocated = ( unsigned long ) __builtin_tblrdl( offset );
    tba_energyAllocation = (long) __builtin_tblrdl( offset );

}

void EEwriteEnergyAlloc( void )
{
    unsigned int tempEnergyAllocation;

    // explicitly show the down casting to int
    // this does not hide the fact that allocation is a long that we are storing as an int
    tempEnergyAllocation = (int) tba_energyAllocation;

    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpowerAlloc );
    unsigned int offset = __builtin_tbloffset( &EEpowerAlloc );
    //    __builtin_tblwtl(offset, powerAllocated);
    __builtin_tblwtl( offset, tempEnergyAllocation );

    asm volatile ("disi #5");
    __builtin_write_NVM( );
}

//void EEreadDate(void){
//    while (_WR) continue;
//
//    TBLPAG = __builtin_tblpage(&EEyear);
//    unsigned int offset = __builtin_tbloffset(&EEyear);
//    unsigned char tempYear = (unsigned char) __builtin_tblrdl(offset);
//
//    TBLPAG = __builtin_tblpage(&EEmonth);
//    offset = __builtin_tbloffset(&EEmonth);
//    unsigned char tempMonth = (unsigned char) __builtin_tblrdl(offset);
//
//    TBLPAG = __builtin_tblpage(&EEday);
//    offset = __builtin_tbloffset(&EEday);
//    unsigned char tempDay = (unsigned char) __builtin_tblrdl(offset);
//
//
//    writeTime(tempYear, tempMonth, tempDay, 0, 0, 0);
//}
//void EEwriteDate(void){
//
//    while (_WR) continue;
//    NVMCON = 0x4004;
//
//    TBLPAG = __builtin_tblpage(&EEyear);
//    unsigned int offset = __builtin_tbloffset(&EEyear);
//    __builtin_tblwtl(offset, timeYear);
//
//    asm volatile ("disi #5");
//    __builtin_write_NVM();
//
//    while (_WR) continue;
//    NVMCON = 0x4004;
//
//    TBLPAG = __builtin_tblpage(&EEmonth);
//    offset = __builtin_tbloffset(&EEmonth);
//    __builtin_tblwtl(offset, timeMonth);
//
//    asm volatile ("disi #5");
//    __builtin_write_NVM();
//
//    while (_WR) continue;
//    NVMCON = 0x4004;
//
//    TBLPAG = __builtin_tblpage(&EEday);
//    offset = __builtin_tbloffset(&EEday);
//    __builtin_tblwtl(offset, timeDay);
//
//    asm volatile ("disi #5");
//    __builtin_write_NVM();
//}

void EEreadEmerButton( )
{
    while( _WR ) continue;

    TBLPAG = __builtin_tblpage( &EEemerButton );
    unsigned int offset = __builtin_tbloffset( &EEemerButton );
    int tempEmerAlloc = __builtin_tblrdl( offset );

    if( tempEmerAlloc )
    {
	emerButtonEnable = 1;
	emerButtonEnergyAllocate = tempEmerAlloc;
    }
    else
    {
	emerButtonEnable = 0;
	emerButtonEnergyAllocate = 0;
    }
}

void EEwriteEmerButton( )
{
    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEemerButton );
    unsigned int offset = __builtin_tbloffset( &EEemerButton );
    __builtin_tblwtl( offset, emerButtonEnergyAllocate );

    asm volatile ("disi #5");
    __builtin_write_NVM( );
}

void EEreadResetTime( )
{
    while( _WR ) continue;

    TBLPAG = __builtin_tblpage( &EEresetTime );
    unsigned int offset = __builtin_tbloffset( &EEresetTime );
    unsigned int tempResetTime = __builtin_tblrdl( offset );

    resetTimeMinute = tempResetTime % 100;
    resetTimeHour = tempResetTime / 100;
}

void EEwriteResetTime( )
{
    while( _WR ) continue;
    NVMCON = 0x4004;

    unsigned int tempResetTime = resetTimeMinute + (100 * resetTimeHour);

    TBLPAG = __builtin_tblpage( &EEresetTime );
    unsigned int offset = __builtin_tbloffset( &EEresetTime );
    __builtin_tblwtl( offset, tempResetTime );

    asm volatile ("disi #5");
    __builtin_write_NVM( );
}

void EEreadAlarm( )
{
    while( _WR ) continue;

    TBLPAG = __builtin_tblpage( &EEaudibleAlarm );
    unsigned int offset = __builtin_tbloffset( &EEaudibleAlarm );
    audibleAlarm = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEalarmOnePower );
    offset = __builtin_tbloffset( &EEalarmOnePower );
    alarm1Energy = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEalarmTwoPower );
    offset = __builtin_tbloffset( &EEalarmTwoPower );
    alarm2Energy = (char) __builtin_tblrdl( offset );

    if( alarm1Energy )
	alarm1Enabled = 1;
    else
	alarm1Enabled = 0;

    if( alarm2Energy )
	alarm2Enabled = 1;
    else
	alarm2Enabled = 0;
}

void EEwriteAlarm( )
{
    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEaudibleAlarm );
    unsigned int offset = __builtin_tbloffset( &EEaudibleAlarm );
    __builtin_tblwtl( offset, audibleAlarm );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEalarmOnePower );
    offset = __builtin_tbloffset( &EEalarmOnePower );
    __builtin_tblwtl( offset, alarm1Energy );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEalarmTwoPower );
    offset = __builtin_tbloffset( &EEalarmTwoPower );
    __builtin_tblwtl( offset, alarm2Energy );

    asm volatile ("disi #5");
    __builtin_write_NVM( );
}

void EEreadTotals( )
{

    unsigned int tempTotalUsedH;
    unsigned int tempTotalUsedL;
    unsigned int tempPreviousDayUsedH;
    unsigned int tempPreviousDayUsedL;

    while( _WR ) continue;

    TBLPAG = __builtin_tblpage( &EEtotalUsedH );
    unsigned int offset = __builtin_tbloffset( &EEtotalUsedH );
    tempTotalUsedH = __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEtotalUsedL );
    offset = __builtin_tbloffset( &EEtotalUsedL );
    tempTotalUsedL = __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpreviousDayUsedH );
    offset = __builtin_tbloffset( &EEpreviousDayUsedH );
    tempPreviousDayUsedH = __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpreviousDayUsedL );
    offset = __builtin_tbloffset( &EEpreviousDayUsedL );
    tempPreviousDayUsedL = __builtin_tblrdl( offset );

    tba_energyUsedLastDayReset = (((unsigned long) tempTotalUsedH) << 16) + tempTotalUsedL;
    tba_energyUsedPreviousDay = (((unsigned long) tempPreviousDayUsedH) << 16) + tempPreviousDayUsedL;
    //    totalUsed = ( ( ( unsigned long ) tempTotalUsedH ) << 16 ) + tempTotalUsedL;
    //    previousDayUsed = ( ( ( unsigned long ) tempPreviousDayUsedH ) << 16 ) + tempPreviousDayUsedL;
}

void EEwriteTotals( )
{

    unsigned int tempTotalUsedH;
    unsigned int tempTotalUsedL;
    unsigned int tempPreviousDayUsedH;
    unsigned int tempPreviousDayUsedL;

    tempTotalUsedH = tba_energyUsedLastDayReset >> 16;
    tempTotalUsedL = tba_energyUsedLastDayReset & 0xFFFF;
    tempPreviousDayUsedH = tba_energyUsedPreviousDay >> 16;
    tempPreviousDayUsedL = tba_energyUsedPreviousDay & 0xFFFF;


    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEtotalUsedH );
    unsigned int offset = __builtin_tbloffset( &EEtotalUsedH );
    __builtin_tblwtl( offset, tempTotalUsedH );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEtotalUsedL );
    offset = __builtin_tbloffset( &EEtotalUsedL );
    __builtin_tblwtl( offset, tempTotalUsedL );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpreviousDayUsedH );
    offset = __builtin_tbloffset( &EEpreviousDayUsedH );
    __builtin_tblwtl( offset, tempPreviousDayUsedH );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpreviousDayUsedL );
    offset = __builtin_tbloffset( &EEpreviousDayUsedL );
    __builtin_tblwtl( offset, tempPreviousDayUsedL );

    asm volatile ("disi #5");
    __builtin_write_NVM( );
}

void EEreadPowerUsed( )
{
    unsigned char EEpowerSelect;
    unsigned int tempPowerH;
    unsigned int tempPowerL;
    unsigned int offset;

    while( _WR ) continue;

    EEpowerSelect = findPowerToRead( );

    switch( EEpowerSelect )
    {
    case 0:
	tempPowerH = tempPowerL = 0;
	break;

    case 1:
	TBLPAG = __builtin_tblpage( &EEpowerUsed1H );
	offset = __builtin_tbloffset( &EEpowerUsed1H );
	tempPowerH = __builtin_tblrdl( offset );
	TBLPAG = __builtin_tblpage( &EEpowerUsed1L );
	offset = __builtin_tbloffset( &EEpowerUsed1L );
	tempPowerL = __builtin_tblrdl( offset );
	break;

    case 2:
	TBLPAG = __builtin_tblpage( &EEpowerUsed2H );
	offset = __builtin_tbloffset( &EEpowerUsed2H );
	tempPowerH = __builtin_tblrdl( offset );
	TBLPAG = __builtin_tblpage( &EEpowerUsed2L );
	offset = __builtin_tbloffset( &EEpowerUsed2L );
	tempPowerL = __builtin_tblrdl( offset );
	break;

    case 3:
	TBLPAG = __builtin_tblpage( &EEpowerUsed3H );
	offset = __builtin_tbloffset( &EEpowerUsed3H );
	tempPowerH = __builtin_tblrdl( offset );
	TBLPAG = __builtin_tblpage( &EEpowerUsed3L );
	offset = __builtin_tbloffset( &EEpowerUsed3L );
	tempPowerL = __builtin_tblrdl( offset );

	break;

    case 4:
	TBLPAG = __builtin_tblpage( &EEpowerUsed4H );
	offset = __builtin_tbloffset( &EEpowerUsed4H );
	tempPowerH = __builtin_tblrdl( offset );
	TBLPAG = __builtin_tblpage( &EEpowerUsed4L );
	offset = __builtin_tbloffset( &EEpowerUsed4L );
	tempPowerL = __builtin_tblrdl( offset );
	break;

    case 5:
	TBLPAG = __builtin_tblpage( &EEpowerUsed5H );
	offset = __builtin_tbloffset( &EEpowerUsed5H );
	tempPowerH = __builtin_tblrdl( offset );
	TBLPAG = __builtin_tblpage( &EEpowerUsed5L );
	offset = __builtin_tbloffset( &EEpowerUsed5L );
	tempPowerL = __builtin_tblrdl( offset );
	break;

    case 6:
	TBLPAG = __builtin_tblpage( &EEpowerUsed6H );
	offset = __builtin_tbloffset( &EEpowerUsed6H );
	tempPowerH = __builtin_tblrdl( offset );
	TBLPAG = __builtin_tblpage( &EEpowerUsed6L );
	offset = __builtin_tbloffset( &EEpowerUsed6L );
	tempPowerL = __builtin_tblrdl( offset );
	break;

    case 7:
	TBLPAG = __builtin_tblpage( &EEpowerUsed7H );
	offset = __builtin_tbloffset( &EEpowerUsed7H );
	tempPowerH = __builtin_tblrdl( offset );
	TBLPAG = __builtin_tblpage( &EEpowerUsed7L );
	offset = __builtin_tbloffset( &EEpowerUsed7L );
	tempPowerL = __builtin_tblrdl( offset );
	break;

    case 8:
	TBLPAG = __builtin_tblpage( &EEpowerUsed8H );
	offset = __builtin_tbloffset( &EEpowerUsed8H );
	tempPowerH = __builtin_tblrdl( offset );
	TBLPAG = __builtin_tblpage( &EEpowerUsed8L );
	offset = __builtin_tbloffset( &EEpowerUsed8L );
	tempPowerL = __builtin_tblrdl( offset );
    }

    //    powerUsedMW = ( ( ( unsigned long ) tempPowerH ) << 16 ) + tempPowerL;
    tba_energyUsedLifetime = (((unsigned long) tempPowerH) << 16) + tempPowerL;
}

void EEwriteEnergyUsed( )
{
    unsigned int offset;
    unsigned int tempPowerH = tba_energyUsedLifetime >> 16;
    unsigned int tempPowerL = tba_energyUsedLifetime & 0xFFFF;
    //    unsigned int tempPowerH = powerUsedMW >> 16;
    //    unsigned int tempPowerL = powerUsedMW & 0xFFFF;
    unsigned char EEpowerSelect = findPowerToWrite( );

    switch( EEpowerSelect )
    {
    case 1:
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed1H );
	offset = __builtin_tbloffset( &EEpowerUsed1H );
	__builtin_tblwtl( offset, tempPowerH );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed1L );
	offset = __builtin_tbloffset( &EEpowerUsed1L );
	__builtin_tblwtl( offset, tempPowerL );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed2H );
	offset = __builtin_tbloffset( &EEpowerUsed2H );
	__builtin_tblwtl( offset, 0xFFFF );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	break;

    case 2:
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed2H );
	offset = __builtin_tbloffset( &EEpowerUsed2H );
	__builtin_tblwtl( offset, tempPowerH );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed2L );
	offset = __builtin_tbloffset( &EEpowerUsed2L );
	__builtin_tblwtl( offset, tempPowerL );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed3H );
	offset = __builtin_tbloffset( &EEpowerUsed3H );
	__builtin_tblwtl( offset, 0xFFFF );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	break;

    case 3:
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed3H );
	offset = __builtin_tbloffset( &EEpowerUsed3H );
	__builtin_tblwtl( offset, tempPowerH );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed3L );
	offset = __builtin_tbloffset( &EEpowerUsed3L );
	__builtin_tblwtl( offset, tempPowerL );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed4H );
	offset = __builtin_tbloffset( &EEpowerUsed4H );
	__builtin_tblwtl( offset, 0xFFFF );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	break;

    case 4:
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed4H );
	offset = __builtin_tbloffset( &EEpowerUsed4H );
	__builtin_tblwtl( offset, tempPowerH );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed4L );
	offset = __builtin_tbloffset( &EEpowerUsed4L );
	__builtin_tblwtl( offset, tempPowerL );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed5H );
	offset = __builtin_tbloffset( &EEpowerUsed5H );
	__builtin_tblwtl( offset, 0xFFFF );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	break;

    case 5:
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed5H );
	offset = __builtin_tbloffset( &EEpowerUsed5H );
	__builtin_tblwtl( offset, tempPowerH );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed5L );
	offset = __builtin_tbloffset( &EEpowerUsed5L );
	__builtin_tblwtl( offset, tempPowerL );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed6H );
	offset = __builtin_tbloffset( &EEpowerUsed6H );
	__builtin_tblwtl( offset, 0xFFFF );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	break;

    case 6:
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed6H );
	offset = __builtin_tbloffset( &EEpowerUsed6H );
	__builtin_tblwtl( offset, tempPowerH );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed6L );
	offset = __builtin_tbloffset( &EEpowerUsed6L );
	__builtin_tblwtl( offset, tempPowerL );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed7H );
	offset = __builtin_tbloffset( &EEpowerUsed7H );
	__builtin_tblwtl( offset, 0xFFFF );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	break;

    case 7:
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed7H );
	offset = __builtin_tbloffset( &EEpowerUsed7H );
	__builtin_tblwtl( offset, tempPowerH );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed7L );
	offset = __builtin_tbloffset( &EEpowerUsed7L );
	__builtin_tblwtl( offset, tempPowerL );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed8H );
	offset = __builtin_tbloffset( &EEpowerUsed8H );
	__builtin_tblwtl( offset, 0xFFFF );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	break;

    case 8:
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed8H );
	offset = __builtin_tbloffset( &EEpowerUsed8H );
	__builtin_tblwtl( offset, tempPowerH );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed8L );
	offset = __builtin_tbloffset( &EEpowerUsed8L );
	__builtin_tblwtl( offset, tempPowerL );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
	while( _WR ) continue;
	NVMCON = 0x4004;
	TBLPAG = __builtin_tblpage( &EEpowerUsed1H );
	offset = __builtin_tbloffset( &EEpowerUsed1H );
	__builtin_tblwtl( offset, 0xFFFF );
	asm volatile ("disi #5");
	__builtin_write_NVM( );
    }
}

//void EEreadHL( )
//{
//    while( _WR ) continue;
//
//    TBLPAG = __builtin_tblpage( &EEhighLow );
//    unsigned int offset = __builtin_tbloffset( &EEhighLow );
//    isHigh = (char) __builtin_tblrdl( offset );
//}
//
//void EEwriteHL( )
//{
//    while( _WR ) continue;
//    NVMCON = 0x4004;
//
//    TBLPAG = __builtin_tblpage( &EEhighLow );
//    unsigned int offset = __builtin_tbloffset( &EEhighLow );
//    __builtin_tblwtl( offset, isHigh );
//
//    asm volatile ("disi #5");
//    __builtin_write_NVM( );
//}

void EEreadRelay( )
{
    while( _WR ) continue;

    TBLPAG = __builtin_tblpage( &EErelay );
    unsigned int offset = __builtin_tbloffset( &EErelay );
    relayActive = (char) __builtin_tblrdl( offset );
}

void EEwriteRelay( )
{
    while( _WR ) continue;
    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EErelay );
    unsigned int offset = __builtin_tbloffset( &EErelay );
    __builtin_tblwtl( offset, relayActive );

    asm volatile ("disi #5");
    __builtin_write_NVM( );
}

unsigned char findPowerToRead( )
{

    unsigned int tempPower;
    unsigned char EEpowerSelect;

    TBLPAG = __builtin_tblpage( &EEpowerUsed1H );
    unsigned int offset = __builtin_tbloffset( &EEpowerUsed1H );
    tempPower = __builtin_tblrdl( offset );

    if( tempPower == 0xFFFF )
    {
	EEpowerSelect = 8;
    }
    else
    {
	TBLPAG = __builtin_tblpage( &EEpowerUsed2H );
	unsigned int offset = __builtin_tbloffset( &EEpowerUsed2H );
	tempPower = __builtin_tblrdl( offset );

	if( tempPower == 0xFFFF )
	{
	    EEpowerSelect = 1;
	}
	else
	{
	    TBLPAG = __builtin_tblpage( &EEpowerUsed3H );
	    unsigned int offset = __builtin_tbloffset( &EEpowerUsed3H );
	    tempPower = __builtin_tblrdl( offset );

	    if( tempPower == 0xFFFF )
	    {
		EEpowerSelect = 2;
	    }
	    else
	    {
		TBLPAG = __builtin_tblpage( &EEpowerUsed4H );
		unsigned int offset = __builtin_tbloffset( &EEpowerUsed4H );
		tempPower = __builtin_tblrdl( offset );

		if( tempPower == 0xFFFF )
		{
		    EEpowerSelect = 3;
		}
		else
		{
		    TBLPAG = __builtin_tblpage( &EEpowerUsed5H );
		    unsigned int offset = __builtin_tbloffset( &EEpowerUsed5H );
		    tempPower = __builtin_tblrdl( offset );

		    if( tempPower == 0xFFFF )
		    {
			EEpowerSelect = 4;
		    }
		    else
		    {
			TBLPAG = __builtin_tblpage( &EEpowerUsed6H );
			unsigned int offset = __builtin_tbloffset( &EEpowerUsed6H );
			tempPower = __builtin_tblrdl( offset );

			if( tempPower == 0xFFFF )
			{
			    EEpowerSelect = 5;
			}
			else
			{
			    TBLPAG = __builtin_tblpage( &EEpowerUsed7H );
			    unsigned int offset = __builtin_tbloffset( &EEpowerUsed7H );
			    tempPower = __builtin_tblrdl( offset );

			    if( tempPower == 0xFFFF )
			    {
				EEpowerSelect = 6;
			    }
			    else
			    {
				TBLPAG = __builtin_tblpage( &EEpowerUsed8H );
				unsigned int offset = __builtin_tbloffset( &EEpowerUsed8H );
				tempPower = __builtin_tblrdl( offset );

				if( tempPower == 0xFFFF )
				{
				    EEpowerSelect = 7;
				}
				else
				{
				    EEpowerSelect = 0;
				}
			    }
			}
		    }
		}
	    }
	}
    }

    return EEpowerSelect;
}

unsigned char findPowerToWrite( )
{
    unsigned char temp = findPowerToRead( );
    temp = (temp % 8) + 1;
    return temp;
}
