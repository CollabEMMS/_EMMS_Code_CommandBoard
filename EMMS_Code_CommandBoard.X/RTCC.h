#ifndef I2C_RTCC_H
#define	I2C_RTCC_H


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
void rtccCopyI2CTime( void );
void rtccInit( void);

void rtccI2CReadPowerTimes( struct date_time *timePowerFail, struct date_time *timePowerRestore );
void rtccI2CReadTime( struct date_time *readDateTime );
void rtccI2CSetTime( struct date_time *setDateTime );




// RTCC Internal
void rtccReadTime( struct date_time *readDateTime );



#endif	/* I2C_RTCC_H */

