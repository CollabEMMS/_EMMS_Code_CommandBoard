/* 
 * File:   PowerMain.h
 * Author: Austin
 *
 * Created on June 2, 2019, 6:52 AM
 */

#ifndef POWERMAIN_H
#define	POWERMAIN_H

extern char emerButtonEnable;
extern int emerButtonEnergyAllocate;
extern unsigned char resetTimeHour;
extern unsigned char resetTimeMinute;

extern char audibleAlarm;
extern char  alarm1Energy;
extern char  alarm2Energy;
extern char  alarm1Enabled;
extern char  alarm2Enabled;

extern char isHigh;
extern char relayActive;

extern bool lightsModeActive;

//FIX = this is clunky and may not work
extern  char powerBoxCodeVersion[9];

// internal and external
void debugLEDSet( int LEDNum, bool On );
void debugLEDRotateA( int minLED, int maxLED );
void debugLEDRotateB( int minLED, int maxLED );
void debugLEDToggle( int LEDNum );

#endif	/* POWERMAIN_H */

