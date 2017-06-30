/* File:    SharedCommunication.c
 * Authors: Dan Baker
 *          Nathan Chaney
 */

/* Ensure that SharedCommunication.c is excluded from the source build path!
 * SharedCommunication.c must be included LAST in your main source file
 * like this to ensure the compiler builds the correct version:
 *    #define COMM_INCLUDED
 *    #include "../Shared/SharedCommunication.c"
 */


/* Includes *******************************************************************/

#ifndef COMM_INCLUDED
#error Please add #define COMM_INCLUDED and #include "../Shared/SharedCommunication.c" to your main source and exclude SharedCommunication.c from the source build path.
#endif

#include <xc.h>
#include <stdlib.h>

#include "ExternSharedDefinitions.h"
#include "SharedDefinitions.h"

//#include "OC_PWM.c" // why do we include this here?


extern unsigned long tba_energyAllocation;
extern unsigned long tba_energyUsedLifetime;
extern unsigned long tba_energyUsedLastDayReset;
extern unsigned long tba_powerWatts;
extern unsigned long tba_energyUsedPreviousDay;

void commInit(void)
{

    sendIndexU1 = receiveIndexU1 = 0;
    newSendDataU1 = 0;

    sendIndexU2 = receiveIndexU2 = 0;
    newSendDataU2 = 0;
    newSendData = 0;

    sendBuffer1Used = sendBuffer2Used = sendBuffer3Used =
            sendBuffer4Used = sendBuffer5Used = 0;

    receiveBuffer1Used = receiveBuffer2Used = receiveBuffer3Used =
            receiveBuffer4Used = receiveBuffer5Used = 0;

}

void commFunctions(void)
{
    splitReceivedCommand();
    sendCommands();
}

void splitReceivedCommand(void)
{

    // promote buffers if there is data in one or more of them
    while (newReceiveData == 0 && (receiveBuffer1Used || receiveBuffer2Used ||
            receiveBuffer3Used || receiveBuffer4Used || receiveBuffer5Used))
    {

        if (newReceiveData == 0 && receiveBuffer1Used != 0)
        {
            stringCopy(receiveBuffer1, receivedToProcess);
            receiveBuffer1Used = 0;
            flushBuffer(receiveBuffer1, RECEIVE_STRING_LENGTH);
            newReceiveData = 1;
        }

        if (receiveBuffer1Used == 0 && receiveBuffer2Used != 0)
        {
            stringCopy(receiveBuffer2, receiveBuffer1);
            receiveBuffer2Used = 0;
            flushBuffer(receiveBuffer2, RECEIVE_STRING_LENGTH);
            receiveBuffer1Used = 1;
        }

        if (receiveBuffer2Used == 0 && receiveBuffer3Used != 0)
        {
            stringCopy(receiveBuffer3, receiveBuffer2);
            receiveBuffer3Used = 0;
            flushBuffer(receiveBuffer3, RECEIVE_STRING_LENGTH);
            receiveBuffer2Used = 1;
        }

        if (receiveBuffer3Used == 0 && receiveBuffer4Used != 0)
        {
            stringCopy(receiveBuffer4, receiveBuffer3);
            receiveBuffer4Used = 0;
            flushBuffer(receiveBuffer4, RECEIVE_STRING_LENGTH);
            receiveBuffer3Used = 1;
        }

        if (receiveBuffer4Used == 0 && receiveBuffer5Used != 0)
        {
            stringCopy(receiveBuffer5, receiveBuffer4);
            receiveBuffer5Used = 0;
            flushBuffer(receiveBuffer5, RECEIVE_STRING_LENGTH);
            receiveBuffer4Used = 1;
        }
    }

    // check for received information and split it up
    if (newReceiveData != 0 && readyToProcess == 0)
    {

        // Initialize receivedCommand, receivedAttribute, and receivedValue
        short k = 0;
        while (k < RECEIVE_PARAM_LENGTH)
        {
            receivedCommand[k] = receivedAttribute[k] = receivedValue[k] = 0;
            k++;
        }

        // Check for beginning of command. If not, abort.
        if (receivedToProcess[0] != '%')
        {
            newReceiveData = 0;
            return;
        }
            // Command start was present. Split string into components.
        else
        {
            short i = 1;
            short j = 0;

            // Extract receivedCommand
            while (receivedToProcess[i] > 47 && receivedToProcess[i] < 127 &&
                    j < RECEIVE_PARAM_LENGTH && i < RECEIVE_STRING_LENGTH - 1)
            { // RECEIVE_STRING_LENGTH - 1 because of check for '.' in the last step
                receivedCommand[j++] = receivedToProcess[i++];
            }
            // If delimeter is not next, something went wrong. Abort.
            if (receivedToProcess[i++] != ' ')
            {
                newReceiveData = 0;
                return;
            }

            // Extract receivedAttribute
            j = 0;
            while (receivedToProcess[i] > 47 && receivedToProcess[i] < 127 &&
                    j < RECEIVE_PARAM_LENGTH && i < RECEIVE_STRING_LENGTH - 1)
            {
                receivedAttribute[j++] = receivedToProcess[i++];
            }
            // If delimeter is not next, something went wrong. Abort.
            if (receivedToProcess[i++] != ' ')
            {
                newReceiveData = 0;
                return;
            }

            // Extract receivedValue
            j = 0;
            while (receivedToProcess[i] > 47 && receivedToProcess[i] < 127 &&
                    j < RECEIVE_PARAM_LENGTH && i < RECEIVE_STRING_LENGTH - 1)
            {
                receivedValue[j++] = receivedToProcess[i++];
            }
            // If end of command is not next, something went wrong. Abort.
            if (receivedToProcess[i] != '.')
            {
                newReceiveData = 0;
                return;
            }

            // Received string was in the right format
            // and has been split up successfully
            readyToProcess = 1;
            newReceiveData = 0;
            commError = 0;

            processReceivedCommand();
            flushBuffer(receivedToProcess, SEND_STRING_LENGTH);
        }
    }
}

