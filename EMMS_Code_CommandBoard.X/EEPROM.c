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

// internal only


// EEPROM reservations
int __attribute__( (space( eedata )) ) EEpassword0 = '1';
int __attribute__( (space( eedata )) ) EEpassword1 = '2';
int __attribute__( (space( eedata )) ) EEpassword2 = '3';
int __attribute__( (space( eedata )) ) EEpassword3 = '4';
int __attribute__( (space( eedata )) ) EEpassword4 = '1';
int __attribute__( (space( eedata )) ) EEpassword5 = '2';
int __attribute__( (space( eedata )) ) EEpowerAlloc = 500;
int __attribute__( (space( eedata )) ) EEemerButton = 0;
int __attribute__( (space( eedata )) ) EEresetTime = 700;
int __attribute__( (space( eedata )) ) EEaudibleAlarm = 0;
int __attribute__( (space( eedata )) ) EEalarmOnePower = 0;
int __attribute__( (space( eedata )) ) EEalarmTwoPower = 0;
int __attribute__( (space( eedata )) ) EEenergyLastResetH = 0;
int __attribute__( (space( eedata )) ) EEenergyLastResetL = 0;
int __attribute__( (space( eedata )) ) EEpreviousCycleUsedH = 0;
int __attribute__( (space( eedata )) ) EEpreviousCycleUsedL = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime1H = 0xFF;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime1L = 0xFF;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime2H = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime2L = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime3H = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime3L = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime4H = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime4L = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime5H = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime5L = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime6H = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime6L = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime7H = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime7L = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime8H = 0;
int __attribute__( (space( eedata )) ) EEenergyUsedLifetime8L = 0;
int __attribute__( (space( eedata )) ) EErelayActive = 0xFF;
int __attribute__( (space( eedata )) ) EErtccIsSet = 0;

/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
     should be marked
 *****************/

// external only



// internal and external


// internal only
void writeWait( void );
unsigned char findSlotToRead( void );
unsigned char findPowerToWrite( void );
unsigned long combineIntsToLong( unsigned int intH, unsigned int intL );
void separateLongToInts( unsigned long data, unsigned int *intH, unsigned int *intL );

/****************
 CODE
 ****************/

// wait for the EEPROM to be ready

void writeWait( void )
{
    bool writeDone = false;

    // wait for the write control bit to reset before continuing
    while( writeDone == false )
    {
	if( NVMCONbits.WR == 0 )
	{
	    writeDone = true;
	}
    };

    return;
}

void eeReadPasswordNew( char* password )
{
    unsigned int offset;

    writeWait( );

    TBLPAG = __builtin_tblpage( &EEpassword0 );
    offset = __builtin_tbloffset( &EEpassword0 );
    password[0] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword1 );
    offset = __builtin_tbloffset( &EEpassword1 );
    password[1] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword2 );
    offset = __builtin_tbloffset( &EEpassword2 );
    password[2] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword3 );
    offset = __builtin_tbloffset( &EEpassword3 );
    password[3] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword4 );
    offset = __builtin_tbloffset( &EEpassword4 );
    password[4] = (char) __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpassword5 );
    offset = __builtin_tbloffset( &EEpassword5 );
    password[5] = (char) __builtin_tblrdl( offset );

    return;
}

void eeWritePasswordNew( char *password )
{
    unsigned int offset;

    // char 0
    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword0 );
    offset = __builtin_tbloffset( &EEpassword0 );
    __builtin_tblwtl( offset, password[0] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 1
    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword1 );
    offset = __builtin_tbloffset( &EEpassword1 );
    __builtin_tblwtl( offset, password[1] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 2
    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword2 );
    offset = __builtin_tbloffset( &EEpassword2 );
    __builtin_tblwtl( offset, password[2] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 3
    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword3 );
    offset = __builtin_tbloffset( &EEpassword3 );
    __builtin_tblwtl( offset, password[3] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 4
    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword4 );
    offset = __builtin_tbloffset( &EEpassword4 );
    __builtin_tblwtl( offset, password[4] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    // char 5
    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpassword5 );
    offset = __builtin_tbloffset( &EEpassword5 );
    __builtin_tblwtl( offset, password[5] );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    return;
}

