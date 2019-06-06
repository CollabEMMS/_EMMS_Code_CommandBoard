/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include "common.h"

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

/* delayFourUS
 * delays for approximately four microseconds per input
 * parameter a: delays for a * 4 microseconds
 */
void delayFourUS( unsigned int a )

{
    unsigned int i;

    for( i = 0; i < a; i++ );
}

/* delayMS
 * delays for approximately one millisecond per input
 * parameter a: delays for a milliseconds
 */
void delayMS( unsigned int a )
{
    unsigned int i, j;

    for( i = 0; i < a; i++ )
	for( j = 0; j < 250; j++ ); // delay for 1ms
}

void delayMSTenths( unsigned int a )
{
    unsigned int i, j;

    for( i = 0; i < a; i++ )
	for( j = 0; j < 25; j++ ); // delay for 1ms
}


//void commDelay(unsigned int a) {
//    unsigned int i;
//
//    for(i = 0; i < a; i++)
//        commFunctions();
//}