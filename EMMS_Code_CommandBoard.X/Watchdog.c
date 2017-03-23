/* File:    Watchdog.c
 * Author: Nathan Chaney
 */

/* Includes *******************************************************************/

#include "../Shared/ExternSharedDefinitions.h"
#include <p24FV32KA301.h>

/* Functions ******************************************************************/

void resetWDT(void) {
    asm("CLRWDT");
}