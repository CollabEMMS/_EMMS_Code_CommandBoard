/* 
 * File:   MasterComm.h
 * Author: Austin
 *
 * Created on June 1, 2019, 12:30 PM
 */

#ifndef MASTERCOMM_H
#define	MASTERCOMM_H

extern long tba_energyAllocation;
extern long tba_energyUsedLifetime;
extern long tba_energyUsedLastDayReset;
extern long tba_powerWatts;
extern long tba_energyUsedPreviousDay;

extern char audibleAlarm;
extern char alarm1Enabled;
extern char alarm2Enabled;
extern char alarm1Energy;
extern char alarm2Energy;

extern	    int emerAllocation;


void communications( bool init);
void SPIMasterInit(void);

#endif	/* MASTERCOMM_H */

