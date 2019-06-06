/****************
 INCLUDES
 only include the header files that are required
 ****************/
#include <stdlib.h>

#include "common.h"
#include "I2C_RTCC.h"
#include "Delays.h"
#include "EEPROM.h"
#include "PowerMain.h"

/****************
 MACROS
 ****************/
#define BUFFER_LENGTH 40  // max size is positive signed character size
#define PORT_COUNT 3 // one based count of the number of ports

#define BUF_SIZE_CHAR 5
#define BUF_SIZE_INT 7
#define BUF_SIZE_LONG 12

//#define RUN_INTERVAL 8000 // run periodically, going too fast causes problems - this is based off a timer, not loops through the program

#define PARAMETER_MAX_COUNT 7
#define PARAMETER_MAX_LENGTH 10

//#define CHAR_NULL '\0'
#define COMMAND_SEND_RECEIVE_PRIMER_CHAR '#' // something to run the SPI clock so data can be received
#define COMMAND_START_CHAR '!'
#define COMMAND_END_CHAR '*'
#define COMMAND_DELIMETER ';'

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
long tba_energyAllocation;
long tba_energyUsedLifetime;
long tba_energyUsedLastDayReset;
long tba_powerWatts;
long tba_energyUsedPreviousDay = 0;

char audibleAlarm;
char alarm1Enabled;
char alarm2Enabled;
char alarm1Energy;
char alarm2Energy;

int emerAllocation = 0;

// internal only

enum receive_status
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

struct buffer
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

bool process_data( struct buffer *receive_buffer, struct buffer *send_buffer );
void process_data_parameterize( char parameters[][PARAMETER_MAX_LENGTH], struct buffer *buffer_to_parameterize );
bool process_data_parameters( char parameters[][PARAMETER_MAX_LENGTH], struct buffer *send_buffer );

void command_builder1( struct buffer *send_buffer, char* data1 );
void command_builder2( struct buffer *send_buffer, char* data1, char* data2 );
void command_builder3( struct buffer *send_buffer, char* data1, char* data2, char* data3 );
void command_builder4( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4 );
void command_builder5( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5 );
void command_builder6( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6 );
void command_builder7( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6, char* data7 );
void command_builder_add_char( struct buffer *send_buffer, char data );
void command_builder_add_string( struct buffer *send_buffer, char *data );

bool SPI_receive_data_char( char * );
bool SPI_send_data_char( char data );
bool UART1_receive_data_char( char * );
bool UART1_send_data_char( char data );
bool UART2_receive_data_char( char * );
bool UART2_send_data_char( char data );

bool strmatch( char* a, char* b );
int strcmp2( char* a, char* b );
void strcpy2( char* rcv, char* source );

void fillOnOff( char *buf, int checkValue );
bool checkOnOff( char *toCheck );

void zeroPad_itoa( char *output, int num, int minDigits );


void send_end_of_transmission( struct buffer *send_buffer );


bool communicationsSPI( bool initialize );
void communicationsUART1( bool initialize );
void communicationsUART2( bool initialize );
bool communicationsRecv( struct buffer *receive_buffer, struct buffer *send_buffer, enum communications_port_enum communicationsPort, enum receive_status *receive_current_state );
bool communicationsSend( struct buffer *send_buffer, enum communications_port_enum communicationsPort );

/****************
 CODE
 ****************/








void communications( bool initialize )
{
    communicationsSPI( initialize );

    communicationsUART1( initialize );
    communicationsUART2( initialize );

}

bool communicationsSPI( bool initialize )
{

    bool enabledSPI;

    static unsigned char current_port = PORT_COUNT; // port we are on - zero based - 0 to (PORT_COUNT - 1) we start with max so next port is 0
    static unsigned char current_port_done = true; // start with true and let normal program mechanism automatically init things

    static struct buffer send_buffer;
    static struct buffer receive_buffer;

    static bool end_of_transmission_received = false;
    bool no_more_to_send; // here to make this more readable

    static enum receive_status receive_current_state = enum_receive_status_waiting;
    static unsigned int receive_wait_count;
    static unsigned int receive_in_command_count;

    if( initialize == true )
    {
	send_buffer.port = enum_port_commSPI;
	//for now do nothing
	// variable init takes place when port changes, which initial values cause automatically
	// maybe init SPI ports here int he future
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

	if( process_data( &receive_buffer, &send_buffer ) == true )
	{
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
	    // make sure trans buffer is empty
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

    return enabledSPI;

    //    bool enabledSPI;
    //
    //    static unsigned char current_port = PORT_COUNT; // port we are on - zero based - 0 to (PORT_COUNT - 1) we start with max so next port is 0
    //    static unsigned char current_port_done = true; // start with true and let normal program mechanism automatically init things
    //
    //    static struct buffer send_buffer;
    //    static struct buffer receive_buffer;
    //
    //    static unsigned int receive_wait_count;
    //    static unsigned int receive_in_command_count;
    //
    //
    //    if( initialize == true )
    //    {
    //	// possibly init the SPI port here
    //	// do this later if we get to it
    //
    //	send_buffer.write_position = 0;
    //	send_buffer.read_position = 0;
    //	send_buffer.data_buffer[0] = CHAR_NULL;
    //
    //	receive_buffer.write_position = 0;
    //	receive_buffer.read_position = 0;
    //	receive_buffer.data_buffer[0] = CHAR_NULL;
    //
    //    }
    //
    //    if( current_port_done == true )
    //    {
    //	enabledSPI = set_current_port( &current_port );
    //
    //	if( enabledSPI == true )
    //	{
    //	    current_port_done = false;
    //	    //	    end_of_transmission_received = false;
    //
    //	    receive_wait_count = 0;
    //	    receive_in_command_count = 0;
    //
    //	    // put something in the send buffer to run the clock
    //	    send_buffer.write_position = 0;
    //	    send_buffer.read_position = 0;
    //	    receive_buffer.write_position = 0;
    //	    receive_buffer.read_position = 0;
    //
    //	    command_builder_add_char( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    //	}
    //    }
    //
    //    communicationsRecv( &receive_buffer, &send_buffer, enum_port_commSPI );
    //
    //
    //
    //    communicationsSend( &send_buffer, enum_port_commSPI );
    //
    //
    //    return enabledSPI;
}

void communicationsUART1( bool initialize )
{

    static struct buffer send_buffer;
    static struct buffer receive_buffer;
    static enum receive_status receive_current_state = enum_receive_status_waiting;

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

	if( process_data( &receive_buffer, &send_buffer ) == true )
	{
	    end_of_transmission_received = true;
	}
	break;
    }

    communicationsSend( &send_buffer, enum_port_commUART1 );

    return;
}

void communicationsUART2( bool initialize )
{
    static struct buffer send_buffer;
    static struct buffer receive_buffer;
    static enum receive_status receive_current_state = enum_receive_status_waiting;

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

	if( process_data( &receive_buffer, &send_buffer ) == true )
	{
	    end_of_transmission_received = true;
	}
	break;
    }

    communicationsSend( &send_buffer, enum_port_commUART2 );

    return;
}

