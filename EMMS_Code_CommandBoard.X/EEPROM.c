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
//Not Named
int __attribute__( ( space( eedata ) ) ) EEmeterName00 = 'N';
int __attribute__( ( space( eedata ) ) ) EEmeterName01 = 'o';
int __attribute__( ( space( eedata ) ) ) EEmeterName02 = 't';
int __attribute__( ( space( eedata ) ) ) EEmeterName03 = ' ';
int __attribute__( ( space( eedata ) ) ) EEmeterName04 = 'N';
int __attribute__( ( space( eedata ) ) ) EEmeterName05 = 'a';
int __attribute__( ( space( eedata ) ) ) EEmeterName06 = 'm';
int __attribute__( ( space( eedata ) ) ) EEmeterName07 = 'e';
int __attribute__( ( space( eedata ) ) ) EEmeterName08 = 'd';
int __attribute__( ( space( eedata ) ) ) EEmeterName09 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName10 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName11 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName12 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName13 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName14 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName15 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName16 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName17 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName18 = CHAR_NULL;
int __attribute__( ( space( eedata ) ) ) EEmeterName19 = CHAR_NULL;


int __attribute__( ( space( eedata ) ) ) EEpassword0 = '1';
int __attribute__( ( space( eedata ) ) ) EEpassword1 = '2';
int __attribute__( ( space( eedata ) ) ) EEpassword2 = '3';
int __attribute__( ( space( eedata ) ) ) EEpassword3 = '4';
int __attribute__( ( space( eedata ) ) ) EEpassword4 = '1';
int __attribute__( ( space( eedata ) ) ) EEpassword5 = '2';
int __attribute__( ( space( eedata ) ) ) EEpowerAlloc = 500;
int __attribute__( ( space( eedata ) ) ) EEemerButton = 0;
int __attribute__( ( space( eedata ) ) ) EEresetTime = 1200;
int __attribute__( ( space( eedata ) ) ) EEaudibleAlarm = 0;
int __attribute__( ( space( eedata ) ) ) EEalarmOnePower = 0;
int __attribute__( ( space( eedata ) ) ) EEalarmTwoPower = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyLastResetH = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyLastResetL = 0;
int __attribute__( ( space( eedata ) ) ) EEpreviousCycleUsedH = 0;
int __attribute__( ( space( eedata ) ) ) EEpreviousCycleUsedL = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime1H = 0xFF;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime1L = 0xFF;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime2H = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime2L = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime3H = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime3L = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime4H = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime4L = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime5H = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime5L = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime6H = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime6L = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime7H = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime7L = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime8H = 0;
int __attribute__( ( space( eedata ) ) ) EEenergyUsedLifetime8L = 0;
int __attribute__( ( space( eedata ) ) ) EErelayMode = 2;
int __attribute__( ( space( eedata ) ) ) EErtccIsSet = 0;

int __attribute__( ( space( eedata ) ) ) EEcalibrationFactor1H = 0;         // default high byte is 0
int __attribute__( ( space( eedata ) ) ) EEcalibrationFactor1L = 22124;     // default value
int __attribute__( ( space( eedata ) ) ) EEcalibrationFactor2H = 0;         // default high byte is 0
int __attribute__( ( space( eedata ) ) ) EEcalibrationFactor2L = 1382;      // default is cal1 / 16
//  the default values for calibration factor are likely wrong - but they give a starting place


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

/*******************/