long eeReadEnergyCycleAllocationNew( void )
{
    unsigned int offset;

    unsigned int temp;
    long energyAllocation;

    writeWait( );

    TBLPAG = __builtin_tblpage( &EEpowerAlloc );
    offset = __builtin_tbloffset( &EEpowerAlloc );
    //    powerAllocated = ( unsigned long ) __builtin_tblrdl( offset );
    temp = __builtin_tblrdl( offset );
    energyAllocation = __builtin_tblrdl( offset );

    return energyAllocation;
}

void eeWriteEnergyAllocNew( long energyAllocation )
{
    unsigned int offset;

    unsigned int tempEnergyAllocation;

    // explicitly show the down casting to int
    // this does not hide the fact that allocation is a long that we are storing as an int
    tempEnergyAllocation = (int) energyAllocation;

    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpowerAlloc );
    offset = __builtin_tbloffset( &EEpowerAlloc );
    __builtin_tblwtl( offset, tempEnergyAllocation );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    return;
}

struct emergency_button eeReadEmergencyButtonNew( void )
{
    unsigned int offset;

    struct emergency_button emergencyButtonTemp;


    int tempEmerAlloc;

    writeWait( );

    TBLPAG = __builtin_tblpage( &EEemerButton );
    offset = __builtin_tbloffset( &EEemerButton );
    tempEmerAlloc = __builtin_tblrdl( offset );

    emergencyButtonTemp.energyAmount = tempEmerAlloc;
    if( tempEmerAlloc > 0 )
    {
	emergencyButtonTemp.enabled = true;
    }
    else
    {
	emergencyButtonTemp.enabled = false;
    }

    return emergencyButtonTemp;
}

void eeWriteEmerButtonNew( struct emergency_button emergencyButton )
{
    unsigned int offset;

    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEemerButton );
    offset = __builtin_tbloffset( &EEemerButton );
    __builtin_tblwtl( offset, emergencyButton.energyAmount );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    return;
}

struct reset_time eeReadResetTimeNew( void )
{
    unsigned int offset;

    unsigned int tempResetTime;
    struct reset_time resetTimeTemp;

    writeWait( );

    TBLPAG = __builtin_tblpage( &EEresetTime );
    offset = __builtin_tbloffset( &EEresetTime );
    tempResetTime = __builtin_tblrdl( offset );

    resetTimeTemp.minute = tempResetTime % 100;
    resetTimeTemp.hour = tempResetTime / 100;

    return resetTimeTemp;
}

void eeWriteResetTimeNew( int resetHour, int resetMinute )
{
    unsigned int offset;

    writeWait( );

    NVMCON = 0x4004;

    unsigned int tempResetTime;

    tempResetTime = resetMinute + (100 * resetHour);

    TBLPAG = __builtin_tblpage( &EEresetTime );
    offset = __builtin_tbloffset( &EEresetTime );
    __builtin_tblwtl( offset, tempResetTime );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    return;
}

struct alarm_info eeReadAlarmNew( void )
{
    unsigned int offset;

    struct alarm_info alarmInfoTemp;

    int audibleAlarm;
    int alarm1Energy;
    int alarm2Energy;

    writeWait( );

    TBLPAG = __builtin_tblpage( &EEaudibleAlarm );
    offset = __builtin_tbloffset( &EEaudibleAlarm );
    audibleAlarm = __builtin_tblrdl( offset );


    TBLPAG = __builtin_tblpage( &EEalarmOnePower );
    offset = __builtin_tbloffset( &EEalarmOnePower );
    alarm1Energy = __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEalarmTwoPower );
    offset = __builtin_tbloffset( &EEalarmTwoPower );
    alarm2Energy = __builtin_tblrdl( offset );

    if( audibleAlarm == 0 )
    {
	alarmInfoTemp.alarmAudible = false;
    }
    else
    {
	alarmInfoTemp.alarmAudible = true;
    }

    alarmInfoTemp.alarm1Energy = alarm1Energy;
    if( alarmInfoTemp.alarm1Energy > 0 )
    {
	alarmInfoTemp.alarm1Enabled = true;
    }
    else
    {
	alarmInfoTemp.alarm1Enabled = false;
    }

    alarmInfoTemp.alarm2Energy = alarm2Energy;
    if( alarmInfoTemp.alarm2Energy > 0 )
    {
	alarmInfoTemp.alarm2Enabled = false;
    }
    else
    {
	alarmInfoTemp.alarm2Enabled = false;
    }

    return alarmInfoTemp;
}