void sendCommands(void)
{
    // Move data from sendBuffer1 into stringToSend if possible
    // and promote data in other sendBuffers
    if (newSendData == 0 && sendBuffer1Used != 0)
    {
        stringCopy(sendBuffer1, stringToSendU2);
        newSendDataU2 = 1;

        stringCopy(sendBuffer1, stringToSendU1);
        newSendDataU1 = 1;

        sendBuffer1Used = 0;
        flushBuffer(sendBuffer1, SEND_STRING_LENGTH);
        newSendData = 1;

    }

    if (sendBuffer1Used == 0 && sendBuffer2Used != 0)
    {
        stringCopy(sendBuffer2, sendBuffer1);
        sendBuffer2Used = 0;
        flushBuffer(sendBuffer2, SEND_STRING_LENGTH);
        sendBuffer1Used = 1;
    }

    if (sendBuffer2Used == 0 && sendBuffer3Used != 0)
    {
        stringCopy(sendBuffer3, sendBuffer2);
        sendBuffer3Used = 0;
        flushBuffer(sendBuffer3, SEND_STRING_LENGTH);
        sendBuffer2Used = 1;
    }

    if (sendBuffer3Used == 0 && sendBuffer4Used != 0)
    {
        stringCopy(sendBuffer4, sendBuffer3);
        sendBuffer4Used = 0;
        flushBuffer(sendBuffer4, SEND_STRING_LENGTH);
        sendBuffer3Used = 1;
    }

    if (sendBuffer4Used == 0 && sendBuffer5Used != 0)
    {
        stringCopy(sendBuffer5, sendBuffer4);
        sendBuffer5Used = 0;
        flushBuffer(sendBuffer5, SEND_STRING_LENGTH);
        sendBuffer4Used = 1;
    }

    /* Fill UART transmit buffers with characters.
     * When checking if we can send another character
     * UxSTAbits.UTXBF == 0 is not a valid check.
     * UTXBF doesn't always update correctly.
     * See PIC errata */
    while (U2STAbits.TRMT == 1 && stringToSendU2[sendIndexU2] != 0 && newSendDataU2 != 0)
    {
        U2TXREG = stringToSendU2[sendIndexU2++];

        if (stringToSendU2[sendIndexU2] == 0 || stringToSendU2[sendIndexU2 - 1] == '\n')
        {
            sendIndexU2 = 0;
            newSendDataU2 = 0;
        }
    }

    while (U1STAbits.TRMT == 1 && stringToSendU1[sendIndexU1] != 0 && newSendDataU1 != 0)
    {
        U1TXREG = stringToSendU1[sendIndexU1++];

        if (stringToSendU1[sendIndexU1] == 0 || stringToSendU1[sendIndexU1 - 1] == '\n')
        {
            sendIndexU1 = 0;
            newSendDataU1 = 0;
        }
    }
    if (newSendData != 0 && newSendDataU2 == 0 && newSendDataU1 == 0)
    {
        newSendData = 0;
    }

}