/*******************/
void eeReadMeterNameNew( char* meterName )
{
	unsigned int offset;

	writeWait( );

	TBLPAG = __builtin_tblpage( &EEmeterName00 );
	offset = __builtin_tbloffset( &EEmeterName00 );
	meterName[0] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName01 );
	offset = __builtin_tbloffset( &EEmeterName01 );
	meterName[1] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName02 );
	offset = __builtin_tbloffset( &EEmeterName02 );
	meterName[2] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName03 );
	offset = __builtin_tbloffset( &EEmeterName03 );
	meterName[3] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName04 );
	offset = __builtin_tbloffset( &EEmeterName04 );
	meterName[4] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName05 );
	offset = __builtin_tbloffset( &EEmeterName05 );
	meterName[5] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName06 );
	offset = __builtin_tbloffset( &EEmeterName06 );
	meterName[6] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName07 );
	offset = __builtin_tbloffset( &EEmeterName07 );
	meterName[7] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName08 );
	offset = __builtin_tbloffset( &EEmeterName08 );
	meterName[8] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName09 );
	offset = __builtin_tbloffset( &EEmeterName09 );
	meterName[9] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName10 );
	offset = __builtin_tbloffset( &EEmeterName10 );
	meterName[10] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName11 );
	offset = __builtin_tbloffset( &EEmeterName11 );
	meterName[11] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName12 );
	offset = __builtin_tbloffset( &EEmeterName12 );
	meterName[12] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName13 );
	offset = __builtin_tbloffset( &EEmeterName13 );
	meterName[13] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName14 );
	offset = __builtin_tbloffset( &EEmeterName14 );
	meterName[14] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName15 );
	offset = __builtin_tbloffset( &EEmeterName15 );
	meterName[15] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName16 );
	offset = __builtin_tbloffset( &EEmeterName16 );
	meterName[16] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName17 );
	offset = __builtin_tbloffset( &EEmeterName17 );
	meterName[17] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName18 );
	offset = __builtin_tbloffset( &EEmeterName18 );
	meterName[18] = (char) __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEmeterName19 );
	offset = __builtin_tbloffset( &EEmeterName19 );
	meterName[19] = (char) __builtin_tblrdl( offset );

	return;
}

void eeWriteMeterNameNew( char *meterName )
{
	unsigned int offset;

	// char 00
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName00 );
	offset = __builtin_tbloffset( &EEmeterName00 );
	__builtin_tblwtl( offset, meterName[0] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 01
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName01 );
	offset = __builtin_tbloffset( &EEmeterName01 );
	__builtin_tblwtl( offset, meterName[1] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 02
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName02 );
	offset = __builtin_tbloffset( &EEmeterName02 );
	__builtin_tblwtl( offset, meterName[2] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 03
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName03 );
	offset = __builtin_tbloffset( &EEmeterName03 );
	__builtin_tblwtl( offset, meterName[3] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 04
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName04 );
	offset = __builtin_tbloffset( &EEmeterName04 );
	__builtin_tblwtl( offset, meterName[4] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 05
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName05 );
	offset = __builtin_tbloffset( &EEmeterName05 );
	__builtin_tblwtl( offset, meterName[5] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 06
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName06 );
	offset = __builtin_tbloffset( &EEmeterName06 );
	__builtin_tblwtl( offset, meterName[6] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 07
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName07 );
	offset = __builtin_tbloffset( &EEmeterName07 );
	__builtin_tblwtl( offset, meterName[7] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 08
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName08 );
	offset = __builtin_tbloffset( &EEmeterName08 );
	__builtin_tblwtl( offset, meterName[8] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 09
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName09 );
	offset = __builtin_tbloffset( &EEmeterName09 );
	__builtin_tblwtl( offset, meterName[9] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 10
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName10 );
	offset = __builtin_tbloffset( &EEmeterName10 );
	__builtin_tblwtl( offset, meterName[10] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 11
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName11 );
	offset = __builtin_tbloffset( &EEmeterName11 );
	__builtin_tblwtl( offset, meterName[11] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 12
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName12 );
	offset = __builtin_tbloffset( &EEmeterName12 );
	__builtin_tblwtl( offset, meterName[12] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 13
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName13 );
	offset = __builtin_tbloffset( &EEmeterName13 );
	__builtin_tblwtl( offset, meterName[13] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 14
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName14 );
	offset = __builtin_tbloffset( &EEmeterName14 );
	__builtin_tblwtl( offset, meterName[14] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 15
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName15 );
	offset = __builtin_tbloffset( &EEmeterName15 );
	__builtin_tblwtl( offset, meterName[15] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 16
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName16 );
	offset = __builtin_tbloffset( &EEmeterName16 );
	__builtin_tblwtl( offset, meterName[16] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 17
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName17 );
	offset = __builtin_tbloffset( &EEmeterName17 );
	__builtin_tblwtl( offset, meterName[17] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 18
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName18 );
	offset = __builtin_tbloffset( &EEmeterName18 );
	__builtin_tblwtl( offset, meterName[18] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 19
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEmeterName19 );
	offset = __builtin_tbloffset( &EEmeterName19 );
	__builtin_tblwtl( offset, meterName[19] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );


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

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 1
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEpassword1 );
	offset = __builtin_tbloffset( &EEpassword1 );
	__builtin_tblwtl( offset, password[1] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 2
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEpassword2 );
	offset = __builtin_tbloffset( &EEpassword2 );
	__builtin_tblwtl( offset, password[2] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 3
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEpassword3 );
	offset = __builtin_tbloffset( &EEpassword3 );
	__builtin_tblwtl( offset, password[3] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 4
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEpassword4 );
	offset = __builtin_tbloffset( &EEpassword4 );
	__builtin_tblwtl( offset, password[4] );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	// char 5
	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEpassword5 );
	offset = __builtin_tbloffset( &EEpassword5 );
	__builtin_tblwtl( offset, password[5] );

	asm volatile ("disi #5" );
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

	asm volatile ("disi #5" );
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

	asm volatile ("disi #5" );
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

	tempResetTime = resetMinute + ( 100 * resetHour );

	TBLPAG = __builtin_tblpage( &EEresetTime );
	offset = __builtin_tbloffset( &EEresetTime );
	__builtin_tblwtl( offset, tempResetTime );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	return;
}

