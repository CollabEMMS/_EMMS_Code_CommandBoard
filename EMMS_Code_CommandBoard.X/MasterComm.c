/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "LEDControl.h"
#include "RTCC.h"
#include "EEPROM.h"
#include "PowerMain.h"

/****************
 MACROS
 ****************/
#define BAUD_UART   9600    // set the baud rate as close to this as practical
#define BAUD_SPI    9600    // set the clock rate as close to this as possible
// be aware that the SPI clock is not calculated based off this
// the init function needs modified directly

#define BUFFER_LENGTH_RECV 10	// length of receive buffers - characters are received by interrupt
#define BUFFER_LENGTH 40  // max size is positive signed character size (255))
#define PORT_COUNT 3 // one based count of the number of ports

#define BUF_SIZE_CHAR 5
#define BUF_SIZE_INT 7
#define BUF_SIZE_LONG 12

#define PARAMETER_MAX_COUNT 7
#define PARAMETER_MAX_LENGTH 10

//#define CHAR_NULL '\0' // defined in common.h since it is used in a lot of places
#define COMMAND_SEND_RECEIVE_PRIMER_CHAR '#' // something to run the SPI clock so data can be received
#define COMMAND_START_CHAR '!'
#define COMMAND_END_CHAR '*'
#define COMMAND_DELIMETER ';'
#define XSUM_DELIMETER '$'


#define RECEIVE_WAIT_COUNT_LIMIT 25
#define RECEIVE_IN_COMMAND_COUNT_LIMIT 253

#define SPI_PORT_0_DIR TRISBbits.TRISB15
#define SPI_PORT_1_DIR TRISBbits.TRISB14
#define SPI_PORT_2_DIR TRISBbits.TRISB12

#define SPI_PORT_0 LATBbits.LATB15
#define SPI_PORT_1 LATBbits.LATB14
#define SPI_PORT_2 LATBbits.LATB12

/****************
 VARIABLES
 these are the globals required by only this c file
 there should be as few of these as possible to help keep things clean
 variables required by other c functions should be here and also in the header .h file
   as external
   indicate the variables which as also external
 ****************/

// external


// internal only

// character receiving buffers (internal)
volatile char UARTRecvBuffer[10];
volatile int UARTRecvBufferReadPos = 0;
volatile int UARTRecvBufferWritePos = 0;

// character receiving buffers (internal)
volatile char SPIRecvBuffer[BUFFER_LENGTH_RECV];
volatile int SPIRecvBufferReadPos = 0;
volatile int SPIRecvBufferWritePos = 0;

enum receive_status_enum
{
    enum_receive_status_waiting,
    enum_receive_status_in_command,
    enum_receive_status_end_command
};

enum communications_port_enum
{
    enum_port_commSPI,
    enum_port_commUART1,
    enum_port_commUART2,
};

struct buffer_struct
{
    enum communications_port_enum port;
    char data_buffer[ BUFFER_LENGTH + 1];
    unsigned char write_position;
    unsigned char read_position;
};


/****************
 FUNCTION PROTOTYPES
 only include functions called from within this code
 external functions should be in the header
 ideally these are in the same order as in the code listing
 any functions used internally and externally (prototype here and in the .h file)
     should be marked
 ****************/
bool set_current_port( unsigned char * );

bool process_data( struct buffer_struct *receive_buffer, struct buffer_struct *send_buffer, bool xSumMatches );
void process_data_parameterize( char parameters[][PARAMETER_MAX_LENGTH], struct buffer_struct *buffer_to_parameterize );
bool process_data_parameters( char parameters[][PARAMETER_MAX_LENGTH], struct buffer_struct *send_buffer );

void command_builder1( struct buffer_struct *send_buffer, char* data1 );
void command_builder2( struct buffer_struct *send_buffer, char* data1, char* data2 );
void command_builder3( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3 );
void command_builder4( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4 );
void command_builder5( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5 );
void command_builder6( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6 );
void command_builder7( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6, char* data7 );
int command_builder_add_char( struct buffer_struct *send_buffer, char data );
int command_builder_add_string( struct buffer_struct *send_buffer, char *data );
void xsum_builder( struct buffer_struct *send_buffer, int xsum );


bool SPI_receive_data_char( char * );
bool SPI_send_data_char( char data );
bool UART1_receive_data_char( char * );
bool UART1_receive_all_data_char( char *data );
bool UART1_send_data_char( char data );
bool UART2_receive_data_char( char * );
bool UART2_send_data_char( char data );

bool strmatch( char* a, char* b );
int strcmp2( char* a, char* b );
void strcpy2( char* rcv, char* source );

void fillOnOff( char *buf, int checkValue );
bool checkOnOff( char *toCheck );

void zeroPad_itoa( char *output, int num, int minDigits );


void send_end_of_transmission( struct buffer_struct *send_buffer );


void communicationsSPI( bool initialize );
void communicationsUART1( bool initialize );
void communicationsUART2( bool initialize );
bool communicationsRecv( struct buffer_struct *receive_buffer, struct buffer_struct *send_buffer, enum communications_port_enum communicationsPort, enum receive_status_enum *receive_current_state );
bool communicationsSend( struct buffer_struct *send_buffer, enum communications_port_enum communicationsPort );

void uartInit( void );
void commSPIInit( void );
void initUART2( void );
void initUART1( void );

bool xSumMatches( struct buffer_struct buffer_to_chk );

/****************
 CODE
 ****************/


// Setup comm ports and other fun stuff
void commInit( )
{

    bool initialize;

    initialize = true;

    uartInit( );
    commSPIInit( );

    communicationsSPI( initialize );

    communicationsUART1( initialize );
    communicationsUART2( initialize );

}

// Runs the comm loop for continuous operation 
void commRunRoutine( )
{
    bool initialize;

    initialize = false;

    communicationsSPI( initialize );

    communicationsUART1( initialize ); // This one works (Lower RJ45)
    communicationsUART2( initialize ); // This one no worky (Upper RJ45)


}

