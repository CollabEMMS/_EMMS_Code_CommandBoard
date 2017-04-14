/* File:    PowerPinDefinitions.h
 * Authors: Dan Baker
 *          Nathan Chaney
 */

#ifndef POWERPINDEFINITIONS_H
#define	POWERPINDEFINITIONS_H

#define PIC_VERSION 28

#if PIC_VERSION == 20

    // 20 pin PIC
    #define PIN1        _RA5    // Pin 1:  RA5
    #define PIN2        _RA0    // Pin 2:  RA0
    #define PIN3        _RA1    // Pin 3:  RA1
    #define PIN4        _RB0    // Pin 4:  RB0
    #define PIN5        _RB1    // Pin 5:  RB1
    #define PIN6        _RB2    // Pin 6:  RB2
    #define PIN7        _RA2    // Pin 7:  RA2
    #define PIN8        _RA3    // Pin 8:  RA3
    #define PIN9        _RB4    // Pin 9:  RB4
    #define PIN10       _RA4    // Pin 10: RA4
    #define PIN11       _RB7    // Pin 11: RB7
    #define PIN12       _RB8    // Pin 12: RB8
    #define PIN13       _RB9    // Pin 13: RB9
    #define PIN14       "Vcap"  // Pin 14: Vcap
    #define PIN15       _RB12   // Pin 15: RB12
    #define PIN16       _RB13   // Pin 16: RB13
    #define PIN17       _RB14   // Pin 17: RB14
    #define PIN18       _RB15   // Pin 18: RB15
    #define PIN19       "Vss"   // Pin 19: Vss
    #define PIN20       "Vdd"   // Pin 20: Vdd

    #define BTN_EMER    _RA0    // Pin 2:  RA0
    #define BAR_GRAPH   _RB2    // Pin 6:  RB2
    #define RELAY       _RB14   // Pin 17: RB14
    #define MCP_RESET   _RB15   // Pin 18: RB15

#else

    // 28 pin PIC
    #define PIN1        _RA5    // Pin 1:  RA5
    #define PIN2        _RA0    // Pin 2:  RA0
    #define PIN3        _RA1    // Pin 3:  RA1
    #define PIN4        _RB0    // Pin 4:  RB0
    #define PIN5        _RB1    // Pin 5:  RB1
    #define PIN6        _RB2    // Pin 6:  RB2
    #define PIN7        _RB3    // Pin 7:  RA2
    #define PIN8        "Vss"   // Pin 8:  RA3
    #define PIN9        _RA2    // Pin 9:  RB4
    #define PIN10       _RA3    // Pin 10: RA4
    #define PIN11       _RB4    // Pin 11: RB7
    #define PIN12       _RA4    // Pin 12: RB8
    #define PIN13       "Vdd"   // Pin 13: RB9
    #define PIN14       _RB5    // Pin 14: Vcap
    #define PIN15       _RB6    // Pin 15: RB12
    #define PIN16       _RB7    // Pin 16: RB13
    #define PIN17       _RB8    // Pin 17: RB14
    #define PIN18       _RB9    // Pin 18: RB15
    #define PIN19       _RA7    // Pin 19: Vss
    #define PIN20       "Vcap"  // Pin 20: Vdd
    #define PIN21       _RB10   // Pin 21: RB10
    #define PIN22       _RB11   // Pin 22: RB11
    #define PIN23       _RB12   // Pin 23: RB12
    #define PIN24       _RB13   // Pin 24: RB13
    #define PIN25       _RB14   // Pin 25: RB14
    #define PIN26       _RB15   // Pin 26: RB15
    #define PIN27       "Vss"   // Pin 27: Vss
    #define PIN28       "Vdd"   // Pin 28: Vdd

    #define BTN_EMER    _RA0    // Pin 2:  RA0
    #define BAR_GRAPH   _RB11   // Pin 22: RB11
    #define RELAY       _RB6    // Pin 15: RB6
    #define MCP_RESET   _RB15   // Pin 26: RB15
    #define BUZZER      _RA3    // Pin 8:  RA3

#endif

#endif	/* POWERPINDEFINITIONS_H */