struct alarm_info_struct eeReadAlarmNew( void )
{
	unsigned int offset;

	struct alarm_info_struct alarmInfoTemp;

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

	alarmInfoTemp.alarm1PercentThreshold = alarm1Energy;

	alarmInfoTemp.alarm2PercentThreshold = alarm2Energy;

	return alarmInfoTemp;
}

void eeWriteAlarmNew( struct alarm_info_struct alarms )
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

	asm volatile ("disi #5" );
	__builtin_write_NVM( );


	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEalarmOnePower );
	offset = __builtin_tbloffset( &EEalarmOnePower );
	__builtin_tblwtl( offset, alarms.alarm1PercentThreshold );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );


	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEalarmTwoPower );
	offset = __builtin_tbloffset( &EEalarmTwoPower );
	__builtin_tblwtl( offset, alarms.alarm2PercentThreshold );

	asm volatile ("disi #5" );
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

	asm volatile ("disi #5" );
	__builtin_write_NVM( );


	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEenergyLastResetL );
	offset = __builtin_tbloffset( &EEenergyLastResetL );
	__builtin_tblwtl( offset, tempL );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );


	separateLongToInts( energyData.previousCycleUsed, &tempH, &tempL );

	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEpreviousCycleUsedH );
	offset = __builtin_tbloffset( &EEpreviousCycleUsedH );
	__builtin_tblwtl( offset, tempH );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );


	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEpreviousCycleUsedL );
	offset = __builtin_tbloffset( &EEpreviousCycleUsedL );
	__builtin_tblwtl( offset, tempL );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );

	return;
}

struct calibration_struct eeReadCalibrationNew( void )
{
	unsigned int offset;
    
    struct calibration_struct calibrationFactors;
    

	unsigned int tempH;
	unsigned int tempL;

	writeWait( );

	TBLPAG = __builtin_tblpage( &EEcalibrationFactor1H );
	offset = __builtin_tbloffset( &EEcalibrationFactor1H );
	tempH = __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEcalibrationFactor1L );
	offset = __builtin_tbloffset( &EEcalibrationFactor1L );
	tempL = __builtin_tblrdl( offset );

	calibrationFactors.cal1 = combineIntsToLong( tempH, tempL );
    

	TBLPAG = __builtin_tblpage( &EEcalibrationFactor2H );
	offset = __builtin_tbloffset( &EEcalibrationFactor2H );
	tempH = __builtin_tblrdl( offset );

	TBLPAG = __builtin_tblpage( &EEcalibrationFactor2L );
	offset = __builtin_tbloffset( &EEcalibrationFactor2L );
	tempL = __builtin_tblrdl( offset );

	calibrationFactors.cal2 = combineIntsToLong( tempH, tempL );

	return calibrationFactors;
}