// Handles all of the SPI functionality, for the the initial and the continuous running processes
void communicationsSPI( bool initialize )
{

    bool enabledSPI;

    static unsigned char current_port = PORT_COUNT; // port we are on - zero based - 0 to (PORT_COUNT - 1) we start with max so next port is 0
    static unsigned char current_port_done = true; // start with true and let normal program mechanism automatically init things

    static struct buffer_struct send_buffer;
    static struct buffer_struct receive_buffer;

    static bool end_of_transmission_received = false;
    bool no_more_to_send; // here to make this more readable

    static enum receive_status_enum receive_current_state = enum_receive_status_waiting;
    static unsigned int receive_wait_count;
    static unsigned int receive_in_command_count;

    if( initialize == true )
    {
	send_buffer.port = enum_port_commSPI;
	//for now do nothing else
	// variable init takes place when port changes, which initial values cause automatically
	// maybe init SPI ports here in the future
    }

    if( current_port_done == true )
    {
	enabledSPI = set_current_port( &current_port );

	if( enabledSPI == true )
	{
	    current_port_done = false;
	    end_of_transmission_received = false;

	    receive_wait_count = 0;
	    receive_in_command_count = 0;

	    send_buffer.write_position = 0;
	    send_buffer.read_position = 0;
	    receive_buffer.write_position = 0;
	    receive_buffer.read_position = 0;

	    // put something in the send buffer to run the clock
	    // clock must run to receive any characters
	    command_builder_add_char( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	}
    }

    bool data_received;

    data_received = communicationsRecv( &receive_buffer, &send_buffer, enum_port_commSPI, &receive_current_state );

    switch( receive_current_state )
    {
	case enum_receive_status_waiting:
	    // count # of times we are waiting for COMMAND_START_CHAR !
	    if( data_received == true )
	    {
		receive_wait_count++;
	    }

	    break;
	case enum_receive_status_in_command:
	    // count # of times we are in a command
	    // need to check if we somehow missed the COMMAND_END_CHAR *
	    // must be more than max length a command can be
	    if( data_received == true )
	    {
		receive_wait_count = 0;
		receive_in_command_count++;
	    }

	    break;
	case enum_receive_status_end_command:
        
        if( process_data( &receive_buffer, &send_buffer, xSumMatches(receive_buffer) ) == true ){
            end_of_transmission_received = true;
        }
        
	    receive_wait_count = 0;
	    receive_in_command_count = 0;
	    break;
    }

    no_more_to_send = communicationsSend( &send_buffer, enum_port_commSPI );

    if( no_more_to_send == true )
    {
	if( end_of_transmission_received == true )
	{
	    // make sure transmit buffer is empty
	    // the following test is for standard buffer mode only
	    // a different check must be performed if the enhanced buffer is used
	    if( SPI1STATbits.SPITBF == 0b0 ) // only for standard buffer
	    {
		current_port_done = true;
	    }
	}
	else if( receive_wait_count >= RECEIVE_WAIT_COUNT_LIMIT )
	{
	    // not receiving anything valid from slave
	    // just move to the next port - things should clear up on their own eventually
	    current_port_done = true;
	}
	else if( receive_in_command_count >= RECEIVE_IN_COMMAND_COUNT_LIMIT )
	{
	    // received too many characters before the command was ended
	    // likely a garbled COMMAND_END_CHAR or something
	    // just move to the next port - things should clear up on their own eventually
	    current_port_done = true;
	}
	else
	{
	    command_builder_add_char( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	}
    }

    return;
}
// Will check if the Checksum in a message is accurate to ensure accuracy
bool xSumMatches( struct buffer_struct buffer_to_chk){
    
    //xsum vars
    //XSUM = sum of ascii value of all chars in command EXCEPT start char '!' and final delimeter ';'
    int xsum = 0;
    int recXsum = 0;
    char recXsumbuf[16];
    int recXsumPointer = 0;
    bool xsumRecieving = false; //this is true after the xsum delimiter
    char currentData;  
    char* data = buffer_to_chk.data_buffer;
    int i = 1;  
        //cycle through receive buffer
        //we start at 1 because the start char '!' is not needed
        while (currentData != COMMAND_END_CHAR){
            
            currentData = data[i];
            i++;

            if (xsumRecieving){ 
                recXsumbuf[recXsumPointer] = currentData;
                recXsumPointer++;
            }
            else{
                //if the next character in line is the delimiter...
                if (data[i] == XSUM_DELIMETER){
                    data[i] = CHAR_NULL;
                    xsumRecieving = true;
                    recXsumbuf[recXsumPointer] = CHAR_NULL;
                    i++;
                }
                else{
                    xsum += currentData;
                }

            }

        }

        bool matches = false;

        recXsum = atoi (recXsumbuf);
        if (xsum == recXsum){
            matches = true;
        }

        return matches;
}

// Handles the UART functionality for UART1, for the the initial and the continuous running processes
void communicationsUART1( bool initialize )
{

    static struct buffer_struct send_buffer;
    static struct buffer_struct receive_buffer;
    static enum receive_status_enum receive_current_state = enum_receive_status_waiting;


    if( initialize == true )
    {
	send_buffer.port = enum_port_commUART1;
	send_buffer.read_position = 0;
	send_buffer.write_position = 0;
	receive_buffer.read_position = 0;
	receive_buffer.write_position = 0;
	receive_current_state = enum_receive_status_waiting;
    }

    communicationsRecv( &receive_buffer, &send_buffer, enum_port_commUART1, &receive_current_state );
    
    bool end_of_transmission_received;

    switch( receive_current_state )
    {
	case enum_receive_status_waiting:
	    break;
	case enum_receive_status_in_command:
	    break;
	case enum_receive_status_end_command:
        
	    if( process_data( &receive_buffer, &send_buffer, xSumMatches(receive_buffer) ) == true )
	    {
		end_of_transmission_received = true;
	    }
	    break;
    }
    // TODO see that sending keeps going

    communicationsSend( &send_buffer, enum_port_commUART1 );

    return;
}

// Handles the UART functionality for UART2, for the the initial and the continuous running processes
void communicationsUART2( bool initialize )
{
    static struct buffer_struct send_buffer;
    static struct buffer_struct receive_buffer;
    static enum receive_status_enum receive_current_state = enum_receive_status_waiting;

    if( initialize == true )
    {
	send_buffer.port = enum_port_commUART2;
	send_buffer.read_position = 0;
	send_buffer.write_position = 0;
	receive_buffer.read_position = 0;
	receive_buffer.write_position = 0;
	receive_current_state = enum_receive_status_waiting;
    }

    receive_current_state = communicationsRecv( &receive_buffer, &send_buffer, enum_port_commUART2, &receive_current_state );

    bool end_of_transmission_received;

    switch( receive_current_state )
    {
	case enum_receive_status_waiting:
	    break;
	case enum_receive_status_in_command:
	    break;
	case enum_receive_status_end_command:
        
	    if( process_data( &receive_buffer, &send_buffer, xSumMatches(receive_buffer) ) == true )
	    {
		end_of_transmission_received = true;
	    }
	    break;
    }

    communicationsSend( &send_buffer, enum_port_commUART2 );

    return;
}

// pulls the received information into buffers
bool communicationsRecv( struct buffer_struct *receive_buffer, struct buffer_struct *send_buffer, enum communications_port_enum communicationsPort, enum receive_status_enum *receive_current_state )
{

    bool data_received;

    char data;

    if( *receive_current_state == enum_receive_status_end_command )
    {
	*receive_current_state = enum_receive_status_waiting;
    }

    data_received = false;

    bool gotSomething = false;


    // the UART automatically buffers 4 characters
    // unlikely there will be more than one character in the buffer since
    // there is no blocking code, but ideally we would clean out the buffer with each run
    // right now this does not happen
    switch( communicationsPort )
    {
	case enum_port_commSPI:
	    gotSomething = SPI_receive_data_char( &data );
	    break;
	case enum_port_commUART1:
	    gotSomething = UART1_receive_data_char( &data );
	    break;
	case enum_port_commUART2:
	    gotSomething = UART2_receive_data_char( &data );
	    break;
    }

    if( gotSomething == true )
    {
	data_received = true;
    
	if( (data == COMMAND_START_CHAR) && (*receive_current_state != enum_receive_status_in_command) )
	{
	    *receive_current_state = enum_receive_status_in_command;
	    receive_buffer->read_position = 0;
	    receive_buffer->write_position = 0;
	}

	if( *receive_current_state == enum_receive_status_in_command )
	{
	    receive_buffer->data_buffer[ receive_buffer->write_position] = data;
        
	    receive_buffer->write_position++;
	    if( receive_buffer->write_position >= BUFFER_LENGTH )
	    {
		receive_buffer->write_position = (BUFFER_LENGTH - 1);
	    }
	}

	if( (*receive_current_state == enum_receive_status_in_command) && (data == COMMAND_END_CHAR) )
	{
	    *receive_current_state = enum_receive_status_end_command;
	}
    }
    
    // maybe we do not need to return the status since we modify the function variable
    return data_received;
}

// Will send information through any port
bool communicationsSend( struct buffer_struct *send_buffer, enum communications_port_enum communicationsPort )
{
    bool send_end;
    
    if( send_buffer->read_position == send_buffer->write_position )
    {
	send_end = true;
    }
    else
    {
	send_end = false;
    
    bool data_sent;
	switch( communicationsPort )
	{
	    case enum_port_commSPI:
		data_sent = SPI_send_data_char( send_buffer->data_buffer[send_buffer->read_position] );
        break;
	    case enum_port_commUART1:       
        data_sent = UART1_send_data_char( send_buffer->data_buffer[send_buffer->read_position] );
        break;
	    case enum_port_commUART2:
		data_sent = UART2_send_data_char( send_buffer->data_buffer[send_buffer->read_position] );
		break;
	}
	if( data_sent == true )
	{
	    send_buffer->read_position++;
	    if( send_buffer->read_position >= BUFFER_LENGTH )
	    {
		send_buffer->read_position = 0;
	    }
	}
    }

    return send_end;

}

// Handles Chip Select Functionality
bool set_current_port( unsigned char *current_port )
{
    static bool enabledSPI = true;

    if( enabledSPI == true )
    {
	SPI1STATbits.SPIEN = 0; //disable master SPI
	enabledSPI = false;

	SPI_PORT_0 = 1; //disable slave select (1 is disabled)
	SPI_PORT_1 = 1; //disable slave select (1 is disabled)
	SPI_PORT_2 = 1; //disable slave select (1 is disabled)
    }
    else
    {
	(*current_port)++;
	if( *current_port >= PORT_COUNT )
	{
	    *current_port = 0;
	}
	switch( *current_port )
	{
	    case 0:
		// set correct DO chip select here
		SPI_PORT_0 = 0; //enable Slave Select
		break;
	    case 1:
		// set correct DO the chip select here
		SPI_PORT_1 = 0;

		//	    LED4___SET = 1;

		break;
	    case 2:
		// set correct DO the chip select here
		SPI_PORT_2 = 0;
		break;
	}

	SPI1STATbits.SPIEN = 1; //enable master SPI
	enabledSPI = true;
    }

    return enabledSPI;
}


// Passes received data strings into parsing functions
bool process_data( struct buffer_struct *receive_buffer, struct buffer_struct *send_buffer, bool xSumMatches )
{
    bool end_of_transmission_received;

    // if we are here then the receive buffer must have good data with start and end command characters
    // the characters are not included as they were not added

    char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH];

    process_data_parameterize( parameters, receive_buffer );

        
    if (xSumMatches){
         ledTestSetOn(4);
         ledTestSetOff(3);
        end_of_transmission_received = process_data_parameters( parameters, send_buffer );

    }
    else{
         ledTestSetOff(4);
         ledTestSetOn(3);
        end_of_transmission_received = true;
    }

    return end_of_transmission_received;
}

// Split the full strings into lists
void process_data_parameterize( char parameters[][PARAMETER_MAX_LENGTH], struct buffer_struct *buffer_to_parameterize )
{
    unsigned char parameter_position = 0;
    unsigned char parameter_index = 0;


    // only one command is expected due to the way we read
    // go through buffer until we hit end char or end of buffer

    // this is super important - we must initialize the entire array
    // if we do not we risk passing junk into some functions that assume strings and check for NULL
    // without NULL a string function could run forever until we die from old age
    // even then it would keep running
    for( int inx = 0; inx < PARAMETER_MAX_COUNT; inx++ )
    {
	parameters[inx][0] = CHAR_NULL;
    }

    while( (buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position ] != COMMAND_END_CHAR) && (buffer_to_parameterize->read_position < BUFFER_LENGTH) && (buffer_to_parameterize->read_position != buffer_to_parameterize->write_position) )
    {
	switch( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position] )
	{
	    case COMMAND_START_CHAR:
		// this character should never appear
		break;
	    case COMMAND_DELIMETER:
		// move to next parameter
		parameter_position = 0;
		parameter_index++;

		if( parameter_index >= PARAMETER_MAX_COUNT )
		{
		    // if we run out of parameters just overwrite the last one
		    // we should never have this case, but this keeps us from crashing and burning
		    parameter_index = (PARAMETER_MAX_COUNT - 1);
		}

		break;
	    default:
		// add the character to the current parameter
		parameters[parameter_index][parameter_position] = buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position];
		parameter_position++;
		if( parameter_position >= PARAMETER_MAX_LENGTH )
		{
		    // if our parameter is too long, just overwrite the last character
		    // we should never have this case, but this keeps us from crashing and burning
		    parameter_position = (PARAMETER_MAX_LENGTH - 1);
		}
		parameters[parameter_index][parameter_position] = CHAR_NULL;
		break;
	}
	buffer_to_parameterize->read_position++;
    }

    buffer_to_parameterize->read_position = 0;
    buffer_to_parameterize->write_position = 0;

    return;
}