bool communicationsRecv( struct buffer *receive_buffer, struct buffer *send_buffer, enum communications_port_enum communicationsPort, enum receive_status *receive_current_state )
{

    bool data_received;

    char data;

    if( *receive_current_state == enum_receive_status_end_command )
    {
	*receive_current_state = enum_receive_status_waiting;
    }

    data_received = false;

    bool gotSomething = false;


    //FIX
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

bool communicationsSend( struct buffer *send_buffer, enum communications_port_enum communicationsPort )
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
	    delayMSTenths( 5 );
	    send_buffer->read_position++;
	    if( send_buffer->read_position >= BUFFER_LENGTH )
	    {
		send_buffer->read_position = 0;
	    }
	}
    }

    return send_end;

}

//bool communicationsOld( enum communications_port_enum communicationsPort )
//{
//    bool enabledSPI;
//
//    static unsigned char current_port = PORT_COUNT; // port we are on - zero based - 0 to (PORT_COUNT - 1) we start with max so next port is 0
//    static unsigned char current_port_done = true; // start with true and let normal program mechanism automatically init things
//
//    static struct buffer send_buffer;
//    static struct buffer receive_buffer;
//
//    static bool end_of_transmission_received = false;
//    bool no_more_to_send; // here to make this more readable
//
//    static enum receive_status receive_current_state;
//    static unsigned int receive_wait_count;
//    static unsigned int receive_in_command_count;
//
//    if( current_port_done == true )
//    {
//	enabledSPI = set_current_port( &current_port );
//
//	if( enabledSPI == true )
//	{
//	    current_port_done = false;
//	    end_of_transmission_received = false;
//
//	    receive_wait_count = 0;
//	    receive_in_command_count = 0;
//
//	    // put something in the send buffer to run the clock
//	    send_buffer.write_position = 0;
//	    send_buffer.read_position = 0;
//	    receive_buffer.write_position = 0;
//	    receive_buffer.read_position = 0;
//
//	    command_builder_add_char( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
//
//	}
//    }
//
//
//    bool data_received;
//
//    receive_current_state = receive_data( &receive_buffer, &data_received, communicationsPort );
//    switch( receive_current_state )
//    {
//    case enum_receive_status_waiting:
//	// count # of times we are waiting for COMMAND_START_CHAR !
//	if( data_received == true )
//	{
//	    receive_wait_count++;
//	}
//
//	break;
//    case enum_receive_status_in_command:
//	// count # of times we are in a command
//	// need to check if we somehow missed the COMMAND_END_CHAR *
//	// must be more than max length a command can be
//	if( data_received == true )
//	{
//	    receive_wait_count = 0;
//	    receive_in_command_count++;
//	}
//
//	break;
//    case enum_receive_status_end_command:
//
//	if( process_data( &receive_buffer, &send_buffer ) == true )
//	{
//	    end_of_transmission_received = true;
//	}
//	receive_wait_count = 0;
//	receive_in_command_count = 0;
//
//	break;
//    }
//
//
//    no_more_to_send = send_data( &send_buffer );
//
//    if( no_more_to_send == true )
//    {
//	if( end_of_transmission_received == true )
//	{
//	    // make sure trans buffer is empty
//	    // the following test is for standard buffer mode only
//	    // a different check must be performed if the enhanced buffer is used
//	    if( SPI1STATbits.SPITBF == 0b0 ) // only for standard buffer
//	    {
//		current_port_done = true;
//	    }
//	}
//	else if( receive_wait_count >= RECEIVE_WAIT_COUNT_LIMIT )
//	{
//	    // not receiving anything valid from slave
//	    // just move to the next port - things should clear up on their own eventually
//
//	    current_port_done = true;
//	}
//	else if( receive_in_command_count >= RECEIVE_IN_COMMAND_COUNT_LIMIT )
//	{
//	    // received too many characters before the command was ended
//	    // likely a garbled COMMAND_END_CHAR or something
//	    // just move to the next port - things should clear up on their own eventually
//	    current_port_done = true;
//	}
//	else
//	{
//	    command_builder_add_char( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
//	}
//    }
//
//    return enabledSPI;
//}

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
	//	LED4___SET = 0;
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

//enum receive_status receive_data( struct buffer *receive_buffer, bool *data_received, enum communications_port_enum communicationsPort )
//{
//    char data;
//
//    static enum receive_status my_status = enum_receive_status_waiting;
//
//    if( my_status == enum_receive_status_end_command )
//    {
//	my_status = enum_receive_status_waiting;
//    }
//
//    *data_received = false;
//
//    bool gotSomething = false;
//
//    switch( communicationsPort )
//    {
//    case enum_port_commSPI:
//	gotSomething = SPI_receive_data_char( &data );
//	break;
//    case enum_port_commUART1:
//	//	gotSomething = UART1_receive_data( &data );
//	break;
//    case enum_port_commUART2:
//	//	gotSomething = UART2_receive_data( &data );
//	break;
//    }
//
//    //    if( SPI_receive_data( &data ) == true )
//    if( gotSomething == true )
//    {
//	*data_received = true;
//
//	if( (data == COMMAND_START_CHAR) && (my_status != enum_receive_status_in_command) )
//	{
//
//	    my_status = enum_receive_status_in_command;
//	    receive_buffer->read_position = 0;
//	    receive_buffer->write_position = 0;
//	}
//
//	if( my_status == enum_receive_status_in_command )
//	{
//	    receive_buffer->data_buffer[ receive_buffer->write_position] = data;
//
//	    receive_buffer->write_position++;
//	    if( receive_buffer->write_position >= BUFFER_LENGTH )
//	    {
//		receive_buffer->write_position = (BUFFER_LENGTH - 1);
//	    }
//	}
//
//	if( (my_status == enum_receive_status_in_command) && (data == COMMAND_END_CHAR) )
//	{
//	    my_status = enum_receive_status_end_command;
//	}
//    }
//
//    return my_status;
//}

bool process_data( struct buffer *receive_buffer, struct buffer *send_buffer )
{
    bool end_of_transmission_received;

    // if we are here then the receive buffer must have good data with start and end command characters
    // the characters are not included as they were not added

    char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH];

    process_data_parameterize( parameters, receive_buffer );

    end_of_transmission_received = process_data_parameters( parameters, send_buffer );

    return end_of_transmission_received;
}

void process_data_parameterize( char parameters[][PARAMETER_MAX_LENGTH], struct buffer *buffer_to_parameterize )
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