char* sendBuffer(void)
{
    if (sendBuffer1Used == 0)
    {
        sendBuffer1Used = 1;
        return sendBuffer1;
    }
    else if (sendBuffer2Used == 0)
    {
        sendBuffer2Used = 1;
        return sendBuffer2;
    }
    else if (sendBuffer3Used == 0)
    {
        sendBuffer3Used = 1;
        return sendBuffer3;
    }
    else if (sendBuffer4Used == 0)
    {
        sendBuffer4Used = 1;
        return sendBuffer4;
    }
    else if (sendBuffer5Used == 0)
    {
        sendBuffer5Used = 1;
        return sendBuffer5;
    }
        // no room in any buffer, throw data away
    else return sendBlackHole;
}

char* receiveBuffer(void)
{
    if (receiveBuffer1Used == 0)
    {
        receiveBuffer1Used = 1;
        return receiveBuffer1;
    }
    else if (receiveBuffer2Used == 0)
    {
        receiveBuffer2Used = 1;
        return receiveBuffer2;
    }
    else if (receiveBuffer3Used == 0)
    {
        receiveBuffer3Used = 1;
        return receiveBuffer3;
    }
    else if (receiveBuffer4Used == 0)
    {
        receiveBuffer4Used = 1;
        return receiveBuffer4;
    }
    else if (receiveBuffer5Used == 0)
    {
        receiveBuffer5Used = 1;
        return receiveBuffer5;
    }
        // no room in any buffer, throw data away
    else return receiveBlackHole;
}

void flushBuffer(char* target, short length)
{
    short k = 0;
    while (k < length)
        target[k++] = 0;
}

void stringCopy(char* from, char* to)
{
    int i;
    for (i = 0; from[i] != 0; i++)
    {
        to[i] = from[i];
    }
    to[i] = 0;
}

// concatenate the second string onto the first

void stringConcat(char* destination, char* source)
{
    int i = 0;
    int start = stringLength(destination);
    while (source[i] != 0)
    {
        destination[start + i] = source[i];
        i++;
    }
    destination[start + i] = 0;
}

// Returns 1 if strings differ, because that is
// the convention for comparison methods.

char stringCompare(char* a, char* b)
{
    short i = 0;
    while (a[i] != 0 && b[i] != 0)
    {
        if (a[i] != b[i])
            return 1;
        i++;
    }
    return 0;
}

int stringLength(char* string)
{
    int i = 0;
    while (string[i++] != 0);
    return i - 1;
}

void commandBuilder1(char* command, char* attribute, char* value)
{
    char newCommand[RECEIVE_PARAM_LENGTH] = "%";
    stringConcat(newCommand, command);
    stringConcat(newCommand, " ");
    stringConcat(newCommand, attribute);
    stringConcat(newCommand, " ");
    stringConcat(newCommand, value);
    stringConcat(newCommand, ".\r\n");
    stringCopy(newCommand, sendBuffer());
}

