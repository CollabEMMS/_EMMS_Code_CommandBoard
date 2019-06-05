/******
 Common Header File
 Only put something in here if:
    it is required for all C files
    it is not best included in any other header file
*/

#ifndef COMMON_H
#define	COMMON_H

//#define FOSC       32000000
//#define _XTAL_FREQ 32000000
// don't know if the above is actually needed, but keep for now so I don't lose it

//#include <xc.h>
#include <p24FV32KA302.h>
#include "GenericTypeDefs.h"
#include <stdbool.h>

#define RELAY       _RB6    // Pin 15: RB6
#define BTN_EMER    _RA0    // Pin 2:  RA0



// LEDs for power remaining indicator
// can also be used for testing
#define LEDS_FOR_TESTING false
// set this to 'true' to turn off the LED graph for testing purposes

#define LED1_DIR TRISAbits.TRISA2
#define LED2_DIR TRISAbits.TRISA3
#define LED3_DIR TRISBbits.TRISB4
#define LED4_DIR TRISAbits.TRISA4

#define LED1_SET LATAbits.LATA2
#define LED2_SET LATAbits.LATA3
#define LED3_SET LATBbits.LATB4
#define LED4_SET LATAbits.LATA4

#define LED1_READ PORTAbits.RA2
#define LED2_READ PORTAbits.RA3
#define LED3_READ PORTBbits.RB4
#define LED4_READ PORTAbits.RA4


#define CHAR_NULL '\0'


#endif	/* COMMON_H */










/******
 Below are the comments for the headers of C files
 This should be placed at the top of each c file to provide some sense and
   structure to make things easier to understand
 */

/****************
 INCLUDES
 only include the header files that are required
 ****************/

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

/****************
 CODE
 ****************/
