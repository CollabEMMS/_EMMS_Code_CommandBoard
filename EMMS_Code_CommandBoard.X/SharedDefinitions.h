/* File:    SharedDefinitions.h
 * Authors: Dan Baker
 *          Nathan Chaney
 */

#ifndef SHAREDDEFINITIONS_H
#define	SHAREDDEFINITIONS_H

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
int stringLength(char* string);
void stringConcat(char* destination, char* source);
void zeroPad(char* target, int length);
char* sendBuffer(void);
char* receiveBuffer(void);

char lastSendSec;
char sendBuffer1[SEND_STRING_LENGTH];
char sendBuffer2[SEND_STRING_LENGTH];
char sendBuffer3[SEND_STRING_LENGTH];
char sendBuffer4[SEND_STRING_LENGTH];
char sendBuffer5[SEND_STRING_LENGTH];
char sendBlackHole[SEND_STRING_LENGTH];
char receivedToProcess[RECEIVE_STRING_LENGTH];
char receiveBuffer1[RECEIVE_STRING_LENGTH];
char receiveBuffer2[RECEIVE_STRING_LENGTH];
char receiveBuffer3[RECEIVE_STRING_LENGTH];
char receiveBuffer4[RECEIVE_STRING_LENGTH];
char receiveBuffer5[RECEIVE_STRING_LENGTH];
char receiveBlackHole[RECEIVE_STRING_LENGTH];
char receivedCommand[RECEIVE_PARAM_LENGTH];
char receivedAttribute[RECEIVE_PARAM_LENGTH];
char receivedValue[RECEIVE_PARAM_LENGTH];
char sendBuffer1Used;
char sendBuffer2Used;
char sendBuffer3Used;
char sendBuffer4Used;
char sendBuffer5Used;
char receiveBuffer1Used;
char receiveBuffer2Used;
char receiveBuffer3Used;
char receiveBuffer4Used;
char receiveBuffer5Used;
char readyToProcess;
char newSendData;
char newReceiveData;
unsigned char commError;
unsigned char commErrorTime;
int emerAllocNow;
int emerAllocSend;
char powerDownTime[12];
char powerUpTime[12];


/* Communication Low Level - Display Box and Power Box ************************/
char newSendDataU2;
short sendIndexU2;
short receiveIndexU2;
char stringToSendU2[SEND_STRING_LENGTH];
char receiveBufferU2[RECEIVE_STRING_LENGTH];

/* Communication Low Level - Power Box Only ***********************************/
char newSendDataU1;
short sendIndexU1;
short receiveIndexU1;
char stringToSendU1[SEND_STRING_LENGTH];
char receiveBufferU1[RECEIVE_STRING_LENGTH];

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

void readTimeI2C(void);
void writeClockStrings(void);

unsigned char timeYear;
unsigned char timeMonth;
unsigned char timeDay;
unsigned char timeWeekday;
unsigned char timeHour;
unsigned char timeMinute;
unsigned char timeSecond;

char tempHour;
char tempMin;
char tempMonth;
char tempDay;
char tempYear;


/* Delays.c *******************************************************************/

void delayFourUS(unsigned int);
void delayMS(unsigned int);
void commDelay(unsigned int);


/* UART.c *********************************************************************/

void initUART(void);

/* Power.c ********************************************************************/


/* Variable Definitions *******************************************************/
char buffer1[12];
char buffer2[12];
char buffer3[12];
char buffer4[12];
char buffer5[12];
char buffer6[12];
char buffer7[12];
char buffer8[12];
char audibleAlarm;
char alarmOneEnabled;
char alarmTwoEnabled;
char alarmOnePower;
char alarmTwoPower;
char passwordSet[6];
char emerButtonEnable;
int emerButtonAlloc;
unsigned long powerBoxCodeVersion;
unsigned long displayBoxCodeVersion;
char powerBoxCodeVersionString[9];
unsigned char resetHour;
unsigned char resetMinute;
char relayActive;
char isHigh;


///* PowerSurvive.c *************************************************************/
//void initPowerLossDetection(void);

/* Watchdog.c *****************************************************************/
//void resetWDT(void);
#endif