void eeWriteCalibrationNew( struct calibration_struct calibrationFactors )
{
	unsigned int offset;

	unsigned int tempH;
	unsigned int tempL;


	separateLongToInts( calibrationFactors.cal1, &tempH, &tempL );

	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEcalibrationFactor1H );
	offset = __builtin_tbloffset( &EEcalibrationFactor1H );
	__builtin_tblwtl( offset, tempH );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );


	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEcalibrationFactor1L );
	offset = __builtin_tbloffset( &EEcalibrationFactor1L );
	__builtin_tblwtl( offset, tempL );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );


	separateLongToInts( calibrationFactors.cal2, &tempH, &tempL );

	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEcalibrationFactor2H );
	offset = __builtin_tbloffset( &EEcalibrationFactor2H );
	__builtin_tblwtl( offset, tempH );

	asm volatile ("disi #5" );
	__builtin_write_NVM( );


	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EEcalibrationFactor2L );
	offset = __builtin_tbloffset( &EEcalibrationFactor2L );
	__builtin_tblwtl( offset, tempL );

	asm volatile ("disi #5" );
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

	tempH = 0;
	tempL = 0;

	writeWait( ); // this might not need to be here, but it doesn't hurt if it is here

	EEpowerSelect = findSlotToRead( );

	switch( EEpowerSelect )
	{
		case 0:
			tempH = 0;
			tempL = 0;
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

	unsigned long energyLifetimeStored;

	energyLifetimeStored = eeReadEnergyUsedNew( );

	if( energyLifetime != energyLifetimeStored )
	{
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
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime1L );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime1L );
				__builtin_tblwtl( offset, tempL );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime2H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime2H );
				__builtin_tblwtl( offset, 0xFFFF );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );
				break;

			case 2:
				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime2H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime2H );
				__builtin_tblwtl( offset, tempH );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime2L );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime2L );
				__builtin_tblwtl( offset, tempL );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime3H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime3H );
				__builtin_tblwtl( offset, 0xFFFF );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );
				break;

			case 3:
				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime3H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime3H );
				__builtin_tblwtl( offset, tempH );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime3L );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime3L );
				__builtin_tblwtl( offset, tempL );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime4H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime4H );
				__builtin_tblwtl( offset, 0xFFFF );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );
				break;

			case 4:

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime4H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime4H );
				__builtin_tblwtl( offset, tempH );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime4L );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime4L );
				__builtin_tblwtl( offset, tempL );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime5H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime5H );
				__builtin_tblwtl( offset, 0xFFFF );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );
				break;

			case 5:
				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime5H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime5H );
				__builtin_tblwtl( offset, tempH );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime5L );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime5L );
				__builtin_tblwtl( offset, tempL );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime6H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime6H );
				__builtin_tblwtl( offset, 0xFFFF );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );
				break;

			case 6:
				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime6H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime6H );
				__builtin_tblwtl( offset, tempH );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime6L );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime6L );
				__builtin_tblwtl( offset, tempL );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime7H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime7H );
				__builtin_tblwtl( offset, 0xFFFF );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );
				break;

			case 7:
				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime7H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime7H );
				__builtin_tblwtl( offset, tempH );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime7L );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime7L );
				__builtin_tblwtl( offset, tempL );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime8H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime8H );
				__builtin_tblwtl( offset, 0xFFFF );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );
				break;

			case 8:
				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime8H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime8H );
				__builtin_tblwtl( offset, tempH );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime8L );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime8L );
				__builtin_tblwtl( offset, tempL );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );

				writeWait( );

				NVMCON = 0x4004;
				TBLPAG = __builtin_tblpage( &EEenergyUsedLifetime1H );
				offset = __builtin_tbloffset( &EEenergyUsedLifetime1H );
				__builtin_tblwtl( offset, 0xFFFF );
				asm volatile ("disi #5" );
				__builtin_write_NVM( );
		}

	}

	return;
}

unsigned int eeReadRelayNew( )
{

	unsigned int offset;

	unsigned int tempRelayMode;

	writeWait( );

	TBLPAG = __builtin_tblpage( &EErelayMode );
	offset = __builtin_tbloffset( &EErelayMode );
	tempRelayMode = __builtin_tblrdl( offset );

	return tempRelayMode;
}

void eeWriteRelayNew( unsigned int relayMode )
{
	// TODO non critical - figure out variable passing
	// passing in the variable relayMode does not work
	// the local relayMode variable does not contain the expected value
	// as a result, we use the global relayMode_global for now
	// it would be nice to figure out what is wrong, but that is beyond reach at the moment
    // potentially could have been due to large program size (overusing filling PIC memory)

	unsigned int offset;

	writeWait( );

	NVMCON = 0x4004;

	TBLPAG = __builtin_tblpage( &EErelayMode );
	offset = __builtin_tbloffset( &EErelayMode );
	__builtin_tblwtl( offset, relayMode_global );

	asm volatile ("disi #5" );
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

	asm volatile ("disi #5" );
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
	temp = ( temp % 8 ) + 1;

	return temp;
}

unsigned long combineIntsToLong( unsigned int intH, unsigned int intL )
{

	// combine High and Low integers into a long

	unsigned long temp;

	temp = ( ( (unsigned long) intH ) << 16 ) + intL;

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