// Read all of the parameters to find out what the command says to do
bool process_data_parameters( char parameters[][PARAMETER_MAX_LENGTH], struct buffer_struct *send_buffer )
{
    bool end_of_transmission_received = false;

    // the 'commands' shown here are for example only
    // make them whatever is needed

    // ideally, any new commands are set in a separate function called from one of these tests
    // it's not very clean to call the command builder functions from here
    // especially if there is some processing to do, like setting a clock or something


    if( strmatch( parameters[0], "END" ) == true )
    {

	send_end_of_transmission( send_buffer );
	end_of_transmission_received = true;

    }
    else if( strmatch( parameters[0], "Set" ) == true )
    {
	if( strmatch( parameters[1], "Time" ) == true )
	{
	    char timeDayBuf[3];
	    char timeMonthBuf[3];
	    char timeYearBuf[3];

	    char timeHourBuf[3];
	    char timeMinuteBuf[3];
	    char timeSecondBuf[3];

	    // DD-MM-YY
	    timeDayBuf[0] = parameters[2][0];
	    timeDayBuf[1] = parameters[2][1];
	    timeDayBuf[2] = CHAR_NULL;

	    timeMonthBuf[0] = parameters[2][3];
	    timeMonthBuf[1] = parameters[2][4];
	    timeMonthBuf[2] = CHAR_NULL;

	    timeYearBuf[0] = parameters[2][6];
	    timeYearBuf[1] = parameters[2][7];
	    timeYearBuf[2] = CHAR_NULL;

	    // HH:MM:SS
	    timeHourBuf[0] = parameters[3][0];
	    timeHourBuf[1] = parameters[3][1];
	    timeHourBuf[2] = CHAR_NULL;

	    timeMinuteBuf[0] = parameters[3][3];
	    timeMinuteBuf[1] = parameters[3][4];
	    timeMinuteBuf[2] = CHAR_NULL;

	    timeSecondBuf[0] = parameters[3][6];
	    timeSecondBuf[1] = parameters[3][7];
	    timeSecondBuf[2] = CHAR_NULL;

	    struct date_time newDateTime;

	    newDateTime.day = atoi( timeDayBuf );
	    newDateTime.month = atoi( timeMonthBuf );
	    newDateTime.year = atoi( timeYearBuf );

	    newDateTime.hour = atoi( timeHourBuf );
	    newDateTime.minute = atoi( timeMinuteBuf );
	    newDateTime.second = atoi( timeSecondBuf );

	    // set the RTCC I2C and then copy it to the RTCC Internal
	    // this will help verify that they are the same
	    rtccI2CSetTime( &newDateTime );
	    rtccCopyI2CTime( );

	    command_builder2( send_buffer, "Conf", "Time" );

	}
	else if( strmatch( parameters[1], "EnAl" ) == true )
	{
	    energyCycleAllocation_global = atol( parameters[2] );

	    eeWriteEnergyAllocNew( energyCycleAllocation_global );

	    command_builder2( send_buffer, "Conf", "EnAl" );
	}
	else if( strmatch( parameters[1], "Alarm" ) == true )
	{
	    //0 "set"
	    //1 "Alarm"
	    //2 audibleAlarmBuf - On Off
	    //3 alarm1EnabledBuf - On Off
	    //4 alarm1PowerBuf - int
	    //5 alarm2EnabledBuf - On Off
	    //6 alarm2PowerBuf - int

	    if( checkOnOff( parameters[2] ) == true )
	    {
		alarms_global.alarmAudible = 1;
	    }
	    else
	    {
		alarms_global.alarmAudible = 0;
	    }


	    if( checkOnOff( parameters[3] ) == true )
	    {
		alarms_global.alarm1Enabled = 1;
	    }
	    else
	    {
		alarms_global.alarm1Enabled = 0;
	    }

	    alarms_global.alarm1Energy = atoi( parameters[4] );


	    if( checkOnOff( parameters[5] ) == true )
	    {
		alarms_global.alarm2Enabled = 1;
	    }
	    else
	    {
		alarms_global.alarm2Enabled = 0;
	    }

	    alarms_global.alarm2Energy = atoi( parameters[6] );

	    eeWriteAlarmNew( alarms_global );

	    command_builder2( send_buffer, "Conf", "Alarm" );
	}
	else if( strmatch( parameters[1], "Pass" ) == true )
	{

	    char passwordNew[7];

	    passwordNew[0] = parameters[2][0];
	    passwordNew[1] = parameters[2][1];
	    passwordNew[2] = parameters[2][2];
	    passwordNew[3] = parameters[2][3];
	    passwordNew[4] = parameters[2][4];
	    passwordNew[5] = parameters[2][5];

	    eeWritePasswordNew( passwordNew );

	    command_builder2( send_buffer, "Conf", "Pass" );

	}
	else if( strmatch( parameters[1], "Emer" ) == true )
	{

	    if( checkOnOff( parameters[2] ) == true )
	    {
		emergencyButton_global.enabled = true;
		emergencyButton_global.energyAmount = atoi( parameters[3] );
	    }
	    else
	    {
		emergencyButton_global.enabled = false;
		emergencyButton_global.energyAmount = 0;
	    }

	    eeWriteEmerButtonNew( emergencyButton_global );

	    command_builder2( send_buffer, "Conf", "Emer" );

	}
	else if( strmatch( parameters[1], "RstTim" ) == true )
	{


	    resetTime_global.hour = atoi( parameters[2] );
	    resetTime_global.minute = atoi( parameters[3] );

	    char rsh[20];
	    char rsm[20];

	    itoa( rsh, resetTime_global.hour, 10 );
	    itoa( rsm, resetTime_global.minute, 10 );
	    eeWriteResetTimeNew( resetTime_global.hour, resetTime_global.minute );

	    command_builder4( send_buffer, "Conf", "RstTim", rsh, rsm );

	}
	else if( strmatch( parameters[1], "Relay" ) == true )
	{

	    if( checkOnOff( parameters[2] ) == true )
	    {
		relayActive_global = 1;
	    }
	    else
	    {
		relayActive_global = 0;
	    }

	    eeWriteRelayNew( relayActive_global );

	    command_builder2( send_buffer, "Conf", "Relay" );

	}
	    //	else if( strmatch( parameters[1], "DebugMode" ) )

	else if( strmatch( parameters[1], "Watts" ) == true )
	{
        
	    powerWatts_global = atol( parameters[2] );
	    command_builder2( send_buffer, "Conf", "Watts" );
        
        // TODO testing
        if( powerWatts_global == 789)
        {
            ledTestSetOn(1);
            ledTestSetOff(2);
        }
        else
        {
            ledTestSetOff(1);
            ledTestSetOn(2);
        }
       
        
	}
	else if( strmatch( parameters[1], "EnUsed" ) == true )
	{
	    // the lifetime energy is currently stored in the command board EEPROM
	    // power sense at power-up has lifetime energy at 0
	    // if power sense lifetime energy is < command board lifetime energy we must be in start-up
	    // send power sense new lifetime energy value

	    unsigned long tempEnergyUsedLifetime;

	    tempEnergyUsedLifetime = atol( parameters[2] );

	    if( tempEnergyUsedLifetime < energyUsed_global.lifetime )
	    {

		char temp[12];
		ltoa( temp, energyUsed_global.lifetime, 10 );
		command_builder3( send_buffer, "Set", "EnUsed", temp );

	    }
	    else
	    {
		energyUsed_global.lifetime = tempEnergyUsedLifetime;
		command_builder2( send_buffer, "Conf", "EnUsed" );
	    }

	}
	else if( strmatch( parameters[1], "Lights" ) == true )
	{

	    ledFindMeActive_global = checkOnOff( parameters[2] );

	    command_builder2( send_buffer, "Conf", "Lights" );

	}
	else if( strmatch( parameters[1], "AllAdd" ) == true )
	{

	    energyAdd_global = atoi( parameters[2] );

	    char buf[BUF_SIZE_INT];
	    itoa( buf, energyAdd_global, 10 );

	    command_builder3( send_buffer, "Conf", "AllAdd", buf );

	}
	else if( strmatch( parameters[1], "PSVersion" ) == true )
	{
	    command_builder2( send_buffer, "Conf", "PSVersion" );
	}
    }
    else if( strmatch( parameters[0], "Read" ) == true )
    {
	if( strmatch( parameters[1], "Time" ) == true )
	{
	    char timeDateBuf[9]; //	"DD-MM-YY"
	    char timeTimeBuf[9]; //	"HH:MM:SS"

	    char timeDateDDBuf[BUF_SIZE_INT];
	    char timeDateMMBuf[BUF_SIZE_INT];
	    char timeDateYYBuf[BUF_SIZE_INT];

	    char timeTimeHHBuf[BUF_SIZE_INT];
	    char timeTimeMMBuf[BUF_SIZE_INT];
	    char timeTimeSSBuf[BUF_SIZE_INT];

	    zeroPad_itoa( timeDateDDBuf, dateTime_global.day, 2 );
	    zeroPad_itoa( timeDateMMBuf, dateTime_global.month, 2 );
	    zeroPad_itoa( timeDateYYBuf, dateTime_global.year, 2 );
	    zeroPad_itoa( timeTimeHHBuf, dateTime_global.hour, 2 );
	    zeroPad_itoa( timeTimeMMBuf, dateTime_global.minute, 2 );
	    zeroPad_itoa( timeTimeSSBuf, dateTime_global.second, 2 );
  
	    timeDateBuf[0] = timeDateDDBuf[0];
	    timeDateBuf[1] = timeDateDDBuf[1];
	    timeDateBuf[2] = '-';
	    timeDateBuf[3] = timeDateMMBuf[0];
	    timeDateBuf[4] = timeDateMMBuf[1];
	    timeDateBuf[5] = '-';
	    timeDateBuf[6] = timeDateYYBuf[0]; // pull from 4 digit year
	    timeDateBuf[7] = timeDateYYBuf[1]; // pull from 4 digit year
	    timeDateBuf[8] = CHAR_NULL;
        
	    timeTimeBuf[0] = timeTimeHHBuf[0];
	    timeTimeBuf[1] = timeTimeHHBuf[1];
	    timeTimeBuf[2] = ':';
	    timeTimeBuf[3] = timeTimeMMBuf[0];
	    timeTimeBuf[4] = timeTimeMMBuf[1];
	    timeTimeBuf[5] = ':';
	    timeTimeBuf[6] = timeTimeSSBuf[0];
	    timeTimeBuf[7] = timeTimeSSBuf[1];
	    timeTimeBuf[8] = CHAR_NULL;

	    command_builder4( send_buffer, "Set", "Time", timeDateBuf, timeTimeBuf );

	}
	else if( strmatch( parameters[1], "EnAl" ) == true )
	{
	    char energyAllocationBuf[BUF_SIZE_LONG];

	    ltoa( energyAllocationBuf, energyCycleAllocation_global, 10 );

	    command_builder3( send_buffer, "Set", "EnAl", energyAllocationBuf );
	}
	else if( strmatch( parameters[1], "Alarm" ) == true )
	{

	    // send the current alarm parameters
	    char audibleAlarmBuf[4];
	    char alarm1EnabledBuf[4];
	    char alarm2EnabledBuf[4];

	    int alarm1EnergyTemp;
	    int alarm2EnergyTemp;
	    char alarm1EnergyBuf[BUF_SIZE_INT];
	    char alarm2EnergyBuf[BUF_SIZE_INT];

	    fillOnOff( audibleAlarmBuf, alarms_global.alarmAudible );
	    fillOnOff( alarm1EnabledBuf, alarms_global.alarm1Enabled );
	    fillOnOff( alarm2EnabledBuf, alarms_global.alarm2Enabled );

	    // using itoa() - variable type is char, make sure it is an int
	    alarm1EnergyTemp = alarms_global.alarm1Energy;
	    alarm2EnergyTemp = alarms_global.alarm2Energy;
	    itoa( alarm1EnergyBuf, alarm1EnergyTemp, 10 );
	    itoa( alarm2EnergyBuf, alarm2EnergyTemp, 10 );

	    command_builder7( send_buffer, "Set", "Alarm", audibleAlarmBuf, alarm1EnabledBuf, alarm1EnergyBuf, alarm2EnabledBuf, alarm2EnabledBuf );

	}
	else if( strmatch( parameters[1], "Pass" ) == true )
	{
	    char password[7];

	    eeReadPasswordNew( password );
	    password[6] = CHAR_NULL;

	    command_builder3( send_buffer, "Set", "Pass", password );

	}
	else if( strmatch( parameters[1], "Emer" ) == true )
	{
	    char emerButtonEnableBuf[4];
	    char emerButtonEnergyAllocateBuf[BUF_SIZE_INT];

	    fillOnOff( emerButtonEnableBuf, emergencyButton_global.enabled );

	    itoa( emerButtonEnergyAllocateBuf, emergencyButton_global.energyAmount, 10 );

	    command_builder4( send_buffer, "Set", "Emer", emerButtonEnableBuf, emerButtonEnergyAllocateBuf );

	}
	else if( strmatch( parameters[1], "RstTim" ) == true )
	{

	    char resetTimeHourBuf[BUF_SIZE_INT];
	    char resetTimeMinuteBuf[BUF_SIZE_INT];


	    itoa( resetTimeHourBuf, resetTime_global.hour, 10 );
	    itoa( resetTimeMinuteBuf, resetTime_global.minute, 10 );

	    command_builder4( send_buffer, "Set", "RstTim", resetTimeHourBuf, resetTimeMinuteBuf );

	}
	else if( strmatch( parameters[1], "Relay" ) == true )
	{
	    char relayActiveBuf[4];

	    fillOnOff( relayActiveBuf, relayActive_global );

	    command_builder3( send_buffer, "Set", "Relay", relayActiveBuf );

	}
	else if( strmatch( parameters[1], "Stat" ) == true )
	{
	    char charEnergyUsedLifetime[BUF_SIZE_LONG];
	    char charEnergyUsedPreviousDay[BUF_SIZE_LONG];

	    ltoa( charEnergyUsedLifetime, energyUsed_global.lifetime, 10 );
	    ltoa( charEnergyUsedPreviousDay, energyUsed_global.previousCycleUsed, 10 );

	    command_builder4( send_buffer, "Set", "Stat", charEnergyUsedLifetime, charEnergyUsedPreviousDay );

	}
	else if( strmatch( parameters[1], "CBver" ) == true )
	{

	    char temp[10];
	    temp[0] = 'T';
	    temp[1] = 'e';
	    temp[2] = 's';
	    temp[3] = 't';
	    temp[4] = ':';
	    temp[5] = '0';
	    temp[6] = '1';
	    temp[7] = CHAR_NULL;

	    command_builder3( send_buffer, "Set", "CBver", powerBoxCodeVersion_global );

	}
	else if( strmatch( parameters[1], "PwrFail" ) == true )
	{

	    char powerFailTimeBuf[12];
	    char powerRestoreTimeBuf[12];
        char temp[12];


	    powerFailTimeBuf[0] = 'H';
	    powerFailTimeBuf[1] = 'H';
	    powerFailTimeBuf[2] = ':';
	    powerFailTimeBuf[3] = 'M';
	    powerFailTimeBuf[4] = 'M';
	    powerFailTimeBuf[5] = ':';
	    powerFailTimeBuf[6] = 'D';
	    powerFailTimeBuf[7] = 'D';
	    powerFailTimeBuf[8] = ':';
	    powerFailTimeBuf[9] = 'M';
	    powerFailTimeBuf[10] = 'M';
	    powerFailTimeBuf[11] = CHAR_NULL;

	    powerRestoreTimeBuf[0] = 'H';
	    powerRestoreTimeBuf[1] = 'H';
	    powerRestoreTimeBuf[2] = ':';
	    powerRestoreTimeBuf[3] = 'M';
	    powerRestoreTimeBuf[4] = 'M';
	    powerRestoreTimeBuf[5] = ':';
	    powerRestoreTimeBuf[6] = 'D';
	    powerRestoreTimeBuf[7] = 'D';
	    powerRestoreTimeBuf[8] = ':';
	    powerRestoreTimeBuf[9] = 'M';
	    powerRestoreTimeBuf[10] = 'M';
	    powerRestoreTimeBuf[11] = CHAR_NULL;

	    struct date_time timePowerFail;
	    struct date_time timePowerRestore;

	    rtccI2CReadPowerTimes( &timePowerFail, &timePowerRestore );

//	    command_builder4( send_buffer, "Set", "PwrFail", powerFailTimeBuf, powerRestoreTimeBuf ); /// Sends the placeholder time
        
        /////////////////////////////////////////////////// BEGIN NEW CODE
        powerRestoreTimeBuf[0] = timePowerFail.minuteTens;
//	    powerRestoreTimeBuf[1] = timePowerFail.minute;
	    powerRestoreTimeBuf[2] = 'r';
	    powerRestoreTimeBuf[3] = 'o';
	    powerRestoreTimeBuf[4] = 'i';
	    powerRestoreTimeBuf[5] = 's';
	    powerRestoreTimeBuf[6] = 's';
	    powerRestoreTimeBuf[7] = 'a';
	    powerRestoreTimeBuf[8] = 'n';
	    powerRestoreTimeBuf[9] = 't';
	    powerRestoreTimeBuf[10] = 's';
	    powerRestoreTimeBuf[11] = CHAR_NULL;
        
        
        command_builder4( send_buffer, "Set", "PwrFail", powerFailTimeBuf, powerRestoreTimeBuf );
        //////////////////////////////////////////////////// END NEW CODE
	}
	else if( strmatch( parameters[1], "PwrData" ) == true )
	{
	    long energyUsedTemp;

	    energyUsedTemp = energyUsed_global.lifetime - energyUsed_global.lastReset;

	    char energyEmergencyAdderBuf[BUF_SIZE_LONG];
	    char energyUsedBuf[BUF_SIZE_LONG];
	    char powerWattsBuf[BUF_SIZE_LONG];

	    ltoa( energyEmergencyAdderBuf, (energyCycleAllocation_global + energyAdd_global), 10 );
	    ltoa( energyUsedBuf, energyUsedTemp, 10 );
	    ltoa( powerWattsBuf, powerWatts_global, 10 );

        
        
        
	    command_builder5( send_buffer, "Set", "PwrData", energyEmergencyAdderBuf, energyUsedBuf, powerWattsBuf );
	}
    }

    return end_of_transmission_received;
}

