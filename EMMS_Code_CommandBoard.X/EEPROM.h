#ifndef EEPROM_H
#define	EEPROM_H


extern char passwordSet[6];

void EEwriteEnergyAlloc( void );
void EEwriteAlarm( void );
void EEwritePassword( void );
void EEwriteEmerButton( void );
void EEwriteResetTime( void );
void EEwriteRelay( void );




// not sorted yet
void EEreadEnergyAlloc( void);
void EEwriteTotals(void);
void EEwriteEnergyUsed(void);
void EEreadAll(void);



//void EEreadPassword( void );
//void EEreadDate(void);
//void EEwriteDate(void);
//void EEreadEmerButton( void );
//void EEreadResetTime( void );
//void EEwriteResetTime( void );
//void EEreadAlarm( void );
//void EEreadTotals( void );
//void EEwriteTotals( void );
//void EEreadPowerUsed( void );
//void EEwriteEnergyUsed( void );
//void EEreadHL( void );
//void EEwriteHL( void );
//void EEreadRelay( void );


#endif	/* EEPROM_H */

