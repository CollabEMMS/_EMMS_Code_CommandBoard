// UPDATED 2016-03-19


#include "Communications.h"
#include <stdbool.h>
#include <xc.h>
//#include <p24FV32KA302.h>

#define BUFFER_LENGTH 40  // max size is positive signed character size
#define PORT_COUNT 3 // one based count of the number of ports

// adjust this parameter as needed - there is no science to it
// if the delay is too short, then things don't work right
// a larger min program could make it that the number of skips is much less
#define DELAY_RUN_SKIPS 500 // don't run the cmm every loop - skip some  going too fast causes problems

#define PARAMETER_MAX_COUNT 5
#define PARAMETER_MAX_LENGTH 10

#define CHAR_NULL '\0'
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

enum receive_status
{
    receive_waiting,
    receive_in_command,
    receive_end_command
};

struct buffer
{
    char data_buffer[ BUFFER_LENGTH + 1];
    unsigned char write_position;
    unsigned char read_position;
};

extern void delayMS( int );

bool SPI_receive_data( char * );
void set_current_port( unsigned char * );
enum receive_status receive_data( struct buffer * );
bool process_data( struct buffer *receive_buffer, struct buffer *send_buffer );
void process_data_parameterize( char parameters[][PARAMETER_MAX_LENGTH], struct buffer *buffer_to_parameterize );
bool process_data_parameters( char parameters[][PARAMETER_MAX_LENGTH], struct buffer *send_buffer );

void command_builder1( struct buffer *send_buffer, char* data1 );
void command_builder2( struct buffer *send_buffer, char* data1, char* data2 );
void command_builder3( struct buffer *send_buffer, char* data1, char* data2, char* data3 );
void command_builder4( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4 );
void command_builder_add_char( struct buffer *send_buffer, char data );
void command_builder_add_string( struct buffer *send_buffer, char *data );

bool send_data( struct buffer *send_buffer );
bool SPI_send_data( char data );

bool strmatch( char* a, char* b );
int strcmp2( char* a, char* b );
void strcpy2( char* rcv, char* source );

void send_end_of_transmission( struct buffer *send_buffer );

/***********************
 main code body
 */