void command_builder1( struct buffer_struct *send_buffer, char* data1 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    int xsum = 0;
    xsum += command_builder_add_string( send_buffer, data1 );
    xsum_builder( send_buffer, xsum );
    return;
}

void command_builder2( struct buffer_struct *send_buffer, char* data1, char* data2 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    int xsum = 0;
    xsum += command_builder_add_string( send_buffer, data1 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data2 );
    
    xsum_builder( send_buffer, xsum );
    
    return;
}

void command_builder3( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    int xsum = 0;
    xsum += command_builder_add_string( send_buffer, data1 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data2 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data3 );
    xsum_builder( send_buffer, xsum );


    return;
}

void command_builder4( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    int xsum = 0;
    xsum += command_builder_add_string( send_buffer, data1 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data2 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data3 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data4 );
    xsum_builder( send_buffer, xsum );


    return;
}

void command_builder5( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    int xsum = 0;
    xsum += command_builder_add_string( send_buffer, data1 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data2 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data3 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data4 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data5 );
    xsum_builder( send_buffer, xsum );


    return;
}

void command_builder6( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    int xsum = 0;
    xsum += command_builder_add_string( send_buffer, data1 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data2 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data3 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data4 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data5 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data6 );
    xsum_builder( send_buffer, xsum );

    return;
}

