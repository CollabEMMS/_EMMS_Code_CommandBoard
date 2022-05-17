/******
 Common Header File
 Only put something in here if:
	it is required for all C files
	and
	it is not best included in any other header file
 */


/**********************
 READ THIS
 * PROBLEM
 * 
 * Sometimes a standard include file (included with the < include.h > symbology
 *    gets a red underline error, but the program compiles fine
 * The IDE cannot resolve correctly the include file so it notes an error
 * The compiler is separate from the IDE and works differently - it compiles just fine
 * The issue seems to stem from spaces in file paths - the default paths used by the IDE/compiler
 * This keeps the code completion from working properly and generally causes headaches as you try to code
 * Holding <CTRL> while mouse hovering over the include shows the paths searched
 *     you will notice that the path for the include file is there, it just isn't working right
 * 
 * The fix is to manually add the include paths to the compiler
 *     the paths must be in old DOS 8.3 notation
 * Open Project Properties -> Conf:[default] -> XC16
 *     in the "C include dirs" add the following entries
 * 
 * C:\PROGRA~2\Microchip\xc16\v1.36\support\PIC24F\h
 * C:\PROGRA~2\Microchip\xc16\v1.36\support\generic\h
 * C:\PROGRA~2\Microchip\xc16\v1.36\include
 *
 * ensure the above is adjusted for the specific processor
 * ensure the above is adjusted for the specific compiler version
 * 
 * now the code completion works and the red underlines go away
 
 ******************/



#ifndef COMMON_H
#    define	COMMON_H


/****************
 INCLUDES - COMMON
 these include files are external to this project and are used by all c modules
 this list here should be very limited
 things like stdlib should be included in the c module where it is needed
 not all c modules really need stdlib
 ****************/

// operating frequency
// this must come before the include for libpic30.h because it uses it
// the UL postfix stipulates that the number is unsigned long
#    define FCY 16000000UL


#    include <p24FV32KA302.h>
#    include <libpic30.h>
#    include <stdbool.h>


/****************
 MACROS
 these are the macros that are required by external c files
 do not include macros that are only used internally within this module
 ****************/

//							"          "  10 character spacing reference - do not exceed or the system will crash
#    define MODULE_INFO_THIS_0	"Comand Brd"			// 10 chars max name
#    define MODULE_INFO_THIS_1	"v2.8.0"			// 10 chars max version
#    define MODULE_INFO_THIS_2	""					// 10 chars max info
#    define MODULE_INFO_THIS_3	""					// 10 chars max info
#    define MODULE_INFO_THIS_4	""					// 20 chars max info (only 18 will show on UI)

#    define MODULE_INFO_SIZE_SMALL	11	// 10 + include the null char
#    define MODULE_INFO_SIZE_LARGE	21	// 20 + include the null char
#    define MODULE_COUNT		6	// 6 module info sets: 0=Command Board, 1=UART-1, 2=UART-2, 3=SPI-0, 4=SPI-1, 5=SPI-2
#    define MODULE_INFO_COUNT	5	// number of info items per module


/***********************
	LEDS for debugging
	disables normal program LED use (for the power remaining bar graph)
	allows LED testing functions to be used
	set this to 'true' to turn off the LED graph and enable "Test" LEDs for testing purposes
 *************************/

#    define LEDS_FOR_DEBUG false


/******************
	Error Trap Interrupts
	when true - this will hide most memory errors
	just hiding the error is not good as there could be bigger underlying problems which cause other issues
	this should only be set to true for production builds
	if there is a reset error, these traps can also be used to find what is causing it by setting a breakpoint and debugging the program line-by-line
 *****************/
#    define ERROR_TRAP_INTERRUPTS_ACTIVE false

