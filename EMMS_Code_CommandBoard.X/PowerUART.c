/* File:    PowerUART.c
 * Authors: Dan Baker
 *          Nathan Chaney
 */

/* Includes *******************************************************************/

#include <p24FV32KA302.h>
#include "PowerPinDefinitions.h"
#include "ExternPowerDefinitions.h"
#include "ExternSharedDefinitions.h"

/* Functions ******************************************************************/

void initUART2(void);
void initUART1(void);

void initUART(void) {
    initUART2();
    initUART1();
}

void initUART2(void) {
  // FCY = 2 MHz;
  // BAUD = FCY / (16 * (U2BRG + 1))
  // BRG = -1 + FCY / (16 * BAUD)
  // Ok to be slightly off
  // U2BRG = 1;  // baud rate is 62500
  // U2BRG = 3;  // baud rate is 31250
  // U2BRG = 6;  // baud rate is 17829
   U2BRG = 12; // baud rate is 9615
  // U2BRG = 25; // baud rate is 4800
  // U2BRG = 50; // baud rate is 2447


    U2MODEbits.USIDL = 0b0;
    U2MODEbits.IREN = 0b0;
    U2MODEbits.RTSMD = 0b1;
    U2MODEbits.UEN = 0b00;
    U2MODEbits.WAKE = 0b0;
    U2MODEbits.LPBACK = 0b0;
    U2MODEbits.ABAUD = 0b0;
    U2MODEbits.RXINV = 0b0;
    U2MODEbits.BRGH = 0b0;
    U2MODEbits.PDSEL = 0b00;
    U2MODEbits.STSEL = 0b0;

    U2STAbits.UTXISEL0 = 0b0;
    U2STAbits.UTXISEL1 = 0b0;
    U2STAbits.UTXINV = 0b0;
    U2STAbits.UTXBRK = 0b0;
    U2STAbits.URXISEL1 = 0b0; // interrupt after one RX character is received
    U2STAbits.ADDEN = 0;

    //interrupts
    _U2RXIP = 1;     // set RX interrupt priority (1-7, default 4)
    _U2ERIF = 0;     // clear Error Flag
    _U2ERIE = 1;     // enable Error Interrupt
    _U2TXIF = 0;     // clear TX Flag
    _U2TXIE = 0;     // disable TX Interrupt
    _U2RXIF = 0;     // clear RX interrupt flag
    _U2RXIE = 1;     // enable RX interrupt

    U2MODEbits.UARTEN = 0b1; // turn it on
    U2STAbits.UTXEN   = 0b1; // enable transmit

}

void initUART1(void) {
  // FCY = 2 MHz;
  // BAUD = FCY / 16 / (U1BRG + 1)
  // Ok to be slightly off
  // U1BRG = 1;  // baud rate is 62500
  // U1BRG = 3;  // baud rate is 31250
  // U1BRG = 6;  // baud rate is 17829
   U1BRG = 12; // baud rate is 9615
  // U1BRG = 25; // baud rate is 4800
  // U1BRG = 50; // baud rate is 2447


    U1MODEbits.USIDL = 0b0;
    U1MODEbits.IREN = 0b0;
    U1MODEbits.RTSMD = 0b1;
    U1MODEbits.UEN = 0b00;
    U1MODEbits.WAKE = 0b0;
    U1MODEbits.LPBACK = 0b0;
    U1MODEbits.ABAUD = 0b0;
    U1MODEbits.RXINV = 0b0;
    U1MODEbits.BRGH = 0b0;
    U1MODEbits.PDSEL = 0b00;
    U1MODEbits.STSEL = 0b0;

    U1STAbits.UTXISEL0 = 0b0;
    U1STAbits.UTXISEL1 = 0b0;
    U1STAbits.UTXINV = 0b0;
    U1STAbits.UTXBRK = 0b0;
    U1STAbits.URXISEL1 = 0b0; // interrupt after one RX character is received
    U1STAbits.ADDEN = 0;

    //interrupts
    _U1RXIP = 1;     // set RX interrupt priority (1-7, default 4)
    _U1ERIF = 0;     // clear Error Flag
    _U1ERIE = 1;     // enable Error Interrupt
    _U1TXIF = 0;     // clear TX Flag
    _U1TXIE = 0;     // disable TX Interrupt
    _U1RXIF = 0;     // clear RX interrupt flag
    _U1RXIE = 1;     // enable RX interrupt

    U1MODEbits.UARTEN = 0b1; // turn it on
    U1STAbits.UTXEN   = 0b1; // enable transmit

}

void __attribute__((__interrupt__,__no_auto_psv__)) _U2RXInterrupt(void) {

    unsigned char tempChar;

    _U2RXIF = 0; // clear interrupt flag

    U2STAbits.OERR = 0;

    while (U2STAbits.URXDA == 1) { // something to receive

        tempChar = U2RXREG;

        if (tempChar > 31 && tempChar < 127) { // ignore junk characters
            if (tempChar > 64 && tempChar < 91)
                tempChar += 32;
            receiveBufferU2[receiveIndexU2++] = tempChar;
        }

        if ((receiveIndexU2 > RECEIVE_STRING_LENGTH - 1) || (tempChar == '.') || (tempChar == 0)) {
            while (receiveIndexU2 < RECEIVE_STRING_LENGTH)
                receiveBufferU2[receiveIndexU2++] = 0;
            receiveIndexU2 = 0;

            stringCopy(receiveBufferU2, receiveBuffer());
            newReceiveData = 1;
        }
    }
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U2ErrInterrupt(void) {
//    U2MODEbits.UARTEN = 0b0;
//    delayMS(1);
//    U2MODEbits.UARTEN = 0b1;
    U2STAbits.PERR;
    U2STAbits.FERR;
    U2STAbits.OERR = 0;

    _U2ERIF = 0; // clear error interrupt flag
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U1RXInterrupt(void) {

    unsigned char tempChar;

    U1STAbits.OERR = 0;

    while (U1STAbits.URXDA == 1) { // something to receive

        tempChar = U1RXREG;

        if (tempChar > 31 && tempChar < 127) { // ignore junk characters
            if (tempChar > 64 && tempChar < 91)
                tempChar += 32;
            receiveBufferU1[receiveIndexU1++] = tempChar;
        }

        if (receiveIndexU1 > RECEIVE_STRING_LENGTH - 1 || tempChar == '.' || tempChar == 0) {
            while (receiveIndexU1 < RECEIVE_STRING_LENGTH)
                receiveBufferU1[receiveIndexU1++] = 0;
            receiveIndexU1 = 0;

            stringCopy(receiveBufferU1, receiveBuffer());
            newReceiveData = 1;
        }
    }

    _U1RXIF = 0; // clear interrupt flag
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U1ErrInterrupt(void) {
//    U1MODEbits.UARTEN = 0b0;
//    delayMS(1);
//    U1MODEbits.UARTEN = 0b1;
    U1STAbits.PERR;
    U1STAbits.FERR;
    U1STAbits.OERR = 0;

    _U1ERIF = 0; // clear error interrupt flag
}