void commandBuilder2(char* command, char* attribute, char* value1, char* value2)
{
    char newValue[RECEIVE_PARAM_LENGTH] = "";
    stringConcat(newValue, value1);
    stringConcat(newValue, ":");
    stringConcat(newValue, value2);
    commandBuilder1(command, attribute, newValue);
}

void commandBuilder3(char* command, char* attribute, char* value1, char* value2, char* value3)
{
    char newValue[RECEIVE_PARAM_LENGTH] = "";
    stringConcat(newValue, value2);
    stringConcat(newValue, ":");
    stringConcat(newValue, value3);
    commandBuilder2(command, attribute, value1, newValue);
}

void commandBuilder4(char* command, char* attribute, char* value1, char* value2, char* value3, char* value4)
{
    char newValue[RECEIVE_PARAM_LENGTH] = "";
    stringConcat(newValue, value3);
    stringConcat(newValue, ":");
    stringConcat(newValue, value4);
    commandBuilder3(command, attribute, value1, value2, newValue);
}

void commandBuilder5(char* command, char* attribute, char* value1, char* value2, char* value3, char* value4, char* value5)
{
    char newValue[RECEIVE_PARAM_LENGTH] = "";
    stringConcat(newValue, value4);
    stringConcat(newValue, ":");
    stringConcat(newValue, value5);
    commandBuilder4(command, attribute, value1, value2, value3, newValue);
}

// Split combined value string on colon and return desired part

char* commandExpander(char* values, char index)
{
    static char returnValue[RECEIVE_PARAM_LENGTH] = "";
    int i = 0;
    int j = 0;
    char delimCount = 0;

    while (delimCount < index && i < RECEIVE_PARAM_LENGTH)
        if (values[i++] == ':') delimCount++;
    while (values[i] != ':' && values[i] != 0 && i < RECEIVE_PARAM_LENGTH)
        returnValue[j++] = values[i++];

    returnValue[j] = 0;
    return returnValue;
}

