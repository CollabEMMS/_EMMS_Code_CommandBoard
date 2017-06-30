/* File:    ExternPowerDefinitions.h
 * Authors: Dan Baker
 *          Nathan Chaney
 */

#ifndef EXTERNPOWERDEFINITIONS_H
#define	EXTERNPOWERDEFINITIONS_H

#include "GenericTypeDefs.h"
//#include <stdint.h>

/* PowerMain.c ****************************************************************/
#define FOSC       32000000
#define _XTAL_FREQ 32000000

extern unsigned char resetHour;
extern unsigned char resetMinute;
extern unsigned long highAlloc;
extern unsigned long lowAlloc;
extern char isHigh;
extern char relayActive;

void init(void);
void initVars(void);
void setClocks(void);
void syncClocks(void);
void initPorts(void);
void enablePullDownResistors(void);
void enableInterrupts(void);
void disableInterrupts(void);
void readButton(void);
void relayControl(void);
void storeToEE(void);
void setHighLow(void);

/* OC_PWM.c *******************************************************************/
void initOC_PWM(void);
void updateLEDs(void);

/* I2C_RTCC.c *****************************************************************/
void initI2C(void);
void readI2CTime(unsigned char* year, unsigned char* month, unsigned char* day,
        unsigned char* hour, unsigned char* minute, unsigned char* second);
void setI2CTime(unsigned char year, unsigned char month, unsigned char day,
        unsigned char hour, unsigned char minute, unsigned char second);
void startClock(void);
void readI2CPowerTimes(void);
char BcdToDec(char val);
char DecToBcd(char val);

char BcdToDecI2C(char val);
char DecToBcdI2C(char val);

/* EEPROM.c *******************************************************************/
extern int __attribute__ ((space(eedata))) EEpassword0;
extern int __attribute__ ((space(eedata))) EEpassword1;
extern int __attribute__ ((space(eedata))) EEpassword2;
extern int __attribute__ ((space(eedata))) EEpassword3;
extern int __attribute__ ((space(eedata))) EEpassword4;
extern int __attribute__ ((space(eedata))) EEpassword5;
extern int __attribute__ ((space(eedata))) EEpowerAlloc;
//extern int __attribute__ ((space(eedata))) EEyear;
//extern int __attribute__ ((space(eedata))) EEmonth;
//extern int __attribute__ ((space(eedata))) EEday;
extern int __attribute__ ((space(eedata))) EEemerButton;
extern int __attribute__ ((space(eedata))) EEresetTime;
extern int __attribute__ ((space(eedata))) EEaudibleAlarm;
extern int __attribute__ ((space(eedata))) EEalarmOnePower;
extern int __attribute__ ((space(eedata))) EEalarmTwoPower;
extern int __attribute__ ((space(eedata))) EEtotalUsedH;
extern int __attribute__ ((space(eedata))) EEtotalUsedL;
extern int __attribute__ ((space(eedata))) EEpreviousDayUsedH;
extern int __attribute__ ((space(eedata))) EEpreviousDayUsedL;
extern int __attribute__ ((space(eedata))) EEpowerUsed1H;
extern int __attribute__ ((space(eedata))) EEpowerUsed1L;
extern int __attribute__ ((space(eedata))) EEpowerUsed2H;
extern int __attribute__ ((space(eedata))) EEpowerUsed2L;
extern int __attribute__ ((space(eedata))) EEpowerUsed3H;
extern int __attribute__ ((space(eedata))) EEpowerUsed3L;
extern int __attribute__ ((space(eedata))) EEpowerUsed4H;
extern int __attribute__ ((space(eedata))) EEpowerUsed4L;
extern int __attribute__ ((space(eedata))) EEpowerUsed5H;
extern int __attribute__ ((space(eedata))) EEpowerUsed5L;
extern int __attribute__ ((space(eedata))) EEpowerUsed6H;
extern int __attribute__ ((space(eedata))) EEpowerUsed6L;
extern int __attribute__ ((space(eedata))) EEpowerUsed7H;
extern int __attribute__ ((space(eedata))) EEpowerUsed7L;
extern int __attribute__ ((space(eedata))) EEpowerUsed8H;
extern int __attribute__ ((space(eedata))) EEpowerUsed8L;
extern int __attribute__ ((space(eedata))) EEhighLow;
extern int __attribute__ ((space(eedata))) EErelay;
extern char passwordSet[6];
extern char emerButtonEnable;
extern int emerButtonAlloc;
extern unsigned char resetHour;
extern unsigned char resetMinute;
extern char audibleAlarm;
extern char  alarmOnePower;
extern char  alarmTwoPower;
extern char  alarmOneEnabled;
extern char  alarmTwoEnabled;
extern char isHigh;
extern char relayActive;

void EEreadPassword(void);
void EEwritePassword(void);
void EEreadPowerAlloc(void);
void EEwritePowerAlloc(void);
//void EEreadDate(void);
//void EEwriteDate(void);
void EEreadEmerButton(void);
void EEwriteEmerButton(void);
void EEreadResetTime(void);
void EEwriteResetTime(void);
void EEreadAlarm(void);
void EEwriteAlarm(void);
void EEreadTotals(void);
void EEwriteTotals(void);
void EEreadPowerUsed(void);
void EEwritePowerUsed(void);
void EEreadHL(void);
void EEwriteHL(void);
void EEreadRelay(void);
void EEwriteRelay(void);
extern unsigned char findPowerToRead(void);
extern unsigned char findPowerToWrite(void);
void EEreadAll(void);


/* Power.c ********************************************************************/

extern char relayActive;



#endif