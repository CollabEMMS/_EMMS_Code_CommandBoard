/* File:    Delays.c
 * Authors: Dan Baker
 *          Nathan Chaney
 */

/* Includes *******************************************************************/
#include "ExternSharedDefinitions.h"



/* Functions ******************************************************************/

/* delayFourUS
 * delays for approximately four microseconds per input
 * parameter a: delays for a * 4 microseconds
 */
void delayFourUS(unsigned int a) {
    unsigned int i;

    for (i = 0; i < a; i++);
}

/* delayMS
 * delays for approximately one millisecond per input
 * paramter a: delays for a milliseconds
 */
void delayMS(unsigned int a) {
    unsigned int i, j;

    for (i = 0; i < a; i++)
        for (j = 0; j < 250; j++); // delay for 1ms
}

void commDelay(unsigned int a) {
    unsigned int i;

    for(i = 0; i < a; i++)
        commFunctions();
}