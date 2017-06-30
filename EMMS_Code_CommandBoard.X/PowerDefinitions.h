/* File:    PowerDefinitions.h
 * Authors: Dan Baker
 *          Nathan Chaney
 */

#ifndef POWERDEFINITIONS_H
#define	POWERDEFINITIONS_H

#include "GenericTypeDefs.h"

/* Get system clock time to program PIC ***************************************/

// Build date: __DATE__ -> "Mar  2 2015"
// Build time: __TIME__ -> "14:05:00"
#define BUILD_YEAR  (((__DATE__ [9] - 48) * 10) + (__DATE__ [10] - 48))
#define BUILD_MONTH (\
  __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
: __DATE__ [2] == 'b' ? 2 \
: __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
: __DATE__ [2] == 'y' ? 5 \
: __DATE__ [2] == 'l' ? 7 \
: __DATE__ [2] == 'g' ? 8 \
: __DATE__ [2] == 'p' ? 9 \
: __DATE__ [2] == 't' ? 10 \
: __DATE__ [2] == 'v' ? 11 \
: 12)

#define BUILD_DAY (__DATE__ [4] == ' ' ? (__DATE__ [5] - 48) \
: ((__DATE__ [4] - 48) * 10) + (__DATE__ [5] - 48))

#define BUILD_HOUR   (((__TIME__ [0] - 48) * 10) + (__TIME__ [1] - 48))
#define BUILD_MINUTE (((__TIME__ [3] - 48) * 10) + (__TIME__ [4] - 48))
#define BUILD_SECOND (((__TIME__ [6] - 48) * 10) + (__TIME__ [7] - 48))

#define CODE_REVISION 1

// YYMMDDVV

//#define POWER_BOX_CODE_VERSION ((char[]) {__DATE__ [9], __DATE__ [10], (BUILD_MONTH / 10) + 48, (BUILD_MONTH % 10) + 48, \
//    (__DATE__ [4] == ' ' ? '0' : __DATE__ [4]), __DATE__ [5], (CODE_REVISION / 10) + 48, (CODE_REVISION % 10) + 48, 0})

#define POWER_BOX_CODE_VERSION "170610a"

/******************************************************************************/
/* PowerMain.c ****************************************************************/
#define FOSC       32000000
#define _XTAL_FREQ 32000000

unsigned char resetHour;
unsigned char resetMinute;
unsigned long highAlloc;
unsigned long lowAlloc;
char isHigh;
char relayActive;

extern unsigned char powerDownMinute;
extern unsigned char powerDownHour;
extern unsigned char powerDownDay;
extern unsigned char powerDownMonth;

extern unsigned char powerUpMinute;
extern unsigned char powerUpHour;
extern unsigned char powerUpDay;
extern unsigned char powerUpMonth;


void init(void);
void initVars(void);
void setClock(void);
//void dailyResetPowerOnCheck(void);
void dailyResetCheck(void);
void dailyReset(void);
void initPorts(void);
void enablePullDownResistors(void);
void enableInterrupts(void);
void disableInterrupts(void);
void readButton(void);
void relayControl(void);
void storeToEE(void);
void setHighLow(void);

/* OC_PWM.c *******************************************************************/
//void initOC_PWM(void);
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

/* EEPROM.c *******************************************************************/

int __attribute__ ((space(eedata))) EEpassword0 = '1';
int __attribute__ ((space(eedata))) EEpassword1 = '2';
int __attribute__ ((space(eedata))) EEpassword2 = '3';
int __attribute__ ((space(eedata))) EEpassword3 = '4';
int __attribute__ ((space(eedata))) EEpassword4 = '1';
int __attribute__ ((space(eedata))) EEpassword5 = '2';
int __attribute__ ((space(eedata))) EEpowerAlloc = 50;
//int __attribute__ ((space(eedata))) EEyear = 14;
//int __attribute__ ((space(eedata))) EEmonth = 1;
//int __attribute__ ((space(eedata))) EEday = 1;
int __attribute__ ((space(eedata))) EEemerButton = 0;
int __attribute__ ((space(eedata))) EEresetTime = 700;
int __attribute__ ((space(eedata))) EEaudibleAlarm = 0;
int __attribute__ ((space(eedata))) EEalarmOnePower = 0;
int __attribute__ ((space(eedata))) EEalarmTwoPower = 0;
int __attribute__ ((space(eedata))) EEtotalUsedH = 0;
int __attribute__ ((space(eedata))) EEtotalUsedL = 0;
int __attribute__ ((space(eedata))) EEpreviousDayUsedH = 0;
int __attribute__ ((space(eedata))) EEpreviousDayUsedL = 0;
int __attribute__ ((space(eedata))) EEpowerUsed1H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed1L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed2H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed2L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed3H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed3L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed4H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed4L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed5H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed5L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed6H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed6L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed7H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed7L = 0;
int __attribute__ ((space(eedata))) EEpowerUsed8H = 0;
int __attribute__ ((space(eedata))) EEpowerUsed8L = 0;
int __attribute__ ((space(eedata))) EEhighLow = 0xFF;
int __attribute__ ((space(eedata))) EErelay = 0xFF;
char passwordSet[6];
char emerButtonEnable;
int emerButtonAlloc;
unsigned char resetHour;
unsigned char resetMinute;
char audibleAlarm;
char alarmOnePower;
char alarmTwoPower;
char alarmOneEnabled;
char alarmTwoEnabled;
char isHigh;
char relayActive;

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
unsigned char findPowerToRead(void);
unsigned char findPowerToWrite(void);
void EEreadAll(void);


/* Power.c ********************************************************************/


extern char relayActive;

#endif