void processReceivedCommand()
{

    if (readyToProcess == 0) return;

    // Everything will be LOWERCASED when it is received
    if (stringCompare(receivedCommand, "set") == 0)
    {
        if (stringCompare(receivedAttribute, "time") == 0)
        {
            char newYear = (char) ((receivedValue[0] - 48) * 10 + receivedValue[1] - 48);
            char newMonth = (char) ((receivedValue[2] - 48) * 10 + receivedValue[3] - 48);
            char newDay = (char) ((receivedValue[4] - 48) * 10 + receivedValue[5] - 48);
            char newHour = (char) ((receivedValue[6] - 48) * 10 + receivedValue[7] - 48);
            char newMinute = (char) ((receivedValue[8] - 48) * 10 + receivedValue[9] - 48);
            char newSecond = (char) ((receivedValue[10] - 48) * 10 + receivedValue[11] - 48);

            writeTime(newYear, newMonth, newDay, newHour, newMinute, newSecond);
            setI2CTime(newYear, newMonth, newDay, newHour, newMinute, newSecond);

            readTimeI2C();
        }
        else if (stringCompare(receivedAttribute, "power") == 0)
        {
            // it would seem that all we need is the allocation from the UI
            // makes no sense to set power used or current load from the UI
            // these are ignored for now, but should be removed at a later time

            tba_energyAllocation = strtoul(commandExpander(receivedValue, 0), NULL, 10);

            //	    powerAllocated = atoi( commandExpander( receivedValue, 0 ) );
            //	    powerUsed = atoi( commandExpander( receivedValue, 1 ) );
            //	    currentLoad = atoi( commandExpander( receivedValue, 2 ) );

            //	    highAlloc = powerAllocated;
            highAlloc = tba_energyAllocation;
            lowAlloc = (tba_energyAllocation * 3) / 4;
            setHighLow();
            EEwritePowerAlloc();

        }
        else if (stringCompare(receivedAttribute, "alarm") == 0)
        {
            audibleAlarm = (char) atoi(commandExpander(receivedValue, 0));
            alarmOneEnabled = (char) atoi(commandExpander(receivedValue, 1));
            alarmTwoEnabled = (char) atoi(commandExpander(receivedValue, 2));
            alarmOnePower = (char) atoi(commandExpander(receivedValue, 3));
            alarmTwoPower = (char) atoi(commandExpander(receivedValue, 4));

            EEwriteAlarm();

        }
        else if (stringCompare(receivedAttribute, "pwd") == 0)
        {
            passwordSet[0] = receivedValue[0];
            passwordSet[1] = receivedValue[1];
            passwordSet[2] = receivedValue[2];
            passwordSet[3] = receivedValue[3];
            passwordSet[4] = receivedValue[4];
            passwordSet[5] = receivedValue[5];

            EEwritePassword();

        }
        else if (stringCompare(receivedAttribute, "emer") == 0)
        {

            emerButtonEnable = (char) atoi(commandExpander(receivedValue, 0));
            emerButtonAlloc = atoi(commandExpander(receivedValue, 1));

            emerAllocNow = atoi(commandExpander(receivedValue, 2));

            tba_energyAllocation += emerAllocNow;

            //	    if( powerUsedMW / 1000 > emerAllocNow )
            //	    {
            //		LATAbits.LATA3 = 0;
            //		powerUsedMW = powerUsedMW - ( ( long ) emerAllocNow ) * 1000;
            //		extraPower += emerAllocNow;
            //	    }
            //	    else
            //	    {
            //		if( emerAllocNow )
            //		{
            //		    extraPower += ( powerUsed / 1000 ) - powerAllocated;
            //		    // Tommmy and I are a bit iffy about powerUsed
            //		    // we think it maybe shoudl be powerUsedMW
            //		}
            //
            //		powerUsedMW = 0;
            //		powerUsed = 0; // Adding this fixed the Emergency Now problem
            //	    }

            EEwriteEmerButton();

        }
        else if (stringCompare(receivedAttribute, "reset") == 0)
        {
            resetHour = (unsigned char) atoi(commandExpander(receivedValue, 0));
            resetMinute = (unsigned char) atoi(commandExpander(receivedValue, 1));

            EEwriteResetTime();

        }

        else if (stringCompare(receivedAttribute, "relay") == 0)
        {
            // ON means relay control is on
            // OFF means control is off
            if (receivedValue[0] == 'f')
                relayActive = 0;
            else
                relayActive = 0xFF;

            EEwriteRelay();

        }

        else if (stringCompare(receivedAttribute, "stat") == 0)
        {
            // do we receive this when a use reset is requested?
            // for now ignore - solve later
            // if we use this, we need to rework how energylifetime and energyyesterday
            //     since energyused is calced from these
            //     if both are set to 0, it works OK, but the power used for the day is lost as well
            //     maybe this is OK

            //	    totalUsed = atoi( commandExpander( receivedValue, 0 ) );
            //	    previousDayUsed = atoi( commandExpander( receivedValue, 1 ) );

        }
        else if (stringCompare(receivedAttribute, "hl") == 0)
        {
            if (stringCompare(receivedValue, "high") == 0)
                isHigh = 0xFF;
            else
                isHigh = 0;

            EEwriteHL();
            setHighLow();

        }
    }


        // Everything will be LOWERCASED when it is received
    else if (stringCompare(receivedCommand, "update") == 0)
    {
        setRemotePower();
    }

        // Everything will be LOWERCASED when it is received
    else if (stringCompare(receivedCommand, "reset") == 0)
    {
        //	reset = 0xFF;
    }


        // Everything will be LOWERCASED when it is received
    else if (stringCompare(receivedCommand, "read") == 0)
    {
        if (stringCompare(receivedAttribute, "time") == 0)
        {
            setRemoteTime();
        }
        else if (stringCompare(receivedAttribute, "power") == 0)
        {
            setRemotePower();
        }
        else if (stringCompare(receivedAttribute, "alarm") == 0)
        {
            setRemoteAlarm();
        }
        else if (stringCompare(receivedAttribute, "pwd") == 0)
        {
            setRemotePassword();
        }
        else if (stringCompare(receivedAttribute, "emer") == 0)
        {
            setRemoteEmergency();
        }
        else if (stringCompare(receivedAttribute, "vers") == 0)
        {
            setRemoteVersion();
        }
        else if (stringCompare(receivedAttribute, "pwrfail") == 0)
        {
            setRemotePowerDownUpTime();
        }
        else if (stringCompare(receivedAttribute, "reset") == 0)
        {
            setRemoteResetTime();
        }
        else if (stringCompare(receivedAttribute, "relay") == 0)
        {
            setRemoteRelay();
        }
        else if (stringCompare(receivedAttribute, "stat") == 0)
        {
            setRemoteStats();
        }
        else if (stringCompare(receivedAttribute, "hl") == 0)
        {
            setRemoteHL();
        }
    }

    readyToProcess = 0;

}

