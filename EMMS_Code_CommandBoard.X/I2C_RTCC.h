#ifndef I2C_RTCC_H
#define	I2C_RTCC_H


extern unsigned char timeYear;
extern unsigned char timeMonth;
extern unsigned char timeDay;
extern unsigned char timeWeekday;
extern unsigned char timeHour;
extern unsigned char timeMinute;
extern unsigned char timeSecond;

extern unsigned char powerFailTimeMinute;
extern unsigned char powerFailTimeHour;
extern unsigned char powerFailTimeDay;
extern unsigned char powerFailTimeMonth;

extern unsigned char powerRestoreTimeMinute;
extern unsigned char powerRestoreTimeHour;
extern unsigned char powerRestoreTimeDay;
extern unsigned char powerRestoreTimeMonth;



// not sorted yet
void initI2C( void );
void startClock( void );
void readI2CPowerTimes( void );
void readI2CTime( unsigned char* year, unsigned char* month, unsigned char* day,
		  unsigned char* hour, unsigned char* minute, unsigned char* second );
void setI2CTime( unsigned char year, unsigned char month, unsigned char day,
		 unsigned char hour, unsigned char minute, unsigned char second );
void readTimeI2C( void );
bool writeTime( char newYear, char newMonth, char newDay, char newHour, char newMinute, char newSecond );
void readTime( void );

//extern char powerDownTime[12];
//extern char powerUpTime[12];




#endif	/* I2C_RTCC_H */