void command_builder7( struct buffer_struct *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6, char* data7 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    int xsum = 0;
    xsum += command_builder_add_string( send_buffer, data1 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data2 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data3 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data4 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data5 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data6 );
    xsum += command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    xsum += command_builder_add_string( send_buffer, data7 );
    xsum_builder( send_buffer, xsum );

    return;
}

void xsum_builder( struct buffer_struct *send_buffer, int xsum ){
 
    command_builder_add_char( send_buffer, COMMAND_DELIMETER ); // REMOVE THIS ONCE XSUM CHECK IS IMPLEMENTED
    command_builder_add_char( send_buffer, XSUM_DELIMETER ); //@
    char xsumBuf[16]; //allocate space for XSUM
    itoa( xsumBuf, xsum, 10 ); //convert XSUM from int into to string.
    command_builder_add_string( send_buffer, xsumBuf ); //add XSUM to send buffer 

    command_builder_add_char( send_buffer, COMMAND_END_CHAR );
    return;
}

int command_builder_add_char( struct buffer_struct *send_buffer, char data )
{
    send_buffer->data_buffer[send_buffer->write_position] = data;

    send_buffer->write_position++;
    if( send_buffer->write_position >= BUFFER_LENGTH )
    {
	send_buffer->write_position = 0;
    }
    
    return data;
}