/* Set commands **************************************************************/

/* The space (' ') and period ('.') characters are reserved for delimiters.
 * Valid characters for use within commands are ASCII 0x30 ('0') through
 * 0x7e ('~'). Any character outside that set will be discarded. All alphabetic
 * characters will be converted to lowercase at receive time.
 */


void setRemoteTime(void)
{

    readTimeI2C();

    //    char newTime[11] = "";
    char newTime[13] = "";
    newTime[0] = (timeYear / 10) + 0x30;
    newTime[1] = (timeYear % 10) + 0x30;
    newTime[2] = (timeMonth / 10) + 0x30;
    newTime[3] = (timeMonth % 10) + 0x30;
    newTime[4] = (timeDay / 10) + 0x30;
    newTime[5] = (timeDay % 10) + 0x30;
    newTime[6] = (timeHour / 10) + 0x30;
    newTime[7] = (timeHour % 10) + 0x30;
    newTime[8] = (timeMinute / 10) + 0x30;
    newTime[9] = (timeMinute % 10) + 0x30;
    newTime[10] = (timeSecond / 10) + 0x30;
    newTime[11] = (timeSecond % 10) + 0x30;
    newTime[12] = 0;

    commandBuilder1("Set", "Time", newTime);

}

void setRemotePower(void)
{
    unsigned long tempEnergyUsed;

    tempEnergyUsed = tba_energyUsedLifetime - tba_energyUsedLastDayReset;

    char charEnergyAllocated[12];
    char charEnergyUsed[12];
    char charPowerWatts[12];

    ultoa(charEnergyAllocated, tba_energyAllocation, 10);
    ultoa(charEnergyUsed, tempEnergyUsed, 10);
    if (tba_powerWatts == 0)
    {
        charPowerWatts[0] = '0';
        charPowerWatts[1] = '\0';
    }
    else
    {
        ultoa(charPowerWatts, tba_powerWatts, 10);
    }

    commandBuilder3("Set", "Power", charEnergyAllocated, charEnergyUsed, charPowerWatts);

    //
    //    char newPowerAllocated[7];
    //    utoa( newPowerAllocated, ( unsigned int ) powerAllocated, 10 );
    //
    //    char newPowerUsed[7];
    //    utoa( newPowerUsed, ( unsigned int ) powerUsed, 10 );
    //
    //    char newCurrentLoad[7];
    //    utoa( newCurrentLoad, ( unsigned int ) powerWatts, 10 );
    //
    //    commandBuilder3( "Set", "Power", newPowerAllocated,
    //		     newPowerUsed, newCurrentLoad );

    return;
}

void setRemoteAlarm(void)
{
    char* newAudibleAlarm = itoa(buffer1, audibleAlarm, 10);
    char* newAlarmOneEnabled = itoa(buffer2, alarmOneEnabled, 10);
    char* newAlarmTwoEnabled = itoa(buffer3, alarmTwoEnabled, 10);
    char* newAlarmOnePower = itoa(buffer4, alarmOnePower, 10);
    char* newAlarmTwoPower = itoa(buffer5, alarmTwoPower, 10);

    commandBuilder5("Set", "Alarm", newAudibleAlarm,
            newAlarmOneEnabled, newAlarmTwoEnabled,
            newAlarmOnePower, newAlarmTwoPower);
}