void eeWriteAlarmNew( struct alarm_info alarms )
{
    unsigned int offset;

    unsigned int audibleAlarmTemp;

    if( alarms.alarmAudible == true )
    {
	audibleAlarmTemp = 0xFFFF;
    }
    else
    {
	audibleAlarmTemp = 0;
    }

    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEaudibleAlarm );
    offset = __builtin_tbloffset( &EEaudibleAlarm );
    __builtin_tblwtl( offset, audibleAlarmTemp );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEalarmOnePower );
    offset = __builtin_tbloffset( &EEalarmOnePower );
    __builtin_tblwtl( offset, alarms.alarm1Energy );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEalarmTwoPower );
    offset = __builtin_tbloffset( &EEalarmTwoPower );
    __builtin_tblwtl( offset, alarms.alarm2Energy );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    return;
}

struct energy_info eeReadTotalsNew( void )
{
    unsigned int offset;

    struct energy_info energyUsedTemp;

    unsigned int tempH;
    unsigned int tempL;

    writeWait( );

    TBLPAG = __builtin_tblpage( &EEenergyLastResetH );
    offset = __builtin_tbloffset( &EEenergyLastResetH );
    tempH = __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEenergyLastResetL );
    offset = __builtin_tbloffset( &EEenergyLastResetL );
    tempL = __builtin_tblrdl( offset );

    energyUsedTemp.lastReset = combineIntsToLong( tempH, tempL );


    TBLPAG = __builtin_tblpage( &EEpreviousCycleUsedH );
    offset = __builtin_tbloffset( &EEpreviousCycleUsedH );
    tempH = __builtin_tblrdl( offset );

    TBLPAG = __builtin_tblpage( &EEpreviousCycleUsedL );
    offset = __builtin_tbloffset( &EEpreviousCycleUsedL );
    tempL = __builtin_tblrdl( offset );

    energyUsedTemp.previousCycleUsed = combineIntsToLong( tempH, tempL );

    return energyUsedTemp;
}

void eeWriteEnergyTotalsNew( struct energy_info energyData )
{
    unsigned int offset;

    unsigned int tempH;
    unsigned int tempL;


    separateLongToInts( energyData.lastReset, &tempH, &tempL );

    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEenergyLastResetH );
    offset = __builtin_tbloffset( &EEenergyLastResetH );
    __builtin_tblwtl( offset, tempH );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEenergyLastResetL );
    offset = __builtin_tbloffset( &EEenergyLastResetL );
    __builtin_tblwtl( offset, tempL );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    separateLongToInts( energyData.previousCycleUsed, &tempH, &tempL );

    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpreviousCycleUsedH );
    offset = __builtin_tbloffset( &EEpreviousCycleUsedH );
    __builtin_tblwtl( offset, tempH );

    asm volatile ("disi #5");
    __builtin_write_NVM( );


    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EEpreviousCycleUsedL );
    offset = __builtin_tbloffset( &EEpreviousCycleUsedL );
    __builtin_tblwtl( offset, tempL );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    return;
}

unsigned long eeReadEnergyUsedNew( )
{
    unsigned int offset;

    unsigned char EEpowerSelect;

    unsigned int tempH;
    unsigned int tempL;

    unsigned long temp;


    writeWait( ); // this might not need to be here, but it doesn't hurt if it is here

    EEpowerSelect = findSlotToRead( );

    switch( EEpowerSelect )
    {
	case 0:
	    tempH = tempL = 0;
	    break;

	case 1:
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime1H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime1H );
	    tempH = __builtin_tblrdl( offset );
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime1L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime1L );
	    tempL = __builtin_tblrdl( offset );
	    break;

	case 2:
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime2H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime2H );
	    tempH = __builtin_tblrdl( offset );
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime2L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime2L );
	    tempL = __builtin_tblrdl( offset );
	    break;

	case 3:
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime3H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime3H );
	    tempH = __builtin_tblrdl( offset );
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime3L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime3L );
	    tempL = __builtin_tblrdl( offset );
	    break;

	case 4:
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime4H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime4H );
	    tempH = __builtin_tblrdl( offset );
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime4L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime4L );
	    tempL = __builtin_tblrdl( offset );
	    break;

	case 5:
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime5H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime5H );
	    tempH = __builtin_tblrdl( offset );
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime5L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime5L );
	    tempL = __builtin_tblrdl( offset );
	    break;

	case 6:
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime6H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime6H );
	    tempH = __builtin_tblrdl( offset );
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime6L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime6L );
	    tempL = __builtin_tblrdl( offset );
	    break;

	case 7:
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime7H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime7H );
	    tempH = __builtin_tblrdl( offset );
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime7L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime7L );
	    tempL = __builtin_tblrdl( offset );
	    break;

	case 8:
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime8H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime8H );
	    tempH = __builtin_tblrdl( offset );
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime8L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime8L );
	    tempL = __builtin_tblrdl( offset );
    }

    temp = combineIntsToLong( tempH, tempL );

    return temp;
}