bool process_data_parameters( char parameters[][PARAMETER_MAX_LENGTH], struct buffer *send_buffer )
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


	    char newTimeDay;
	    char newTimeMonth;
	    char newTimeYear;

	    char newTimeHour;
	    char newTimeMinute;
	    char newTimeSecond;

	    newTimeDay = atoi( timeDayBuf );
	    newTimeMonth = atoi( timeMonthBuf );
	    newTimeYear = atoi( timeYearBuf );

	    newTimeHour = atoi( timeHourBuf );
	    newTimeMinute = atoi( timeMinuteBuf );
	    newTimeSecond = atoi( timeSecondBuf );

	    writeTime( newTimeYear, newTimeMonth, newTimeDay, newTimeHour, newTimeMinute, newTimeSecond );
	    setI2CTime( newTimeYear, newTimeMonth, newTimeDay, newTimeHour, newTimeMinute, newTimeSecond );

	    readTimeI2C( );

	    command_builder2( send_buffer, "Conf", "Time" );

	}
	else if( strmatch( parameters[1], "EnAl" ) == true )
	{
	    tba_energyAllocation = atol( parameters[2] );

	    EEwriteEnergyAlloc( );
	    // previously set high/low here as well
	    // may get rid of high/low for now

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
		audibleAlarm = 1;
	    }
	    else
	    {
		audibleAlarm = 0;
	    }


	    if( checkOnOff( parameters[3] ) == true )
	    {
		alarm1Enabled = 1;
	    }
	    else
	    {
		alarm1Enabled = 0;
	    }

	    alarm1Energy = atoi( parameters[4] );


	    if( checkOnOff( parameters[5] ) == true )
	    {
		alarm2Enabled = 1;
	    }
	    else
	    {
		alarm2Enabled = 0;
	    }

	    alarm2Energy = atoi( parameters[6] );

	    EEwriteAlarm( );

	    command_builder2( send_buffer, "Conf", "Alarm" );
	}
	else if( strmatch( parameters[1], "Pass" ) == true )
	{
	    passwordSet[0] = parameters[2][0];
	    passwordSet[1] = parameters[2][1];
	    passwordSet[2] = parameters[2][2];
	    passwordSet[3] = parameters[2][3];
	    passwordSet[4] = parameters[2][4];
	    passwordSet[5] = parameters[2][5];

	    EEwritePassword( );

	    command_builder2( send_buffer, "Conf", "Pass" );

	}
	else if( strmatch( parameters[1], "Emer" ) == true )
	{

	    if( checkOnOff( parameters[2] ) == true )
	    {
		emerButtonEnable = true;
	    }
	    else
	    {
		emerButtonEnable = false;
	    }

	    emerButtonEnergyAllocate = atoi( parameters[3] );



	    command_builder2( send_buffer, "Conf", "Emer" );

	}
	else if( strmatch( parameters[1], "RstTim" ) == true )
	{


	    resetTimeHour = atoi( parameters[2] );
	    resetTimeMinute = atoi( parameters[3] );

	    char rsh[20];
	    char rsm[20];

	    itoa( rsh, resetTimeHour, 10 );
	    itoa( rsm, resetTimeMinute, 10 );
	    EEwriteResetTime( );

	    command_builder4( send_buffer, "Conf", "RstTim", rsh, rsm );

	}
	else if( strmatch( parameters[1], "Relay" ) == true )
	{

	    if( checkOnOff( parameters[2] ) == true )
	    {
		relayActive = 1;
	    }
	    else
	    {
		relayActive = 0;
	    }

	    EEwriteRelay( );

	    command_builder2( send_buffer, "Conf", "Relay" );

	}
	    //	else if( strmatch( parameters[1], "DebugMode" ) )

	else if( strmatch( parameters[1], "Watts" ) == true )
	{
	    tba_powerWatts = atol( parameters[2] );
	    command_builder2( send_buffer, "Conf", "Watts" );
	}
	else if( strmatch( parameters[1], "EnUsed" ) == true )
	{
	    // the lifetime energy is currently stored in the command board EEPROM
	    // power sense at power-up has lifetime energy at 0
	    // if power sense lifetime energy is < command board lifetime energy we must be in start-up
	    // send power sense new lifetime energy value

	    unsigned long tempEnergyUsedLifetime;



	    tempEnergyUsedLifetime = atol( parameters[2] );

	    // TESTING   remove the lifetime setting
	    if( tempEnergyUsedLifetime < tba_energyUsedLifetime )
	    {
		if( tba_energyUsedLifetime == 0 )
		{
		}

		char temp[12];
		//		ultoa( temp, totalUsed, 10 );
		ltoa( temp, tba_energyUsedLifetime, 10 );
		command_builder3( send_buffer, "Set", "EnUsed", temp );

	    }
	    else
	    {
		tba_energyUsedLifetime = tempEnergyUsedLifetime;
		// done know if we need this here		powerUsed = totalUsed - tba_powerUsedDayStart;
		command_builder2( send_buffer, "Conf", "EnUsed" );
	    }

	}
	else if( strmatch( parameters[1], "Lights" ) == true )
	{

	    lightsModeActive = checkOnOff( parameters[2] );

	    command_builder2( send_buffer, "Conf", "Lights" );

	}
	else if( strmatch( parameters[1], "AllAdd" ) == true )
	{
	    emerAllocation = atoi( parameters[2] );

	    char buf[BUF_SIZE_INT];
	    itoa( buf, emerAllocation, 10 );

	    command_builder3( send_buffer, "Conf", "AllAdd", buf );

	}
	    //	else if( strmatch( parameters[1], "Volts" ) == true )
	    //	{
	    //	    powerVolts = atoi( parameters[2] );
	    //	    command_builder2( send_buffer, "Conf", "Volts" );
	    //	}
	    //	else if( strmatch( parameters[1], "Amps" ) == true )
	    //	{
	    //	    powerAmps = atoi( parameters[2] );
	    //	    command_builder2( send_buffer, "Conf", "Amps" );
	    //	}
	else if( strmatch( parameters[1], "PSVersion" ) == true )
	{
	    command_builder2( send_buffer, "Conf", "PSVersion" );
	}


	//	else if( strmatch( parameters[1], "LED" ) == true )
	//	{
	//	    if( strmatch( parameters[2], "On" ) == true )
	//	    {
	//		command_builder3( send_buffer, "Conf", "LED", "On" );
	//
	//	    }
	//	    else if( strmatch( parameters[2], "Off" ) == true )
	//	    {
	//		command_builder3( send_buffer, "Conf", "LED", "Off" );
	//	    }
	//	}
	//	else if( strmatch( parameters[1], "LEDB" ) == true )
	//	{
	//	    if( strmatch( parameters[2], "On" ) == true )
	//	    {
	////		LED1_SET = 1;
	//		command_builder3( send_buffer, "Conf", "LEDB", "On" );
	//
	//	    }
	//	    else if( strmatch( parameters[2], "Off" ) == true )
	//	    {
	////		LED1_SET = 0;
	//		command_builder3( send_buffer, "Conf", "LEDB", "Off" );
	//	    }
	//	}


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


	    zeroPad_itoa( timeDateDDBuf, timeDay, 2 );
	    zeroPad_itoa( timeDateMMBuf, timeMonth, 2 );
	    zeroPad_itoa( timeDateYYBuf, timeYear, 2 );
	    zeroPad_itoa( timeTimeHHBuf, timeHour, 2 );
	    zeroPad_itoa( timeTimeMMBuf, timeMinute, 2 );
	    zeroPad_itoa( timeTimeSSBuf, timeSecond, 2 );

	    timeDateBuf[0] = timeDateDDBuf[0];
	    timeDateBuf[1] = timeDateDDBuf[1];
	    timeDateBuf[2] = '-';
	    timeDateBuf[3] = timeDateMMBuf[0];
	    timeDateBuf[4] = timeDateMMBuf[1];
	    timeDateBuf[5] = '-';
	    timeDateBuf[6] = timeDateYYBuf[0];
	    timeDateBuf[7] = timeDateYYBuf[1];
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

	    ltoa( energyAllocationBuf, tba_energyAllocation, 10 );

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

	    fillOnOff( audibleAlarmBuf, audibleAlarm );
	    fillOnOff( alarm1EnabledBuf, alarm1Enabled );
	    fillOnOff( alarm2EnabledBuf, alarm2Enabled );

	    // using itoa() - variable type is char, make sure it is an int
	    alarm1EnergyTemp = alarm1Energy;
	    alarm2EnergyTemp = alarm2Energy;
	    itoa( alarm1EnergyBuf, alarm1EnergyTemp, 10 );
	    itoa( alarm2EnergyBuf, alarm2EnergyTemp, 10 );

	    command_builder7( send_buffer, "Set", "Alarm", audibleAlarmBuf, alarm1EnabledBuf, alarm1EnergyBuf, alarm2EnabledBuf, alarm2EnabledBuf );

	}
	else if( strmatch( parameters[1], "Pass" ) == true )
	{
	    char passwordTemp[7];

	    passwordTemp[0] = passwordSet[0];
	    passwordTemp[1] = passwordSet[1];
	    passwordTemp[2] = passwordSet[2];
	    passwordTemp[3] = passwordSet[3];
	    passwordTemp[4] = passwordSet[4];
	    passwordTemp[5] = passwordSet[5];
	    passwordTemp[6] = CHAR_NULL;

	    command_builder3( send_buffer, "Set", "Pass", passwordTemp );

	}
	else if( strmatch( parameters[1], "Emer" ) == true )
	{


	    char emerButtonEnableBuf[4];
	    char emerButtonEnergyAllocateBuf[BUF_SIZE_INT];

	    fillOnOff( emerButtonEnableBuf, emerButtonEnable );
	    itoa( emerButtonEnergyAllocateBuf, emerButtonEnergyAllocate, 10 );

	    command_builder4( send_buffer, "Set", "Emer", emerButtonEnableBuf, emerButtonEnergyAllocateBuf );

	}
	else if( strmatch( parameters[1], "RstTim" ) == true )
	{

	    char resetTimeHourBuf[BUF_SIZE_INT];
	    char resetTimeMinuteBuf[BUF_SIZE_INT];


	    itoa( resetTimeHourBuf, resetTimeHour, 10 );
	    itoa( resetTimeMinuteBuf, resetTimeMinute, 10 );

	    command_builder4( send_buffer, "Set", "RstTim", resetTimeHourBuf, resetTimeMinuteBuf );

	}
	else if( strmatch( parameters[1], "Relay" ) == true )
	{
	    char relayActiveBuf[4];

	    fillOnOff( relayActiveBuf, relayActive );

	    command_builder3( send_buffer, "Set", "Relay", relayActiveBuf );

	}
	else if( strmatch( parameters[1], "Stat" ) == true )
	{
	    char charEnergyUsedLifetime[BUF_SIZE_LONG];
	    char charEnergyUsedPreviousDay[BUF_SIZE_LONG];

	    ltoa( charEnergyUsedLifetime, tba_energyUsedLifetime, 10 );
	    ltoa( charEnergyUsedPreviousDay, tba_energyUsedPreviousDay, 10 );

	    command_builder4( send_buffer, "Set", "Stat", charEnergyUsedLifetime, charEnergyUsedPreviousDay );

	}
	else if( strmatch( parameters[1], "CBver" ) == true )
	{

	    command_builder3( send_buffer, "Set", "CBver", powerBoxCodeVersion );

	}
	else if( strmatch( parameters[1], "PwrFail" ) == true )
	{

	    char powerFailTimeBuf[12];
	    char powerRestoreTimeBuf[12];


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


	    powerFailTimeBuf[ 0] = (powerFailTimeHour >> 4) + 0x30;
	    powerFailTimeBuf[ 1] = (powerFailTimeHour) + 0x30;
	    powerFailTimeBuf[ 3] = (powerFailTimeMinute >> 4) + 0x30;
	    powerFailTimeBuf[ 4] = (powerFailTimeMinute) + 0x30;
	    powerFailTimeBuf[ 6] = (powerFailTimeDay >> 4) + 0x30;
	    powerFailTimeBuf[ 7] = (powerFailTimeDay) + 0x30;
	    powerFailTimeBuf[ 9] = (powerFailTimeMonth >> 4) + 0x30;
	    powerFailTimeBuf[10] = (powerFailTimeMonth) + 0x30;
	    powerFailTimeBuf[11] = CHAR_NULL;

	    powerRestoreTimeBuf [ 0] = (powerRestoreTimeHour >> 4) + 0x30;
	    powerRestoreTimeBuf [ 1] = (powerRestoreTimeHour) + 0x30;
	    powerRestoreTimeBuf [ 3] = (powerRestoreTimeMinute >> 4) + 0x30;
	    powerRestoreTimeBuf [ 4] = (powerRestoreTimeMinute) + 0x30;
	    powerRestoreTimeBuf [ 6] = (powerRestoreTimeDay >> 4) + 0x30;
	    powerRestoreTimeBuf [ 7] = (powerRestoreTimeDay) + 0x30;
	    powerRestoreTimeBuf [ 9] = (powerRestoreTimeMonth >> 4) + 0x30;
	    powerRestoreTimeBuf [10] = (powerRestoreTimeMonth) + 0x30;
	    powerRestoreTimeBuf [11] = CHAR_NULL;


	    command_builder4( send_buffer, "Set", "PwrFail", powerFailTimeBuf, powerRestoreTimeBuf );

	}
	else if( strmatch( parameters[1], "PwrData" ) == true )
	{
	    long energyUsedTemp;

	    energyUsedTemp = tba_energyUsedLifetime - tba_energyUsedLastDayReset;

	    char energyAllocationBuf[BUF_SIZE_LONG];
	    char energyUsedBuf[BUF_SIZE_LONG];
	    char powerWattsBuf[BUF_SIZE_LONG];

	    ltoa( energyAllocationBuf, (tba_energyAllocation + emerAllocation), 10 );
	    ltoa( energyUsedBuf, energyUsedTemp, 10 );
	    ltoa( powerWattsBuf, tba_powerWatts, 10 );

	    command_builder5( send_buffer, "Set", "PwrData", energyAllocationBuf, energyUsedBuf, powerWattsBuf );

	}
    }




    //	if( strmatch( parameters[1], "LEDB" ) == true )
    //	{
    //
    //	    if( LED1READ == 0b1 )
    //	    {
    //		command_builder3( send_buffer, "Data", "LEDB", "On" );
    //	    }
    //	    else
    //	    {
    //		command_builder3( send_buffer, "Data", "LEDB", "Off" );
    //	    }
    //	}
    //	}

    // add new parameters as necessary
    // NEVER check for a higher parameter number than we allocated for.
    // see earlier comments about NULLS and dying from old age

    return end_of_transmission_received;
}

