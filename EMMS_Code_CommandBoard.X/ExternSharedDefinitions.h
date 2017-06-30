/* File:    ExternSharedDefinitions.h
 * Authors: Dan Baker
 *          Nathan Chaney
 */

#ifndef EXTERNSHAREDDEFINITIONS_H
#define	EXTERNSHAREDDEFINITIONS_H

#include <stdbool.h>

/* Communication Low Level ****************************************************/
#define SEND_STRING_LENGTH 40
#define RECEIVE_STRING_LENGTH 41
#define RECEIVE_PARAM_LENGTH 50

void commFunctions(void);
void newString(unsigned char word);
void flushBuffer(char* target, short length);
void sendCommands(void);
void processReceivedCommand(void);
void splitReceivedCommand(void);
void stringCopy(char* from, char* to);
char stringCompare(char* a, char* b);
int  stringLength(char* string);
void stringConcat(char* destination, char* source);
void zeroPad(char* target, int length);
char* sendBuffer(void);
char* receiveBuffer(void);
void commandBuilder1(char* command, char* attribute, char* value);

extern char lastSendSec;
extern char sendBuffer1[SEND_STRING_LENGTH];
extern char sendBuffer2[SEND_STRING_LENGTH];
extern char sendBuffer3[SEND_STRING_LENGTH];
extern char sendBuffer4[SEND_STRING_LENGTH];
extern char sendBuffer5[SEND_STRING_LENGTH];
extern char sendBlackHole[SEND_STRING_LENGTH];
extern char receivedToProcess[RECEIVE_STRING_LENGTH];
extern char receiveBuffer1[RECEIVE_STRING_LENGTH];
extern char receiveBuffer2[RECEIVE_STRING_LENGTH];
extern char receiveBuffer3[RECEIVE_STRING_LENGTH];
extern char receiveBuffer4[RECEIVE_STRING_LENGTH];
extern char receiveBuffer5[RECEIVE_STRING_LENGTH];
extern char receiveBlackHole[RECEIVE_STRING_LENGTH];
extern char receivedCommand[RECEIVE_PARAM_LENGTH];
extern char receivedAttribute[RECEIVE_PARAM_LENGTH];
extern char receivedValue[RECEIVE_PARAM_LENGTH];
extern char sendBuffer1Used;
extern char sendBuffer2Used;
extern char sendBuffer3Used;
extern char sendBuffer4Used;
extern char sendBuffer5Used;
extern char receiveBuffer1Used;
extern char receiveBuffer2Used;
extern char receiveBuffer3Used;
extern char receiveBuffer4Used;
extern char receiveBuffer5Used;
extern char readyToProcess;
extern char newSendData;
extern char newReceiveData;
extern unsigned char commError;
extern unsigned char commErrorTime;
extern int  emerAllocNow;
extern int  emerAllocSend;
extern char powerDownTime[12];
extern char powerUpTime[12];


/* Communication Low Level - Display Box and Power Box ************************/
extern char newSendDataU2;
extern short sendIndexU2;
extern short receiveIndexU2;
extern char stringToSendU2[SEND_STRING_LENGTH];
extern char receiveBufferU2[RECEIVE_STRING_LENGTH];

/* Communication Low Level - Power Box Only ***********************************/
extern char newSendDataU1;
extern short sendIndexU1;
extern short receiveIndexU1;
extern char stringToSendU1[SEND_STRING_LENGTH];
extern char receiveBufferU1[RECEIVE_STRING_LENGTH];

/* Communication High Level ***************************************************/
void setRemoteTime(void);
void readRemoteTime(void);
void setRemotePower(void);
void readRemotePower(void);
void setRemoteAlarm(void);
void readRemoteAlarm(void);
void setRemotePassword(void);
void readRemotePassword(void);
void setRemoteEmergency(void);
void readRemoteEmergency(void);
void setRemoteVersion(void);
void readRemoteVersion(void);
void setRemoteResetTime(void);
void readRemoteResetTime(void);
void setRemoteRelay(void);
void readRemoteRelay(void);
void setRemoteStats(void);
void readRemoteStats(void);
void setRemoteHL(void);
void readRemoteHL(void);
void setRemotePowerDownUpTime(void);
void readRemotePowerDownUpTime(void);
void readUpdate(void);
void doReset(void);

/* Communication High Level - Power Box Only **********************************/
void sendUpdate(void);

/* RTCC.c *********************************************************************/

void readTime(void);
void writeClockStrings(void);
bool writeTime(char newYear, char newMonth, char newDay, char newHour, char newMinute, char newSecond);

extern unsigned char timeYear, timeMonth, timeDay, timeWeekday,
        timeHour, timeMinute, timeSecond;

extern char tempHour, tempMin, tempMonth, tempDay, tempYear;

/* Delays.c *******************************************************************/

void delayFourUS(unsigned int);
void delayMS(unsigned int);

/* UART.c *********************************************************************/

void initUART(void);

/* Power.c ********************************************************************/
//extern unsigned long powerUsed;
//extern unsigned long powerAllocated;
//extern unsigned long currentLoad;

//extern unsigned long powerWatts;
//extern unsigned long powerVolts;
//extern unsigned long powerAmps;


/* Variable Definitions *******************************************************/
extern char buffer1[12];
extern char buffer2[12];
extern char buffer3[12];
extern char buffer4[12];
extern char buffer5[12];
extern char buffer6[12];
extern char buffer7[12];
extern char buffer8[12];
extern char audibleAlarm;
extern char alarmOneEnabled;
extern char alarmTwoEnabled;
extern char alarmOnePower;
extern char alarmTwoPower;
extern char passwordSet[6];
extern char emerButtonEnable;
extern int  emerButtonAlloc;
extern unsigned long powerBoxCodeVersion;
extern unsigned long displayBoxCodeVersion;
extern char powerBoxCodeVersionString[9];
extern unsigned char resetHour;
extern unsigned char resetMinute;
extern char relayActive;
extern char isHigh;


#endif