int command_builder_add_string( struct buffer_struct *send_buffer, char *data_string )
{
    int xsum = 0;
    for( int inx = 0; data_string[inx] != CHAR_NULL; inx++ )
    {
	xsum += command_builder_add_char( send_buffer, data_string[inx] );
    }    
    return xsum;

}

bool strmatch( char* a, char* b )
{
    int result;
    bool match;

    static int co = 0;
    co++;

    result = strcmp2( a, b );

    if( result == 0 )
    {
	match = true;
    }
    else
    {
	match = false;
    }

    return match;
}

int strcmp2( char* a, char* b )
{
    int inx = 0;
    int match = 0;

    while( (a[inx] != CHAR_NULL) && (b[inx] != CHAR_NULL) && (match == 0) )
    {
	if( a[inx] > b[inx] )
	{
	    match = 1;
	}
	else if( a[inx] < b[inx] )
	{
	    match = -1;
	}
	else if( a[inx] == b[inx] )
	{
	    //do nothing
	}

	inx++;
    }

    if( (a[inx] == CHAR_NULL) && (b[inx] != CHAR_NULL) )
    {
	match = -1;
    }
    else if( (a[inx] != CHAR_NULL) && (b[inx] == CHAR_NULL) )
    {
	match = 1;
    }


    return match;
}