void communications( )
{
    static unsigned char current_port = PORT_COUNT; // port we are on - zero based - 0 to (PORT_COUNT - 1) we start with max so next port is 0
    static unsigned char current_port_done = true; // start with true and let normal program mechanism automatically init things

    struct buffer send_buffer;
    static struct buffer receive_buffer;

    static bool end_of_transmission_received = false;
    bool no_more_to_send; // here to make this more readable

    static enum receive_status receive_current_state;
    static unsigned char receive_wait_count;
    static unsigned char receive_in_command_count;

    static unsigned int delayRunCount = 0;

    // a delay was needed to make things run right
    // but a delay blocks the entire program
    // we just skip over comm a number of time the rest of the program can run
    // see notes with macro #define
    delayRunCount++;
    if ( delayRunCount >= DELAY_RUN_SKIPS )
    {
	delayRunCount = 0;

	if ( current_port_done == true )
	{
	    set_current_port( &current_port );

	    current_port_done = false;
	    end_of_transmission_received = false;

	    receive_wait_count = 0;
	    receive_in_command_count = 0;

	    // put something in the send buffer to run the clock
	    send_buffer.write_position = 0;
	    send_buffer.read_position = 0;
	    receive_buffer.write_position = 0;
	    receive_buffer.read_position = 0;

	    command_builder_add_char( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );
	}

	// this delay is replaced with the skips at the start of this function
	//	delayMS( 5 );
	receive_current_state = receive_data( &receive_buffer );
	switch ( receive_current_state )
	{
	case receive_waiting:
	    // count # of times we are waiting for COMMAND_START_CHAR !
	    receive_wait_count++;

	    break;
	case receive_in_command:
	    // count # of times we are in a command
	    // need to check if we somehow missed the COMMAND_END_CHAR *
	    // must be more than max length a command can be
	    receive_wait_count = 0;
	    receive_in_command_count++;

	    break;
	case receive_end_command:
	    if ( process_data( &receive_buffer, &send_buffer ) == true )
	    {
		end_of_transmission_received = true;
	    }
	    receive_wait_count = 0;
	    receive_in_command_count = 0;

	    break;
	}

	no_more_to_send = send_data( &send_buffer );

	if ( no_more_to_send == true )
	{
	    if ( end_of_transmission_received == true )
	    {
		// make sure trans buffer is empty
		// the following test is for standard buffer mode only
		// a different check must be performed if the enhanced buffer is used
		if ( SPI1STATbits.SPITBF == 0b0 ) // only for standard buffer
		{
		    current_port_done = true;
		}
	    }
	    else if ( receive_wait_count >= RECEIVE_WAIT_COUNT_LIMIT )
	    {
		// not receiving anything valid from slave
		// just move to the next port - things should clear up on their own eventually
		current_port_done = true;
	    }
	    else if ( receive_in_command_count >= RECEIVE_IN_COMMAND_COUNT_LIMIT )
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
    }

    return;
}


// set the port
// for right now just make this SPI compatible
// we can change how this works later if we need to

void set_current_port( unsigned char *current_port )
{
    // will it work without disabling the master port?
    SPI1STATbits.SPIEN = 0; //disable master SPI

    SPI_PORT_0 = 1; //disable slave select (1 is disabled)
    SPI_PORT_1 = 1; //disable slave select (1 is disabled)
    SPI_PORT_2 = 1; //disable slave select (1 is disabled)

    // this delay is likely not necessary and has been removed for now
    //    delayMS( 10 );

    ( *current_port )++;
    if ( *current_port >= PORT_COUNT )
    {
	*current_port = 0;
    }

    switch ( *current_port )
    {
    case 0:
	// set correct DO chip select here
	SPI_PORT_0 = 0; //enable Slave Select
	break;
    case 1:
	// set correct DO the chip select here
	SPI_PORT_1 = 0;

	break;
    case 2:
	// set correct DO the chip select here
	SPI_PORT_2 = 0;
	break;
    }

    SPI1STATbits.SPIEN = 1; //enable master SPI

    return;
}

enum receive_status receive_data( struct buffer *receive_buffer )
{
    char data;

    static enum receive_status my_status = receive_waiting;

    if ( my_status == receive_end_command )
    {
	my_status = receive_waiting;
    }

    if ( SPI_receive_data( &data ) == true )
    {
	if ( ( data == COMMAND_START_CHAR ) && ( my_status != receive_in_command ) )
	{
	    my_status = receive_in_command;
	    receive_buffer->read_position = 0;
	    receive_buffer->write_position = 0;
	}

	if ( my_status == receive_in_command )
	{
	    receive_buffer->data_buffer[ receive_buffer->write_position] = data;

	    receive_buffer->write_position++;
	    if ( receive_buffer->write_position >= BUFFER_LENGTH )
	    {
		receive_buffer->write_position = ( BUFFER_LENGTH - 1 );
	    }
	}

	if ( ( my_status == receive_in_command ) && ( data == COMMAND_END_CHAR ) )
	{
	    my_status = receive_end_command;
	}
    }

    return my_status;
}

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
    for ( int inx = 0; inx < PARAMETER_MAX_COUNT; inx++ )
    {
	parameters[inx][0] = CHAR_NULL;
    }

    while ( ( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position ] != COMMAND_END_CHAR ) && ( buffer_to_parameterize->read_position < BUFFER_LENGTH ) && ( buffer_to_parameterize->read_position != buffer_to_parameterize->write_position ) )
    {
	switch ( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position] )
	{
	case COMMAND_START_CHAR:
	    // this character should never appear
	    break;
	case COMMAND_DELIMETER:
	    // move to next parameter
	    parameter_position = 0;
	    parameter_index++;

	    if ( parameter_index >= PARAMETER_MAX_COUNT )
	    {
		// if we run out of parameters just overwrite the last one
		// we should never have this case, but this keeps us from crashing and burning
		parameter_index = ( PARAMETER_MAX_COUNT - 1 );
	    }

	    break;
	default:
	    // add the character to the current parameter
	    parameters[parameter_index][parameter_position] = buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position];
	    parameter_position++;
	    if ( parameter_position >= PARAMETER_MAX_LENGTH )
	    {
		// if our parameter is too long, just overwrite the last character
		// we should never have this case, but this keeps us from crashing and burning
		parameter_position = ( PARAMETER_MAX_LENGTH - 1 );
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

    if ( strmatch( parameters[0], "END" ) == true )
    {
	send_end_of_transmission( send_buffer );
	end_of_transmission_received = true;
    }
    else if ( strmatch( parameters[0], "Set" ) == true )
    {
	if ( strmatch( parameters[1], "Power" ) == true )
	{
	    //set_power( parameters[3]);
	    // send reply?
	}
	else if ( strmatch( parameters[1], "Volts" ) == true )
	{
	    //set_volts( parameters[2]);
	    // send reply?
	}
	else if ( strmatch( parameters[1], "Current" ) == true )
	{

	    //set_current( parameters[3]);
	    // send reply?
	}
	else if ( strmatch( parameters[1], "LED" ) == true )
	{
	    if ( strmatch( parameters[2], "On" ) == true )
	    {
		LATBbits.LATB4 = 1;
		command_builder3( send_buffer, "Conf", "LED", "On" );

	    }
	    else if ( strmatch( parameters[2], "Off" ) == true )
	    {
		LATBbits.LATB4 = 0;
		command_builder3( send_buffer, "Conf", "LED", "Off" );
	    }
	}
    }
    else if ( strmatch( parameters[0], "Read" ) == true )
    {
	if ( strmatch( parameters[1], "Power" ) == true )
	{
	    // send command power
	}
	else if ( strmatch( parameters[1], "Volts" ) == true )
	{
	    // send command volts
	}
	else if ( strmatch( parameters[1], "Current" ) == true )
	{
	    // send command current
	}
	else if ( strmatch( parameters[1], "LED" ) == true )
	{
	    if ( PORTBbits.RB4 == 0b1 )
	    {
		command_builder3( send_buffer, "Data", "LED", "On" );
	    }
	    else
	    {
		command_builder3( send_buffer, "Data", "LED", "Off" );
	    }
	}
    }

    // add new parameters as necessary
    // NEVER check for a higher parameter number than we allocated for.
    // see earlier comments about NULLS and dying from old age

    return end_of_transmission_received;
}