void setRemotePassword(void)
{
    char newPassword[7];
    int i;
    for (i = 0; i < 6; i++)
        newPassword[i] = passwordSet[i];
    newPassword[6] = 0;

    commandBuilder1("Set", "Pwd", newPassword);
}

void setRemoteEmergency(void)
{
    char* newEmerButtonEnable = itoa(buffer1, (int) emerButtonEnable, 10);
    char* newEmerButtonAlloc = itoa(buffer2, emerButtonAlloc, 10);
    char* newEmerAllocSend = itoa(buffer3, emerAllocSend, 10);

    commandBuilder3("Set", "Emer", newEmerButtonEnable,
            newEmerButtonAlloc, newEmerAllocSend);
}

void setRemoteVersion(void)
{
    commandBuilder1("Set", "Vers", POWER_BOX_CODE_VERSION);
}

void setRemotePowerDownUpTime(void)
{
    commandBuilder1("Set", "PDT", powerDownTime);
    commandBuilder1("Set", "PUT", powerUpTime);
}

void setRemoteResetTime(void)
{
    char* newResetHour = itoa(buffer1, (int) resetHour, 10);
    char* newResetMinute = itoa(buffer2, (int) resetMinute, 10);

    commandBuilder2("Set", "Reset", newResetHour, newResetMinute);
}

void setRemoteRelay(void)
{
    if (relayActive)
        commandBuilder1("Set", "Relay", "On");
    else
        commandBuilder1("Set", "Relay", "F");
}

void setRemoteStats(void)
{

    char charEnergyUsedLifetime[12];
    char charEnergyUsedPreviousDay[12];

    ultoa(charEnergyUsedLifetime, tba_energyUsedLifetime, 10);
    ultoa(charEnergyUsedPreviousDay, tba_energyUsedPreviousDay, 10);

    commandBuilder2("Set", "Stat", charEnergyUsedLifetime, charEnergyUsedPreviousDay);

    //    char* stats = itoa( buffer1, totalUsed / 1000, 10 );
    //    char* previousDayStats = itoa( buffer2, previousDayUsed, 10 );
    //    commandBuilder2( "Set", "Stat", stats, previousDayStats );
}

void setRemoteHL(void)
{
    if (isHigh)
        commandBuilder1("Set", "HL", "High");
    else
        commandBuilder1("Set", "HL", "Low");
}

void doReset(void)
{
    commandBuilder1("Reset", "0", "0");
}

void sendUpdate(void)
{
    setRemotePower();
}

/* Read Commands **************************************************************/
void readRemoteTime(void)
{
    commandBuilder1("Read", "Time", "0");
}

void readRemotePower(void)
{
    commandBuilder1("Read", "Power", "0");
}

void readRemoteAlarm(void)
{
    commandBuilder1("Read", "Alarm", "0");
}

void readRemotePassword(void)
{
    commandBuilder1("Read", "Pwd", "0");
}

void readRemoteEmergency(void)
{
    commandBuilder1("Read", "Emer", "0");
}

void readRemoteVersion(void)
{
    commandBuilder1("Read", "Vers", "0");
}

void readRemoteResetTime(void)
{
    commandBuilder1("Read", "Reset", "0");
}

void readRemoteRelay(void)
{
    commandBuilder1("Read", "Relay", "0");
}

void readRemoteStats(void)
{
    commandBuilder1("Read", "Stat", "0");
}

void readRemoteHL(void)
{
    commandBuilder1("Read", "HL", "0");
}

void readRemotePowerDownUpTime(void)
{
    commandBuilder1("Read", "PwrFail", "0");
}

void readUpdate(void)
{
    commandBuilder1("Update", "0", "0");
}