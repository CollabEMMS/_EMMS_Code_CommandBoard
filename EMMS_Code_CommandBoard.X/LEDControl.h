/* 
 * File:   LEDControl.h
 * Author: Austin
 *
 * Created on June 2, 2019, 7:02 AM
 */

#ifndef LEDControl_H
#    define	LEDControl_H


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

extern bool ledFindMeActive_global; // used to run the lights in a pattern for unit identification  SET through communication commands


/****************
 FUNCTION PROTOTYPES
 only include functions required by external c files
 ideally these are in the same order as in the code listing
 any functions used internally and externally must have the prototype in both the c and h files and should be marked
 
 *****************/

// external and internal
void ledSetAll( int led1Value, int led2Value, int led3Value, int led4Value );
void ledSetAllOn( void );
void ledSetAllOff( void );


// letTest functions work only in led debug mode (see common.h)


// external only
void ledInit( void );

void ledRunUp( int ledRunDelay );
void ledRunDown( int ledRunDelay );
void ledRun( int ledRunDelay );
void ledSetFrontEnergyRemain( void );
void ledSetFrontFindMe( void );

void ledTestSetOn( int ledNum );
void ledTestSetOff( int ledNum );
void ledTestSetAll( int led1Value, int led2Value, int led3Value, int led4Value );
void ledTestSetAllOn( void );
void ledTestSetAllOff( void );
void ledTestToggle( int ledNum );
void ledTestShowChar( char showMe );
void ledTestShowIntH( int showMe );
void ledTestShowIntL( int showMe );

/***************
NOTE
 
 DO NOT use regular LED functions for debugging purposes
 Use the "Test" functions for debug purposes
 The "Test" functions can be enabled and disabled using the LEDS_FOR_DEBUG macro found in common.h
 
 Using regular LED functions for debugging purposes will break the Bar Graph and can make future debugging difficult
 
 
 
 *****************/


#endif	/* LEDControl_H */

