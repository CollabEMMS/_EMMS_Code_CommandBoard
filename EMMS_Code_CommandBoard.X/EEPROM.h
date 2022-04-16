#ifndef EEPROM_H
#    define	EEPROM_H


/****************
 MACROS
 these are the macros that are required by external c files
 do not include macros that are only used internally within this module
 ****************/

/****************
 VARIABLES
 these are the globals required by external c files
 there should be as few of these as possible to help keep things clean
 these are all 'extern' and require that the variable is declared in the c file
 ****************/


/****************
 FUNCTION PROTOTYPES
 only include functions required by external c files
 ideally these are in the same order as in the code listing
 any functions used internally and externally must have the prototype in both the c and h files and should be marked
 
 *****************/


// external and internal

// external only


//extern char passwordSet[6];

void eeReadMeterNameNew( char* meterName );
void eeWriteMeterNameNew( char *meterName );

void eeReadPasswordNew( char* password );
void eeWritePasswordNew( char* password );

long eeReadEnergyCycleAllocationNew( void );
void eeWriteEnergyAllocNew( long energyAllocation );

struct emergency_button eeReadEmergencyButtonNew( void );
void eeWriteEmerButtonNew( struct emergency_button emergencyButton );

struct alarm_info_struct eeReadAlarmNew( void );
void eeWriteAlarmNew( struct alarm_info_struct alarms );

struct reset_time eeReadResetTimeNew( void );
void eeWriteResetTimeNew( int resetHour, int resetMinute );

struct energy_info eeReadTotalsNew( void );
void eeWriteEnergyTotalsNew( struct energy_info energyData );

unsigned long eeReadEnergyUsedNew( );
void eeWriteEnergyLifetimeNew( unsigned long energyUsedLifetime );

struct calibration_struct eeReadCalibrationNew( void );
void eeWriteCalibrationNew( struct calibration_struct calibrationFactors );

unsigned int eeReadRelayNew( );
void eeWriteRelayNew( bool relay );

bool eeReadRTCCIsSet( );
void eeWriteRTCCIsSet( bool rtccIsSet );




#endif	/* EEPROM_H */