void eeWriteEnergyLifetimeNew( unsigned long energyLifetime )
{
    unsigned int offset;

    unsigned int tempH;
    unsigned int tempL;

    separateLongToInts( energyLifetime, &tempH, & tempL );

    unsigned char EESlotToWrite = findPowerToWrite( );


    // EEPROM Select
    // can we use a variable to pass the memory address
    // this is not very important, but keep from repeating lots of code
    // future work

    switch( EESlotToWrite )
    {
	case 1:
	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime1H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime1H );
	    __builtin_tblwtl( offset, tempH );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime1L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime1L );
	    __builtin_tblwtl( offset, tempL );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime2H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime2H );
	    __builtin_tblwtl( offset, 0xFFFF );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );
	    break;

	case 2:
	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime2H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime2H );
	    __builtin_tblwtl( offset, tempH );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime2L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime2L );
	    __builtin_tblwtl( offset, tempL );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime3H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime3H );
	    __builtin_tblwtl( offset, 0xFFFF );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );
	    break;

	case 3:
	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime3H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime3H );
	    __builtin_tblwtl( offset, tempH );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime3L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime3L );
	    __builtin_tblwtl( offset, tempL );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime4H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime4H );
	    __builtin_tblwtl( offset, 0xFFFF );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );
	    break;

	case 4:

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime4H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime4H );
	    __builtin_tblwtl( offset, tempH );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime4L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime4L );
	    __builtin_tblwtl( offset, tempL );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime5H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime5H );
	    __builtin_tblwtl( offset, 0xFFFF );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );
	    break;

	case 5:
	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime5H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime5H );
	    __builtin_tblwtl( offset, tempH );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime5L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime5L );
	    __builtin_tblwtl( offset, tempL );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime6H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime6H );
	    __builtin_tblwtl( offset, 0xFFFF );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );
	    break;

	case 6:
	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime6H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime6H );
	    __builtin_tblwtl( offset, tempH );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime6L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime6L );
	    __builtin_tblwtl( offset, tempL );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime7H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime7H );
	    __builtin_tblwtl( offset, 0xFFFF );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );
	    break;

	case 7:
	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime7H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime7H );
	    __builtin_tblwtl( offset, tempH );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime7L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime7L );
	    __builtin_tblwtl( offset, tempL );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime8H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime8H );
	    __builtin_tblwtl( offset, 0xFFFF );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );
	    break;

	case 8:
	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime8H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime8H );
	    __builtin_tblwtl( offset, tempH );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime8L );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime8L );
	    __builtin_tblwtl( offset, tempL );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );

	    writeWait( );

	    NVMCON = 0x4004;
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime1H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime1H );
	    __builtin_tblwtl( offset, 0xFFFF );
	    asm volatile ("disi #5");
	    __builtin_write_NVM( );
    }
}

bool eeReadRelayNew( )
{

    unsigned int offset;

    unsigned int tempRelayActive;
    bool relay;

    writeWait( );

    TBLPAG = __builtin_tblpage( &EErelayActive );
    offset = __builtin_tbloffset( &EErelayActive );
    tempRelayActive = __builtin_tblrdl( offset );

    if( tempRelayActive == 0 )
    {
	relay = false;
    }
    else
    {
	relay = true;
    }

    return relay;
}

void eeWriteRelayNew( bool relay )
{
    unsigned int offset;

    unsigned int relayTemp;

    if( relay == true )
    {
	relayTemp = 0xFFFF;
    }
    else
    {
	relayTemp = 0;
    }



    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EErelayActive );
    offset = __builtin_tbloffset( &EErelayActive );
    __builtin_tblwtl( offset, relayTemp );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    return;
}

