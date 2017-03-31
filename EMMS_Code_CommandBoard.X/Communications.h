/* 
 * File:   Communications.h
 * Author: austin
 *
 * Created on February 21, 2016, 8:37 AM
 */

/*******************************
 This is an outline of how the new communications should fit within the existing program
 From the main loop it should be a simple function call
 
 ASSUMPTION
    the 'master' will never queue up messages to send to various modules. It will only react to what modules send to it.
    this will make it that we do not need to keep separate buffers for each possible port
  
  
 |
 |--- Periodic Updates
 |--- --- readTime
 |--- --- updateLEDs
 |--- --- dailyReset
 |--- --- zeroPower
 |--- --- relayControl
 |
 |--- commFunctions (uART)
 |
 |--- readButton
 |
 |--- storeToEE
 |
 |--- communications (still to be refined and updated as code is put together)
 |--- () read port (might be done with interrupt, undecided)
 |--- () process read
 |--- () send port
 |
 |
 | LOOP
 
 
 
 
 
 
 
 
 */




#ifndef COMMUNICATIONS_H
#define	COMMUNICATIONS_H
#endif

#include <stdbool.h>


bool communications(void);
void SPIMasterInit(void);