void fillOnOff( char *buf, int checkValue )
{
    if( checkValue == 0 )
    {
	buf[0] = 'O';
	buf[1] = 'f';
	buf[2] = 'f';
	buf[3] = CHAR_NULL;

    }
    else
    {
	buf[0] = 'O';
	buf[1] = 'n';
	buf[2] = CHAR_NULL;
	buf[3] = CHAR_NULL;
    }

    return;
}

bool checkOnOff( char *toCheck )
{
    bool isOn = false;

    if( strmatch( toCheck, "On" ) == true )
    {
	isOn = true;
    }
    else if( strmatch( toCheck, "Off" ) == true )
    {
	isOn = false;
    }

    return isOn;
}

void zeroPad_itoa( char *output, int num, int minDigits )
{
    char temp[BUF_SIZE_INT];

    int rawLen;

    itoa( temp, num, 10 );

    rawLen = 0;
    while( temp[rawLen] != CHAR_NULL )
    {
	rawLen++;
    }

    // rawLen now contains the length of the converted number

    int padding;

    padding = minDigits - rawLen;

    int inxOutput;
    for( inxOutput = 0; inxOutput < padding; inxOutput++ )
    {
	output[inxOutput] = '0';
    }

    int inxTemp;
    inxTemp = 0;
    while( temp[inxTemp] != CHAR_NULL )
    {
	output[inxOutput] = temp[inxTemp];
	inxOutput++;
	inxTemp++;
    }
    output[inxOutput ] = CHAR_NULL;


    return;
}

bool SPI_receive_data_char( char *data )
{
    bool recvGood = false;

    if( SPIRecvBufferReadPos != SPIRecvBufferWritePos )
    {
        *data = SPIRecvBuffer[SPIRecvBufferReadPos];
        if(*data != CHAR_NULL)
        {
//            ledToggle(4);
            recvGood = true;
        }
        SPIRecvBufferReadPos++;
        if(SPIRecvBufferReadPos >= BUFFER_LENGTH_RECV){
            SPIRecvBufferReadPos = 0;
        }
    }
    
    return recvGood;
}

//bool SPI_receive_data_char( char *data )
//{
//    bool recvGood = false;
////
////    if( SPI1STATbits.SPIRBF == 1 )
////    {
////	
////	recvGood = true;
////    }
//
//    return recvGood;
//}

bool SPI_send_data_char( char data )
{
    bool sendGood = false;

    if( SPI1STATbits.SPITBF == 0 ) //if in enhance mode use SPI1STATbits.SR1MPT
    {
        SPI1BUF = data;
        sendGood = true;
//        ledToggle(3);
    }

    return sendGood;
}

//bool UART1_receive_data_char( char *data ) // NON-INTERRUPT VERSION
//{
//    bool recvGood = false;
//
//
//    if( U1STAbits.URXDA == 1 )
//    {
//	*data = U1RXREG;
//
//	if( *data != CHAR_NULL )
//	{
//	    recvGood = true;
//	}
//    }
//
//    return recvGood;
//}


bool UART1_receive_data_char( char *data )
{
    bool recvGood = false;


    if( UARTRecvBufferReadPos != UARTRecvBufferWritePos )
    {
        *data = UARTRecvBuffer[UARTRecvBufferReadPos];
        if(*data != CHAR_NULL)
        {
            recvGood = true;
        }
        UARTRecvBufferReadPos++;
        if(UARTRecvBufferReadPos >= BUFFER_LENGTH_RECV){
            UARTRecvBufferReadPos = 0;
        }
    }
    
    return recvGood;
}

bool UART1_send_data_char( char data )
{
    bool sendGood = false;

    if( U1STAbits.UTXBF == 0 )
    {
	U1TXREG = data;
	sendGood = true;
    }

    return sendGood;
}

bool UART2_receive_data_char( char *data )
{
    bool recvGood = false;


    if( U2STAbits.URXDA == 1 )
    {
	*data = U2RXREG;
	recvGood = true;
    }

    return recvGood;
}

bool UART2_send_data_char( char data )
{
    bool sendGood = false;

    if( U2STAbits.UTXBF == 0 )
    {
	U2TXREG = data;
	sendGood = true;
    }

    return sendGood;
}


/************************/
// RESPONSES

void send_end_of_transmission( struct buffer_struct * send_buffer )
{
    command_builder1( send_buffer, "END" );

    return;
}

void commSPIInit( void )
{

    TRISBbits.TRISB10 = 0b1; // SDI1
    TRISBbits.TRISB11 = 0b1; // SCK1 (seems this should be set output, but 0b0 does not work)
    TRISBbits.TRISB13 = 0b0; // SDO1

    SPI_PORT_0_DIR = 0;
    SPI_PORT_1_DIR = 0;
    SPI_PORT_2_DIR = 0;

    SPI_PORT_0 = 1;
    SPI_PORT_1 = 1;
    SPI_PORT_2 = 1;

    //SPI1 Initialize
    SPI1CON1bits.MSTEN = 1; //making SPI1 Master

    SPI1CON1bits.DISSCK = 0b0; // SPI clock enabled
    SPI1CON1bits.DISSDO = 0b0; // SDO used
    SPI1CON1bits.MODE16 = 0b0; // 8 bit mode
    SPI1CON1bits.SMP = 0b1; // sample phase mode end
    SPI1CON1bits.CKE = 0b1; // serial data changes on active to idle clock state
    SPI1CON1bits.SSEN = 0b0; // not a slave
    SPI1CON1bits.CKP = 0b1; // clock idle is high

    // set the SPI clock speed
    // this is not simple to do based off calculations using the FCY macro
    // for now, just hadn calcualte and set the correct bits

    // start at 16,000,000 Hz
    // PRI prescale 16:1 - takes it down to 1,000,000 Hz
    // SEC prescale 4:1 - takes it down to 250,000 Hz

    SPI1CON1bits.PPRE = 0b01; // primary prescale 16:1
    SPI1CON1bits.SPRE = 0b100; // secondary prescale 4:1


    SPI1CON2bits.FRMEN = 0b0; // frame mode, unused
    SPI1CON2bits.SPIFSD = 0b0; // frame mode, unused
    SPI1CON2bits.SPIFPOL = 0b0; // frame mode, unused
    SPI1CON2bits.SPIFE = 0b0; // frame mode, unused

    SPI1CON2bits.SPIBEN = 0b0; // 1=enhanced buffer mode

    SPI1STATbits.SPIROV = 0; //clear flag for overflow data
    SPI1STATbits.SISEL = 0b001; // Interrupt when SPIx receive buffer is full
    
    IFS0bits.SPI1IF = 0; // clear RX interrupt flag
    IEC0bits.SPI1IE = 1; // enable RX interrupt

    return;
}

