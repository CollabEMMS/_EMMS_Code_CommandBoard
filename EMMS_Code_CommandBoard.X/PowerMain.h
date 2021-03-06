/* 
 * File:   PowerMain.h
 * Author: Austin
 *
 * Created on June 2, 2019, 6:52 AM
 */

#ifndef POWERMAIN_H
#define	POWERMAIN_H


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


extern struct energy_info energyUsed_global;
extern struct date_time dateTime_global;
extern struct alarm_info alarms_global;
extern int energyAdd_global;
extern unsigned long energyCycleAllocation_global;
extern struct reset_time resetTime_global;
extern bool relayActive_global;
extern struct emergency_button emergencyButton_global;

extern unsigned long powerWatts_global;

extern char powerBoxCodeVersion_global[9];


/****************
 FUNCTION PROTOTYPES
 only include functions required by external c files
 ideally these are in the same order as in the code listing
 any functions used internally and externally must have the prototype in both the c and h files and should be marked
 
 *****************/


// external and internal

// external only


#endif	/* POWERMAIN_H */