/***********************
	UART2 Modes
 ******************
	UART2_MODULE
		Regular meter communication over UART2
	UART2_DEBUG_IDE
		IDE Debug Mode
		Allows the MPLAB X IDE to be run in debugging mode
			view memory
			view variable values
			step the program line-by-line
		Requires PIC24 pins 4 & 5 - shared pins with other functions
			UART2 TX & RX
			RB0 and RB1 general input/output
			PGED1 and PGEC1 programming pins (required for debugging)
		UART2 cannot be used
			this is the top port on the CommandBoard PCB
			The MAX488 chip cannot be installed for this port (top chip location)
			If it is installed, traces must be cut to disconnect the chip from PIC pins 4 & 5 must be cut
		All UART2 code must be removed from the compile
	UART2_DEBUG_OUTPUT
		Use UART2 for general output of debug information
		Connect UART2 TX to input of reader PIC24 pin 4 - RB0
		Connect UART2 ground 
 
	UART2 Control
		Module Communication	UART2_MODULE
		IDE_DEBUG from PicKit	UART2_DEBUG_IDE
		UART2 Debug output		UART2_DEBUG_OUTPUT
 Only one can be uncommented at a time
 
 ************************/
#    define UART2_MODULE
//#define UART2_DEBUG_IDE
//#    define UART2_DEBUG_OUTPUT

#    if !defined UART2_MODULE && !defined UART2_DEBUG_IDE && !defined UART2_DEBUG_OUTPUT
#        error One UART2 option must be defined
#    endif

#    if (defined UART2_MODULE && defined UART2_DEBUG_IDE) || ( defined UART2_MODULE && defined UART2_DEBUG_OUTPUT ) || ( defined UART2_DEBUG_IDE && defined UART2_DEBUG_OUTPUT )
#        error More than one UART2 option defined - only one can be defined at a time
#    endif


#    define CHAR_NULL '\0'   //yes, used in many many places

/****************
 STRUCTS
 define the structures here
 ***************/
struct moduleInfo_struct
{
	char info0[MODULE_INFO_SIZE_SMALL];
	char info1[MODULE_INFO_SIZE_SMALL];
	char info2[MODULE_INFO_SIZE_SMALL];
	char info3[MODULE_INFO_SIZE_SMALL];
	char info4[MODULE_INFO_SIZE_LARGE];
};

struct date_time_struct
{
	unsigned int year;
	char month;
	char day;
	char hour;
	char minute;
	char second;
	bool valid;
};

struct alarm_info_struct
{
	bool alarmAudible;
	int alarm1PercentThreshold;
	int alarm2PercentThreshold;
};

struct energy_info
{
	// energy is stored as the lifetime energy used by this meter
	// it only increments and never goes down
	// we calculate the energy used per day by storing the last energy lifetime used subtracting that from the current energy used
	//      lifetime - last_reset = day last used
	// we also store the previous day's usage for reporting purposes

	unsigned long lifetime; // the total lifetime energy adder - this only increments, never goes down
	unsigned long lastReset; // the lifetime value at the last reset ( lifetime - lastReset = todayUsed )
	unsigned long previousCycleUsed; // the energy used during the previous cycle

};

struct calibration_struct
{
	unsigned long cal1;
	unsigned long cal2;
};

struct reset_time
{
	int hour;
	int minute;
};

struct emergency_button
{
	bool enabled;
	int energyAmount;
};



#endif	/* COMMON_H */

/***********************/
/***********************/
/***********************/
/***********************/
/***********************/
/***********************/



/******
 Below are the comments for the headers of C files
 This should be placed at the top of each c file to provide some sense and
   structure to make things easier to understand
 */

/****************
 INCLUDES
 only include the header files that are required
 ****************/

/****************
 MACROS
 ****************/

/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
 as external
 ****************/

// external

// internal only


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
	 should be marked
 *****************/



/****************
 CODE
 ****************/










/******
 Below are the comments for the H files
 This should be placed at the top of each h file to provide some sense and
   structure to make things easier to understand
 */

/****************
 MACROS
 these are the macros that are required by external c files
 do not include macros that are only used internally within this module
 ****************/

/****************
 VARIABLES
 these are the globals required by external c files
 there should be as few of these as possible to help keep things clean
 these are all 'extern' and require that the variable is declared in the c file
 ****************/


/****************
 FUNCTION PROTOTYPES
 only include functions required by external c files
 ideally these are in the same order as in the code listing
 any functions used internally and externally must have the prototype in both the c and h files and should be marked
 
 *****************/


// external and internal

// external only