void uartInit( void )
{

    initUART1( );
    initUART2( );

}

void initUART2( void )
{
    TRISBbits.TRISB1 = 1; // U2RX
    TRISBbits.TRISB0 = 0; // U2TX

    //shooting for BAUD_UART (typically 9600))
    // baud = FCY / ( 16 / (U1BRG + ))
    //U1BRG = ( FCY / (16 * baud)) - 1

    // do the math one step at a time
    // to explicitly control the order of operations
    // otherwise we could have a overflow or underflow
    // without really knowing it
    unsigned long tempBaud;

    tempBaud = BAUD_UART * 16ul; // must explicitly make unsigned long
    tempBaud = FCY / tempBaud;
    tempBaud = tempBaud - 1;

    U2BRG = tempBaud;


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

    // we do not need the interrupt on receive
    // set these bits to just make sure they are in a known state
    // interesting that the UTXISEL does not exist - we need to set each bit separately
    U2STAbits.UTXISEL0 = 0b0; // transmit interrupt
    U2STAbits.UTXISEL1 = 0b0; // Must be 0b0 (large integer implicitly truncated to unsigned type)

    U2STAbits.UTXINV = 0b0;
    U2STAbits.UTXBRK = 0b0;
    U2STAbits.URXISEL = 0b00; // interrupt after one RX character is received
    U2STAbits.ADDEN = 0;

    // we need the error interrupt to clear errors - without this it does not work
    IFS4bits.U2ERIF = 0; // clear Error Flag
    IEC4bits.U2ERIE = 1; // enable Error Interrupt

    IFS1bits.U2TXIF = 0; // clear TX Flag
    IEC1bits.U2TXIE = 0; // disable TX Interrupt

    IFS1bits.U2RXIF = 0; // clear RX interrupt flag
    IEC1bits.U2RXIE = 0; // disable RX interrupt

    U2MODEbits.UARTEN = 0b1; // turn it on
    U2STAbits.UTXEN = 0b1; // enable transmit

    return;
}

void initUART1( void )
{
    TRISBbits.TRISB2 = 1; // U1RX
    TRISBbits.TRISB7 = 0; // U1TX

    //shooting for BAUD_UART (typically 9600))
    // baud = FCY / ( 16 / (U1BRG + ))
    //U1BRG = ( FCY / (16 * baud)) - 1

    // do the math one step at a time
    // to explicitly control the order of operations
    // otherwise we could have a overflow or underflow
    // without really knowing it
    unsigned long tempBaud;

    tempBaud = BAUD_UART * 16ul; // must explicitly make unsigned long
    tempBaud = FCY / tempBaud;
    tempBaud = tempBaud - 1;

    U1BRG = tempBaud;

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

    // we do not need the interrupt on receive
    // set these bits to just make sure they are in a known state
    // interesting that the UTXISEL does not exist - we need to set each bit separately
    U1STAbits.UTXISEL0 = 0b0; // transmit interrupt
    U1STAbits.UTXISEL1 = 0b0; // Must be 0b0 (large integer implicitly truncated to unsigned type)


    U1STAbits.UTXINV = 0b0;
    U1STAbits.UTXBRK = 0b0;
    U1STAbits.URXISEL = 0b00; // interrupt after one RX character is received
    U1STAbits.ADDEN = 0;

    // we need the error interrupt to clear errors - without this it does not work
    IFS4bits.U1ERIF = 0; // clear Error Flag
    IEC4bits.U1ERIE = 1; // enable Error Interrupt

    IFS0bits.U1TXIF = 0; // clear TX Flag
    IEC0bits.U1TXIE = 0; // disable TX Interrupt

    IFS0bits.U1RXIF = 0; // clear RX interrupt flag
    IEC0bits.U1RXIE = 1; // enable RX interrupt

    U1MODEbits.UARTEN = 0b1; // turn it on
    U1STAbits.UTXEN = 0b1; // enable transmit

    return;
}

// this triggers when any data is received (through SPI1) 
// and stores that data in a buffer for the main loop

void __attribute__((__interrupt__,__no_auto_psv__)) _SPI1Interrupt(void) {

    unsigned char recvChar;

    _SPI1IF = 0; // clear interrupt flag
    _SPIROV = 0; //clear flag for overflow data
   
//    ledToggle(1);
    recvChar = SPI1BUF;

    if(recvChar != CHAR_NULL) {
        SPIRecvBuffer[SPIRecvBufferWritePos] = recvChar;
	SPIRecvBufferWritePos++;
	if( SPIRecvBufferWritePos >= BUFFER_LENGTH_RECV )
	{
		SPIRecvBufferWritePos = 0;
	}
    }
}

// this triggers when any data is received (through UART1) 
// and stores that data in a buffer for the main loop

void __attribute__((__interrupt__,__no_auto_psv__)) _U1RXInterrupt(void) {

    unsigned char recvChar = 1;

    _U1RXIF = 0; // clear interrupt flag
 
    recvChar = U1RXREG;

//        if(recvChar == 33){
//            ledTestSetOn(4);
//        }
//        if(recvChar == 42) {           
//            ledTestSetOff(4);
//        }
    if(recvChar != CHAR_NULL){
        UARTRecvBuffer[UARTRecvBufferWritePos] = recvChar;
	UARTRecvBufferWritePos++;
	if( UARTRecvBufferWritePos >= BUFFER_LENGTH_RECV )
	{
		UARTRecvBufferWritePos = 0;
	}
//        ledToggle(4);
    }
    
    
}



// this is required to clear any errors that occur - without it things do not work right

void __attribute__( (__interrupt__, __no_auto_psv__) ) _U2ErrInterrupt( void )
{
    U2STAbits.PERR;
    U2STAbits.FERR;
    U2STAbits.OERR = 0;

    IFS4bits.U2ERIF = 0; // clear error interrupt flag

    return;
}



// this is required to clear any errors that occur - without it things do not work right

void __attribute__( (__interrupt__, __no_auto_psv__) ) _U1ErrInterrupt( void )
{
    U1STAbits.PERR;
    U1STAbits.FERR;
    U1STAbits.OERR = 0;

    IFS4bits.U1ERIF = 0; // clear error interrupt flag

    return;
}