void command_builder1( struct buffer *send_buffer, char* data1 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder2( struct buffer *send_buffer, char* data1, char* data2 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder3( struct buffer *send_buffer, char* data1, char* data2, char* data3 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data3 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder4( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data3 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data4 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder5( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data3 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data4 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data5 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder6( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data3 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data4 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data5 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data6 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder7( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4, char* data5, char* data6, char* data7 )
{
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data3 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data4 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data5 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data6 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data7 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );

    return;
}

void command_builder_add_char( struct buffer *send_buffer, char data )
{
    send_buffer->data_buffer[send_buffer->write_position] = data;

    send_buffer->write_position++;
    if( send_buffer->write_position >= BUFFER_LENGTH )
    {
	send_buffer->write_position = 0;
    }

    return;
}

void command_builder_add_string( struct buffer *send_buffer, char *data_string )
{
    for( int inx = 0; data_string[inx] != CHAR_NULL; inx++ )
    {
	command_builder_add_char( send_buffer, data_string[inx] );
    }

    return;
}

//bool send_data( struct buffer *send_buffer )
//{
//    bool send_end;
//
//    if( send_buffer->read_position == send_buffer->write_position )
//    {
//	send_end = true;
//    }
//    else
//    {
//	send_end = false;
//	if( SPI_send_data_char( send_buffer->data_buffer[send_buffer->read_position] ) == true )
//	{
//	    send_buffer->read_position++;
//	    if( send_buffer->read_position >= BUFFER_LENGTH )
//	    {
//		send_buffer->read_position = 0;
//	    }
//	}
//    }
//
//    return send_end;
//}

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

    if( SPI1STATbits.SPIRBF == 1 )
    {
	*data = SPI1BUF;
	recvGood = true;
    }

    return recvGood;
}

bool SPI_send_data_char( char data )
{
    bool sendGood = false;

    if( SPI1STATbits.SPITBF == 0 ) //if in enhance mode use SPI1STATbits.SR1MPT
    {
	SPI1BUF = data;
	sendGood = true;
    }

    return sendGood;
}

bool UART1_receive_data_char( char *data )
{
    bool recvGood = false;

    if( U1STAbits.URXDA == 1 )
    {
	*data = U1RXREG;
	recvGood = true;
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

void send_end_of_transmission( struct buffer * send_buffer )
{
    command_builder1( send_buffer, "END" );

    return;
}

void SPIMasterInit( void )
{
    static bool firstRun = true;

    // make sure analog is turned off - it messes with the pins
    ANSA = 0;
    ANSB = 0;

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
    SPI1CON1bits.SPRE = 0b110; // secondary prescale 2:1
    SPI1CON1bits.PPRE = 0b11; // primary prescale 1:1
    //    SPI1CON1bits.PPRE = 0b00; // primary prescale 1:1

    SPI1CON2bits.FRMEN = 0b0; // frame mode, unused
    SPI1CON2bits.SPIFSD = 0b0; // frame mode, unused
    SPI1CON2bits.SPIFPOL = 0b0; // frame mode, unused
    SPI1CON2bits.SPIFE = 0b0; // frame mode, unused

    SPI1CON2bits.SPIBEN = 0b0; // 1=enhanced buffer mode

    SPI1STATbits.SPIROV = 0; //clear flag for overflow data


    // do not use the interrupt , could not get it to work
    //    SPI1STATbits.SISEL = 0b001;
    //
    //    IFS0bits.SPI1IF = 0;
    //    IEC0bits.SPI1IE = 1;




    //SPI1BUF = SPI1BUF;
    //    SPI1STATbits.SPIEN = 1; //enable SPI

    if( firstRun == true )
    {
	// set timer up here
	T1CONbits.TSIDL = 0b1; //Discontinue module operation when device enters idle mode
	T1CONbits.T1ECS = 0b00; // Timer1 uses Secondary Oscillator (SOSC) as the clock soource)
	T1CONbits.TGATE = 0b0; // Gated time accumulation is disabled
	T1CONbits.TSYNC = 0b0; // Do not synchronize external clock input (asynchronous)
	T1CONbits.TCS = 0b0; //use internal clock


	T1CONbits.TCKPS = 0b00; // Timer 1 Input Clock Prescale (11-256)(10-64) (01-8) (00-1)
	TMR1 = 0x0000; // start timer at 0

	T1CONbits.TON = 0b1; //turn on timer
	firstRun = false;
    }
    return;
}

//void __attribute__((__interrupt__,__auto_psv__)) _SPI1Interrupt(void)
//{
//    // we received a byte
//    IFS0bits.SPI1IF = 0;
//
//    rcvSPI = true;
//    rcvChar =  SPI1BUF;
//
//    return;
//
//}


//void communicationsUART( )
//{
//
//    // must do UART1 and UART2 separately
//
//    // recv
//    // send
//
//    // global volatile UART1 recv ready
//
//
//
//}



//bool communicationsSPI( )
//{
//    bool enabledSPI;
//
//    static unsigned char current_port = PORT_COUNT; // port we are on - zero based - 0 to (PORT_COUNT - 1) we start with max so next port is 0
//    static unsigned char current_port_done = true; // start with true and let normal program mechanism automatically init things
//
//    static struct buffer send_buffer;
//    static struct buffer receive_buffer;
//
//    static bool end_of_transmission_received = false;
//    bool no_more_to_send; // here to make this more readable
//
//    static enum receive_status receive_current_state;
//    static unsigned int receive_wait_count;
//    static unsigned int receive_in_command_count;
//
//    if( current_port_done == true )
//    {
//	enabledSPI = set_current_port( &current_port );
//
//	if( enabledSPI == true )
//	{
//	    current_port_done = false;
//	    end_of_transmission_received = false;
//
//	    receive_wait_count = 0;
//	    receive_in_command_count = 0;
//
//	    // put something in the send buffer to run the clock
//	    send_buffer.write_position = 0;
//	    send_buffer.read_position = 0;
//	    receive_buffer.write_position = 0;
//	    receive_buffer.read_position = 0;
//
//	    command_builder_add_char( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
//
//	}
//    }
//
//    bool data_received;
//
//    receive_current_state = receive_data( &receive_buffer, &data_received );
//    switch( receive_current_state )
//    {
//    case receive_waiting:
//	// count # of times we are waiting for COMMAND_START_CHAR !
//	if( data_received == true )
//	{
//	    receive_wait_count++;
//	}
//
//	break;
//    case receive_in_command:
//	// count # of times we are in a command
//	// need to check if we somehow missed the COMMAND_END_CHAR *
//	// must be more than max length a command can be
//	if( data_received == true )
//	{
//	    receive_wait_count = 0;
//	    receive_in_command_count++;
//	}
//
//	break;
//    case receive_end_command:
//
//	if( process_data( &receive_buffer, &send_buffer ) == true )
//	{
//	    end_of_transmission_received = true;
//	}
//	receive_wait_count = 0;
//	receive_in_command_count = 0;
//
//	break;
//    }
//
//    no_more_to_send = send_data( &send_buffer );
//
//    if( no_more_to_send == true )
//    {
//	if( end_of_transmission_received == true )
//	{
//	    // make sure trans buffer is empty
//	    // the following test is for standard buffer mode only
//	    // a different check must be performed if the enhanced buffer is used
//	    if( SPI1STATbits.SPITBF == 0b0 ) // only for standard buffer
//	    {
//		current_port_done = true;
//	    }
//	}
//	else if( receive_wait_count >= RECEIVE_WAIT_COUNT_LIMIT )
//	{
//	    // not receiving anything valid from slave
//	    // just move to the next port - things should clear up on their own eventually
//
//	    current_port_done = true;
//	}
//	else if( receive_in_command_count >= RECEIVE_IN_COMMAND_COUNT_LIMIT )
//	{
//	    // received too many characters before the command was ended
//	    // likely a garbled COMMAND_END_CHAR or something
//	    // just move to the next port - things should clear up on their own eventually
//	    current_port_done = true;
//	}
//	else
//	{
//	    command_builder_add_char( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
//	}
//    }
//
//    return enabledSPI;
//}
//
//bool set_current_port( unsigned char *current_port )
//{
//    static bool enabledSPI = true;
//
//    if( enabledSPI == true )
//    {
//	SPI1STATbits.SPIEN = 0; //disable master SPI
//	enabledSPI = false;
//
//	SPI_PORT_0 = 1; //disable slave select (1 is disabled)
//	SPI_PORT_1 = 1; //disable slave select (1 is disabled)
//	SPI_PORT_2 = 1; //disable slave select (1 is disabled)
//	//	LED4_SET = 0;
//
//    }
//    else
//    {
//	(*current_port)++;
//	if( *current_port >= PORT_COUNT )
//	{
//	    *current_port = 0;
//	}
//
//	switch( *current_port )
//	{
//	case 0:
//	    // set correct DO chip select here
//	    SPI_PORT_0 = 0; //enable Slave Select
//	    break;
//	case 1:
//	    // set correct DO the chip select here
//	    SPI_PORT_1 = 0;
//	    //	    LED4_SET = 1;
//
//	    break;
//	case 2:
//	    // set correct DO the chip select here
//	    SPI_PORT_2 = 0;
//	    break;
//	}
//
//	SPI1STATbits.SPIEN = 1; //enable master SPI
//	enabledSPI = true;
//    }
//
//    return enabledSPI;
//}
//
//enum receive_status receive_data( struct buffer *receive_buffer, bool *data_received )
//{
//    char data;
//
//    static enum receive_status my_status = receive_waiting;
//
//    if( my_status == receive_end_command )
//    {
//	my_status = receive_waiting;
//    }
//
//    *data_received = false;
//
//    if( SPI_receive_data( &data ) == true )
//    {
//	*data_received = true;
//
//
//	if( (data == COMMAND_START_CHAR) && (my_status != receive_in_command) )
//	{
//
//	    my_status = receive_in_command;
//	    receive_buffer->read_position = 0;
//	    receive_buffer->write_position = 0;
//	}
//
//	if( my_status == receive_in_command )
//	{
//	    receive_buffer->data_buffer[ receive_buffer->write_position] = data;
//
//	    receive_buffer->write_position++;
//	    if( receive_buffer->write_position >= BUFFER_LENGTH )
//	    {
//		receive_buffer->write_position = (BUFFER_LENGTH - 1);
//	    }
//	}
//
//	if( (my_status == receive_in_command) && (data == COMMAND_END_CHAR) )
//	{
//	    my_status = receive_end_command;
//	}
//    }
//
//    return my_status;
//}
//
//bool process_data( struct buffer *receive_buffer, struct buffer *send_buffer )
//{
//    bool end_of_transmission_received;
//
//    // if we are here then the receive buffer must have good data with start and end command characters
//    // the characters are not included as they were not added
//
//    char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH];
//
//    process_data_parameterize( parameters, receive_buffer );
//
//    end_of_transmission_received = process_data_parameters( parameters, send_buffer );
//
//    return end_of_transmission_received;
//}
//
//void process_data_parameterize( char parameters[][PARAMETER_MAX_LENGTH], struct buffer *buffer_to_parameterize )
//{
//    unsigned char parameter_position = 0;
//    unsigned char parameter_index = 0;
//
//    // only one command is expected due to the way we read
//    // go through buffer until we hit end char or end of buffer
//
//    // this is super important - we must initialize the entire array
//    // if we do not we risk passing junk into some functions that assume strings and check for NULL
//    // without NULL a string function could run forever until we die from old age
//    // even then it would keep running
//    for( int inx = 0; inx < PARAMETER_MAX_COUNT; inx++ )
//    {
//	parameters[inx][0] = CHAR_NULL;
//    }
//
//    while( (buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position ] != COMMAND_END_CHAR) && (buffer_to_parameterize->read_position < BUFFER_LENGTH) && (buffer_to_parameterize->read_position != buffer_to_parameterize->write_position) )
//    {
//	switch( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position] )
//	{
//	case COMMAND_START_CHAR:
//	    // this character should never appear
//	    break;
//	case COMMAND_DELIMETER:
//	    // move to next parameter
//	    parameter_position = 0;
//	    parameter_index++;
//
//	    if( parameter_index >= PARAMETER_MAX_COUNT )
//	    {
//		// if we run out of parameters just overwrite the last one
//		// we should never have this case, but this keeps us from crashing and burning
//		parameter_index = (PARAMETER_MAX_COUNT - 1);
//	    }
//
//	    break;
//	default:
//	    // add the character to the current parameter
//	    parameters[parameter_index][parameter_position] = buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position];
//	    parameter_position++;
//	    if( parameter_position >= PARAMETER_MAX_LENGTH )
//	    {
//		// if our parameter is too long, just overwrite the last character
//		// we should never have this case, but this keeps us from crashing and burning
//		parameter_position = (PARAMETER_MAX_LENGTH - 1);
//	    }
//	    parameters[parameter_index][parameter_position] = CHAR_NULL;
//	    break;
//	}
//	buffer_to_parameterize->read_position++;
//    }
//
//    buffer_to_parameterize->read_position = 0;
//    buffer_to_parameterize->write_position = 0;
//
//    return;
//}
//
//bool process_data_parameters( char parameters[][PARAMETER_MAX_LENGTH], struct buffer *send_buffer )
//{
//    bool end_of_transmission_received = false;
//
//    // the 'commands' shown here are for example only
//    // make them whatever is needed
//
//    // ideally, any new commands are set in a separate function called from one of these tests
//    // it's not very clean to call the command builder functions from here
//    // especially if there is some processing to do, like setting a clock or something
//
//    if( strmatch( parameters[0], "END" ) == true )
//    {
//
//	send_end_of_transmission( send_buffer );
//	end_of_transmission_received = true;
//    }
//    else if( strmatch( parameters[0], "Set" ) == true )
//    {
//	if( strmatch( parameters[1], "Watts" ) == true )
//	{
//	    tba_powerWatts = strtoul( parameters[2], NULL, 10 );
//	    command_builder2( send_buffer, "Conf", "Watts" );
//	}
//	else if( strmatch( parameters[1], "EnUsed" ) == true )
//	{
//	    // the lifetime energy is currently stored in the command board EEPROM
//	    // power sense at power-up has lifetime energy at 0
//	    // if power sense lifetime energy is < command board lifetime energy we must be in start-up
//	    // send power sense new lifetime energy value
//
//	    unsigned long tempEnergyUsedLifetime;
//
//
//	    // TESTING
//	    //  Sometimes things don't go well without an end pointer
//	    //        https://www.microchip.com/forums/m633615.aspx
//	    //  Leading '0' in number string can cause trouble
//	    //  text explicitly for leading 0
//	    //  we always use decimal and we never lead a number with 0
//	    //  so, if first character is '0', the our result = 0
//
//	    char *endptr;
//	    if( parameters[2][0] == '0' )
//	    {
//		tempEnergyUsedLifetime = 0;
//	    }
//	    else
//	    {
//		tempEnergyUsedLifetime = strtoul( parameters[2], &endptr, 10 );
//	    }
//
//	    // TESTING   remove the lifetime setting
//	    if( tempEnergyUsedLifetime < tba_energyUsedLifetime )
//	    {
//		//    if( tba_energyUsedLifetime == 0 )
//		{
//		    //		    LED1_DIR = 0;
//		    //		    LED4_DIR = 0;
//		    for( int inx = 0; inx < 10; inx++ )
//		    {
//			LED2_SET = 0;
//			LED4_SET = 1;
//			delayMS( 100 );
//			LED2_SET = 1;
//			LED4_SET = 0;
//			delayMS( 100 );
//		    }
//		}
//
//		char temp[12];
//		//		ultoa( temp, totalUsed, 10 );
//		ultoa( temp, tba_energyUsedLifetime, 10 );
//		command_builder3( send_buffer, "Set", "EnUsed", temp );
//
//	    }
//	    else
//	    {
//		tba_energyUsedLifetime = tempEnergyUsedLifetime;
//		// done know if we need this here		powerUsed = totalUsed - tba_powerUsedDayStart;
//		command_builder2( send_buffer, "Conf", "EnUsed" );
//	    }
//
//	}
//	    //	else if( strmatch( parameters[1], "Volts" ) == true )
//	    //	{
//	    //	    powerVolts = atoi( parameters[2] );
//	    //	    command_builder2( send_buffer, "Conf", "Volts" );
//	    //	}
//	    //	else if( strmatch( parameters[1], "Amps" ) == true )
//	    //	{
//	    //	    powerAmps = atoi( parameters[2] );
//	    //	    command_builder2( send_buffer, "Conf", "Amps" );
//	    //	}
//	else if( strmatch( parameters[1], "PSVersion" ) == true )
//	{
//	    command_builder2( send_buffer, "Conf", "PSVersion" );
//	}
//
//	//	else if( strmatch( parameters[1], "LED" ) == true )
//	//	{
//	//	    if( strmatch( parameters[2], "On" ) == true )
//	//	    {
//	//		command_builder3( send_buffer, "Conf", "LED", "On" );
//	//
//	//	    }
//	//	    else if( strmatch( parameters[2], "Off" ) == true )
//	//	    {
//	//		command_builder3( send_buffer, "Conf", "LED", "Off" );
//	//	    }
//	//	}
//	//	else if( strmatch( parameters[1], "LEDB" ) == true )
//	//	{
//	//	    if( strmatch( parameters[2], "On" ) == true )
//	//	    {
//	////		LED1_SET = 1;
//	//		command_builder3( send_buffer, "Conf", "LEDB", "On" );
//	//
//	//	    }
//	//	    else if( strmatch( parameters[2], "Off" ) == true )
//	//	    {
//	////		LED1_SET = 0;
//	//		command_builder3( send_buffer, "Conf", "LEDB", "Off" );
//	//	    }
//	//	}
//
//
//    }
//    else if( strmatch( parameters[0], "Read" ) == true )
//    {
//	//	if( strmatch( parameters[1], "LEDB" ) == true )
//	//	{
//	//
//	//	    if( LED1READ == 0b1 )
//	//	    {
//	//		command_builder3( send_buffer, "Data", "LEDB", "On" );
//	//	    }
//	//	    else
//	//	    {
//	//		command_builder3( send_buffer, "Data", "LEDB", "Off" );
//	//	    }
//	//	}
//    }
//
//    // add new parameters as necessary
//    // NEVER check for a higher parameter number than we allocated for.
//    // see earlier comments about NULLS and dying from old age
//
//    return end_of_transmission_received;
//}
//
//void command_builder1( struct buffer *send_buffer, char* data1 )
//{
//    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
//    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
//    command_builder_add_string( send_buffer, data1 );
//    command_builder_add_char( send_buffer, COMMAND_END_CHAR );
//
//    return;
//}
//
//void command_builder2( struct buffer *send_buffer, char* data1, char* data2 )
//{
//    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
//    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
//    command_builder_add_string( send_buffer, data1 );
//    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
//    command_builder_add_string( send_buffer, data2 );
//    command_builder_add_char( send_buffer, COMMAND_END_CHAR );
//
//    return;
//}
//
//void command_builder3( struct buffer *send_buffer, char* data1, char* data2, char* data3 )
//{
//    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
//    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
//    command_builder_add_string( send_buffer, data1 );
//    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
//    command_builder_add_string( send_buffer, data2 );
//    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
//    command_builder_add_string( send_buffer, data3 );
//    command_builder_add_char( send_buffer, COMMAND_END_CHAR );
//
//    return;
//}
//
//void command_builder4( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4 )
//{
//    command_builder_add_char( send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
//    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
//    command_builder_add_string( send_buffer, data1 );
//    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
//    command_builder_add_string( send_buffer, data2 );
//    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
//    command_builder_add_string( send_buffer, data3 );
//    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
//    command_builder_add_string( send_buffer, data4 );
//    command_builder_add_char( send_buffer, COMMAND_END_CHAR );
//
//    return;
//}
//
//void command_builder_add_char( struct buffer *send_buffer, char data )
//{
//    send_buffer->data_buffer[send_buffer->write_position] = data;
//
//    send_buffer->write_position++;
//    if( send_buffer->write_position >= BUFFER_LENGTH )
//    {
//	send_buffer->write_position = 0;
//    }
//
//    return;
//}
//
//void command_builder_add_string( struct buffer *send_buffer, char *data_string )
//{
//    for( int inx = 0; data_string[inx] != CHAR_NULL; inx++ )
//    {
//	command_builder_add_char( send_buffer, data_string[inx] );
//    }
//
//    return;
//}
//
//bool send_data( struct buffer *send_buffer )
//{
//    bool send_end;
//
//    if( send_buffer->read_position == send_buffer->write_position )
//    {
//	send_end = true;
//    }
//    else
//    {
//	send_end = false;
//	if( SPI_send_data( send_buffer->data_buffer[send_buffer->read_position] ) == true )
//	{
//	    send_buffer->read_position++;
//	    if( send_buffer->read_position >= BUFFER_LENGTH )
//	    {
//		send_buffer->read_position = 0;
//	    }
//	}
//    }
//
//    return send_end;
//}
//
//bool strmatch( char* a, char* b )
//{
//    int result;
//    bool match;
//
//    static int co = 0;
//    co++;
//
//    result = strcmp2( a, b );
//
//    if( result == 0 )
//    {
//	match = true;
//    }
//    else
//    {
//	match = false;
//    }
//
//    return match;
//}
//
//int strcmp2( char* a, char* b )
//{
//    int inx = 0;
//    int match = 0;
//
//    while( (a[inx] != CHAR_NULL) && (b[inx] != CHAR_NULL) && (match == 0) )
//    {
//	if( a[inx] > b[inx] )
//	{
//	    match = 1;
//	}
//	else if( a[inx] < b[inx] )
//	{
//	    match = -1;
//	}
//	else if( a[inx] == b[inx] )
//	{
//	    //do nothing
//	}
//
//	inx++;
//    }
//
//    if( (a[inx] == CHAR_NULL) && (b[inx] != CHAR_NULL) )
//    {
//	match = -1;
//    }
//    else if( (a[inx] != CHAR_NULL) && (b[inx] == CHAR_NULL) )
//    {
//	match = 1;
//    }
//
//
//    return match;
//}
//
//bool SPI_receive_data( char *data )
//{
//    bool recvGood = false;
//
//    if( SPI1STATbits.SPIRBF == 1 )
//    {
//	*data = SPI1BUF;
//	recvGood = true;
//    }
//
//    return recvGood;
//}
//
//bool SPI_send_data( char data )
//{
//    bool sendGood = false;
//
//    if( SPI1STATbits.SPITBF == 0 ) //if in enhance mode use SPI1STATbits.SR1MPT
//    {
//	SPI1BUF = data;
//	sendGood = true;
//    }
//
//    return sendGood;
//}
//
///************************/
//// RESPONSES
//
//void send_end_of_transmission( struct buffer *send_buffer )
//{
//    command_builder1( send_buffer, "END" );
//
//    return;
//}
//
//void SPIMasterInit( void )
//{
//    static bool firstRun = true;
//
//    // make sure analog is turned off - it messes with the pins
//    ANSA = 0;
//    ANSB = 0;
//
//    TRISBbits.TRISB10 = 0b1; // SDI1
//    TRISBbits.TRISB11 = 0b1; // SCK1 (seems this should be set output, but 0b0 does not work)
//    TRISBbits.TRISB13 = 0b0; // SDO1
//
//    SPI_PORT_0_DIR = 0;
//    SPI_PORT_1_DIR = 0;
//    SPI_PORT_2_DIR = 0;
//
//    SPI_PORT_0 = 1;
//    SPI_PORT_1 = 1;
//    SPI_PORT_2 = 1;
//
//    //SPI1 Initialize
//    SPI1CON1bits.MSTEN = 1; //making SPI1 Master
//
//    SPI1CON1bits.DISSCK = 0b0; // SPI clock enabled
//    SPI1CON1bits.DISSDO = 0b0; // SDO used
//    SPI1CON1bits.MODE16 = 0b0; // 8 bit mode
//    SPI1CON1bits.SMP = 0b1; // sample phase mode end
//    SPI1CON1bits.CKE = 0b1; // serial data changes on active to idle clock state
//    SPI1CON1bits.SSEN = 0b0; // not a slave
//    SPI1CON1bits.CKP = 0b1; // clock idle is high
//    SPI1CON1bits.SPRE = 0b110; // secondary prescale 2:1
//    SPI1CON1bits.PPRE = 0b11; // primary prescale 1:1
//    //    SPI1CON1bits.PPRE = 0b00; // primary prescale 1:1
//
//    SPI1CON2bits.FRMEN = 0b0; // frame mode, unused
//    SPI1CON2bits.SPIFSD = 0b0; // frame mode, unused
//    SPI1CON2bits.SPIFPOL = 0b0; // frame mode, unused
//    SPI1CON2bits.SPIFE = 0b0; // frame mode, unused
//
//    SPI1CON2bits.SPIBEN = 0b0; // 1=enhanced buffer mode
//
//    SPI1STATbits.SPIROV = 0; //clear flag for overflow data
//
//
//    // do not use the interrupt , could not get it to work
//    //    SPI1STATbits.SISEL = 0b001;
//    //
//    //    IFS0bits.SPI1IF = 0;
//    //    IEC0bits.SPI1IE = 1;
//
//
//
//
//    //SPI1BUF = SPI1BUF;
//    //    SPI1STATbits.SPIEN = 1; //enable SPI
//
//    if( firstRun == true )
//    {
//	// set timer up here
//	T1CONbits.TSIDL = 0b1; //Discontinue module operation when device enters idle mode
//	T1CONbits.T1ECS = 0b00; // Timer1 uses Secondary Oscillator (SOSC) as the clock soource)
//	T1CONbits.TGATE = 0b0; // Gated time accumulation is disabled
//	T1CONbits.TSYNC = 0b0; // Do not synchronize external clock input (asynchronous)
//	T1CONbits.TCS = 0b0; //use internal clock
//
//
//	T1CONbits.TCKPS = 0b00; // Timer 1 Input Clock Prescale (11-256)(10-64) (01-8) (00-1)
//	TMR1 = 0x0000; // start timer at 0
//
//	T1CONbits.TON = 0b1; //turn on timer
//	firstRun = false;
//    }
//    return;
//}
//
////void __attribute__((__interrupt__,__auto_psv__)) _SPI1Interrupt(void)
////{
////    // we received a byte
////    IFS0bits.SPI1IF = 0;
////
////    rcvSPI = true;
////    rcvChar =  SPI1BUF;
////
////    return;
////
////}
//
//void communicationsUART( )
//{
//
//    // must do UART1 and UART2 separately
//
//    // recv
//    // send
//
//    // global volatile UART1 recv ready
//
//
//
//}
//
//void communicationsUART1( )
//{
//    static struct buffer send_buffer;
//    static struct buffer receive_buffer;
//
//
//
//
//}
//
//void communicationsUART2( )
//{
//    static struct buffer send_buffer;
//    static struct buffer receive_buffer;
//
//}