void command_builder1( struct buffer *send_buffer, char* data1 )
{
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );
    
    return;
}

void command_builder2( struct buffer *send_buffer, char* data1, char* data2 )
{
    command_builder_add_char( send_buffer, COMMAND_START_CHAR );
    command_builder_add_string( send_buffer, data1 );
    command_builder_add_char( send_buffer, COMMAND_DELIMETER );
    command_builder_add_string( send_buffer, data2 );
    command_builder_add_char( send_buffer, COMMAND_END_CHAR );
    
    return;
}

void command_builder3( struct buffer *send_buffer, char* data1, char* data2, char* data3 )
{
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

void command_builder_add_char( struct buffer *send_buffer, char data )
{
    send_buffer->data_buffer[send_buffer->write_position] = data;

    send_buffer->write_position++;
    if ( send_buffer->write_position >= BUFFER_LENGTH )
    {
	send_buffer->write_position = 0;
    }
    
    return;
}

void command_builder_add_string( struct buffer *send_buffer, char *data_string )
{
    for ( int inx = 0; data_string[inx] != CHAR_NULL; inx++ )
    {
	command_builder_add_char( send_buffer, data_string[inx] );
    }
    
    return;
}

bool send_data( struct buffer *send_buffer )
{
    bool send_end;

    if ( send_buffer->read_position == send_buffer->write_position )
    {
	send_end = true;
    }
    else
    {
	send_end = false;
	if ( SPI_send_data( send_buffer->data_buffer[send_buffer->read_position] ) == true )
	{
	    send_buffer->read_position++;
	    if ( send_buffer->read_position >= BUFFER_LENGTH )
	    {
		send_buffer->read_position = 0;
	    }
	}
    }

    return send_end;
}

bool strmatch( char* a, char* b )
{
    int result;
    bool match;

    if ( ( a[0] != CHAR_NULL ) || ( b[0] != CHAR_NULL ) )
    {
	result = strcmp2( a, b );

	if ( result == 0 )
	{
	    match = true;
	}
	else
	{
	    match = false;
	}

	//match = (result == 0) ? true : false;
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

    while ( ( a[inx] != CHAR_NULL ) && ( b[inx] != CHAR_NULL ) && ( match == 0 ) )
    {
	if ( a[inx] > b[inx] )
	{
	    match = 1;
	}
	else if ( a[inx] < b[inx] )
	{
	    match = -1;
	}
	else if ( a[inx] == b[inx] )
	{
	    //do nothing = never reset to zero
	}

	inx++;
    }

    return match;
}

/****************
 These functions will need to be replaced
 they are place holders for now
 */

bool SPI_receive_data( char *data )
{
    bool recvGood = false;

    if ( SPI1STATbits.SPIRBF == 0b1 )
    {
	*data = SPI1BUF;
	recvGood = true;
    }

    return recvGood;
}

bool SPI_send_data( char data )
{
    bool sendGood = false;

    if ( SPI1STATbits.SPITBF == 0b0 ) //if in enhance mode use SPI1STATbits.SR1MPT
    {
	SPI1BUF = data;
	sendGood = true;
    }

    return sendGood;
}

/************************/
// RESPONSES

void send_end_of_transmission( struct buffer *send_buffer )
{
    command_builder1( send_buffer, "END" );
    
    return;
}

void SPIMasterInit( void )
{

    // make sure analog is turned off - it messes with the pins
    ANSA = 0;
    ANSB = 0;

    SPI_PORT_0_DIR = 0;
    SPI_PORT_1_DIR = 0;
    SPI_PORT_2_DIR = 0;

    SPI_PORT_0 = 0;
    SPI_PORT_1 = 0;
    SPI_PORT_2 = 0;

    //SPI1 Initialize
    SPI1CON1bits.MSTEN = 1; //making SPI1 Master

    SPI1CON1bits.DISSCK = 0b0; // SPI clock enabled
    SPI1CON1bits.DISSDO = 0b0; // SDO used
    SPI1CON1bits.MODE16 = 0b0; // 8 bit mode
    SPI1CON1bits.SMP = 0b0; // sample phase mode middle
    SPI1CON1bits.CKE = 0b1; // serial data changes on active to idle clock state
    SPI1CON1bits.SSEN = 0b0; // not a slave
    SPI1CON1bits.CKP = 0b1; // clock idle is high
    SPI1CON1bits.SPRE = 0b000; // secondary prescale 8:1

    SPI1CON2bits.FRMEN = 0b0; // frame mode, unused
    SPI1CON2bits.SPIFSD = 0b0; // frame mode, unused
    SPI1CON2bits.SPIFPOL = 0b0; // frame mode, unused
    SPI1CON2bits.SPIFE = 0b0; // frame mode, unused

    SPI1CON2bits.SPIBEN = 0b0; // 1=enhanced buffer mode

    SPI1STATbits.SPIROV = 0; //clear flag for overflow data

    SPI1BUF = SPI1BUF;
    SPI1STATbits.SPIEN = 1; //enable SPI
    
    return;
}