bool eeReadRTCCIsSet( )
{

    unsigned int offset;

    unsigned int tempRTCCIsSet;
    bool rtccIsSet;

    writeWait( );

    TBLPAG = __builtin_tblpage( &EErtccIsSet );
    offset = __builtin_tbloffset( &EErtccIsSet );
    tempRTCCIsSet = __builtin_tblrdl( offset );

    if( tempRTCCIsSet == 0 )
    {
	rtccIsSet = false;
    }
    else
    {
	rtccIsSet = true;
    }

    return rtccIsSet;
}

void eeWriteRTCCIsSet( bool rtccIsSet )
{
    unsigned int offset;

    unsigned int tempRTCCIsSet;

    if( rtccIsSet == true )
    {
	tempRTCCIsSet = 0xFFFF;
    }
    else
    {
	tempRTCCIsSet = 0;
    }


    writeWait( );

    NVMCON = 0x4004;

    TBLPAG = __builtin_tblpage( &EErtccIsSet );
    offset = __builtin_tbloffset( &EErtccIsSet );
    __builtin_tblwtl( offset, tempRTCCIsSet );

    asm volatile ("disi #5");
    __builtin_write_NVM( );

    return;
}

unsigned char findSlotToRead( )
{
    // to keep the energy used from wearing out the EEPROM by writing to one spot repeatedly
    // we use a 'round robin' to store the value in different locations
    // find the location with 0xFF - the value we care about is in the previous location
    unsigned int offset;

    unsigned int tempPower;
    unsigned char EEpowerSelect;

    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime1H );
    offset = __builtin_tbloffset( &EEenergyUsedLifetime1H );
    tempPower = __builtin_tblrdl( offset );

    if( tempPower == 0xFFFF )
    {
	EEpowerSelect = 8;
    }
    else
    {
	TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime2H );
	offset = __builtin_tbloffset( &EEenergyUsedLifetime2H );
	tempPower = __builtin_tblrdl( offset );

	if( tempPower == 0xFFFF )
	{
	    EEpowerSelect = 1;
	}
	else
	{
	    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime3H );
	    offset = __builtin_tbloffset( &EEenergyUsedLifetime3H );
	    tempPower = __builtin_tblrdl( offset );

	    if( tempPower == 0xFFFF )
	    {
		EEpowerSelect = 2;
	    }
	    else
	    {
		TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime4H );
		offset = __builtin_tbloffset( &EEenergyUsedLifetime4H );
		tempPower = __builtin_tblrdl( offset );

		if( tempPower == 0xFFFF )
		{
		    EEpowerSelect = 3;
		}
		else
		{
		    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime5H );
		    offset = __builtin_tbloffset( &EEenergyUsedLifetime5H );
		    tempPower = __builtin_tblrdl( offset );

		    if( tempPower == 0xFFFF )
		    {
			EEpowerSelect = 4;
		    }
		    else
		    {
			TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime6H );
			offset = __builtin_tbloffset( &EEenergyUsedLifetime6H );
			tempPower = __builtin_tblrdl( offset );

			if( tempPower == 0xFFFF )
			{
			    EEpowerSelect = 5;
			}
			else
			{
			    TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime7H );
			    offset = __builtin_tbloffset( &EEenergyUsedLifetime7H );
			    tempPower = __builtin_tblrdl( offset );

			    if( tempPower == 0xFFFF )
			    {
				EEpowerSelect = 6;
			    }
			    else
			    {
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime8H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime8H );
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

    // to keep the energy used from wearing out the EEPROM by writing to one spot repeatedly
    // we use a 'round robin' to store the value in different locations
    // find the location with 0xFF
    // overwrite the 0xFF with the new value and make the next value 0xFF to set a new marker

    unsigned char temp;

    temp = findSlotToRead( );
    temp = (temp % 8) + 1;

    return temp;
}

unsigned long combineIntsToLong( unsigned int intH, unsigned int intL )
{

    // combine High and Low integers into a long

    unsigned long temp;

    temp = (((unsigned long) intH) << 16) + intL;

    return temp;
}

void separateLongToInts( unsigned long data, unsigned int *intH, unsigned int *intL )
{

    // EEPROM stores only 16 bits at a time - the size of an int
    // need to break the long (32 bits) into 2 ints

    *intH = data >> 16;
    *intL = data & 0XFFFF;

    return;
}
