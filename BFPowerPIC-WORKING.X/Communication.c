

#include "Communications.h"


/* what data do we need

 which port are we on
    how do we determine it

 receive buffer
 send buffer

 anything else?



 */

#define BOOL unsigned char
#define TRUE 1
#define FALSE 0

#define BUFFER_LENGTH 40
#define PORT_COUNT 3 // one based count of the number of ports

#define PARAMETER_MAX_COUNT 5
#define PARAMETER_MAX_LENGTH 10


#define CHAR_NULL '\0'
#define COMMAND_SEND_RECEIVE_PRIMER_CHAR '#' // something to run the SPI clock so data can be received
#define COMMAND_START_CHAR '!'
#define COMMAND_END_CHAR '*'
#define COMMAND_DELIMETER ';'

struct buffer
{
    char data_buffer[ BUFFER_LENGTH];
    unsigned char write_position;
    unsigned char read_position;
};


BOOL SPI_receive_data ( char * );
void set_current_port ( unsigned char * );
BOOL receive_data ( struct buffer * );
BOOL process_data ( struct buffer *receive_buffer, struct buffer *send_buffer );
void process_data_parameterize ( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer *buffer_to_parameterize );
BOOL process_data_parameters ( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer *send_buffer );


void command_builder1 ( struct buffer *send_buffer, char* data1 );
void command_builder2 ( struct buffer *send_buffer, char* data1, char* data2 );
void command_builder3 ( struct buffer *send_buffer, char* data1, char* data2, char* data3 );
void command_builder4 ( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4 );
void command_builder_add_char ( struct buffer *send_buffer, char data );
void command_builder_add_data ( struct buffer *send_buffer, char *data );



BOOL send_data ( struct buffer *send_buffer );
BOOL SPI_send_data ( char data );



char strcmp2 ( char* a, char* b );
void strcpy2 ( char* rcv, char* source );


void send_end_of_transmission ( struct buffer *send_buffer );

/****************
 These functions will need to be replaced
 they are place holders for now
 */

BOOL SPI_receive_data ( char *data )
{


    char new_char = CHAR_NULL;
    BOOL is_char_good = FALSE;

    // new_char = GET CHAR FROM BUF


    if ( ( new_char > 0 ) && ( new_char < 255 ) )
    {
        *data = new_char;
        is_char_good = TRUE;
    }

    return is_char_good;
}

/***********************
 main code body
 */

void communications ( )
{
    static unsigned char current_port = PORT_COUNT; // port we are on - zero based - 0 to (PORT_COUNT - 1) we start with max so next port is 0
    static unsigned char current_port_done = TRUE; // start with true and let normal program mechanism automatically init things

    static struct buffer receive_buffer;
    static struct buffer send_buffer;

    static BOOL end_of_transmission_received = FALSE;
    BOOL no_more_to_send; // here to make this more readable

    // we need to move to next port ONLY at the right time
    // this still needs worked out
    // setting current_port_done needs more thought
    if ( current_port_done == TRUE )
    {
        set_current_port ( &current_port );

        receive_buffer.write_position = 0;
        receive_buffer.read_position = 0;

        // put something in the send buffer to run the clock
        send_buffer.write_position = 0;
        send_buffer.read_position = 0;
        command_builder_add_char ( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );

        current_port_done = FALSE;
        end_of_transmission_received = FALSE;
    }


    if ( receive_data ( &receive_buffer ) == TRUE )
    {
        if ( process_data ( &receive_buffer, &send_buffer ) == TRUE )
        {
            end_of_transmission_received = TRUE;
        }
    }

    no_more_to_send = send_data ( &send_buffer );

    if ( no_more_to_send == TRUE )
    {
        if ( end_of_transmission_received == TRUE )
        {
            current_port_done = TRUE;
        }
        else
        {
            command_builder_add_char ( &send_buffer, COMMAND_SEND_RECEIVE_PRIMER_CHAR );

        }
    }

}


// set the port
// for right now just make this SPI compatible
// we can change how this works later if we need to

void set_current_port ( unsigned char *current_port )
{
    ( *current_port )++;
    if ( *current_port >= PORT_COUNT )
    {
        *current_port = 0;
    }

    switch ( *current_port )
    {
    case 0:
        // set correct DO chip select here
        break;
    case 1:
        // set correct DO the chip select here
        break;
    case 2:
        // set correct DO the chip select here
        break;
    }

}

BOOL receive_data ( struct buffer *receive_buffer )
{
    char data;
    BOOL my_return = FALSE;

    enum receive_status
    {
        waiting,
        in_command,
        end_command,
    };

    static enum receive_status my_status;

    if ( SPI_receive_data ( &data ) == TRUE )
    {

        if ( ( data == COMMAND_START_CHAR ) && ( my_status != in_command ) )
        {
            my_status = in_command;
            receive_buffer->read_position = 0;
            receive_buffer->write_position = 0;
        }

        if ( my_status == in_command )
        {
            receive_buffer->data_buffer[ receive_buffer->write_position] = data;
            receive_buffer->write_position++;
            if ( receive_buffer->write_position >= BUFFER_LENGTH )
            {
                receive_buffer->write_position = ( BUFFER_LENGTH - 1 );
            }
        }

        if ( ( my_status == in_command ) && ( data == COMMAND_END_CHAR ) )
        {
            my_status = end_command;
            my_return = TRUE;
        }
    }

    return my_return;
}

BOOL process_data ( struct buffer *receive_buffer, struct buffer *send_buffer )
{

    BOOL end_of_transmission_received;

    // if we are here then the receive buffer must have good data with start and end command characters
    // the characters are not included as they were not added

    char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH];

    // init each parameter with null
    for ( int inx = 0; ( inx < PARAMETER_MAX_COUNT ); inx++ )
    {
        parameters[inx][0] = CHAR_NULL;
    }

    process_data_parameterize ( parameters, receive_buffer );

    end_of_transmission_received = process_data_parameters ( parameters, send_buffer );

    return end_of_transmission_received;

}

void process_data_parameterize ( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer *buffer_to_parameterize )
{

    unsigned char parameter_position = 0;
    unsigned char parameter_index = 0;

    // step through
    // only one command is expected due to the way we read
    // go through buffer until we hit end char or end of buffer


    while ( ( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position ] != COMMAND_END_CHAR ) && ( buffer_to_parameterize->read_position < BUFFER_LENGTH ) )
    {
        switch ( buffer_to_parameterize->data_buffer[buffer_to_parameterize->read_position] )
        {
        case COMMAND_START_CHAR:
        case COMMAND_END_CHAR:
            //do nothing with these characters
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

}

BOOL process_data_parameters ( char parameters[PARAMETER_MAX_COUNT][PARAMETER_MAX_LENGTH], struct buffer *send_buffer )
{

    BOOL end_of_transmission_received = FALSE;

    if ( strcmp2 ( parameters[0], "@" ) == 0 )
    {
        send_end_of_transmission ( send_buffer );
        end_of_transmission_received = TRUE;
    }
    else if ( strcmp2 ( parameters[0], "Set" ) == 0 )
    {
        if ( strcmp2 ( parameters[1], "Power" ) == 0 )
        {
            //set_power( parameters[3]);
            // send reply?
        }
        else if ( strcmp2 ( parameters[1], "Volts" ) == 0 )
        {
            //set_volts( parameters[2]);
            // send reply?
        }
        else if ( strcmp2 ( parameters[1], "Current" ) == 0 )
        {
            //set_current( parameters[3]);
            // send reply?
        }

    }
    else if ( strcmp2 ( parameters[0], "Read" ) == 0 )
    {
        if ( strcmp2 ( parameters[1], "Power" ) == 0 )
        {
            // send command power
        }
        else if ( strcmp2 ( parameters[1], "Volts" ) == 0 )
        {
            // send command volts
        }
        else if ( strcmp2 ( parameters[1], "Current" ) == 0 )
        {
            // send command current
        }

    }

    return end_of_transmission_received;

}

void command_builder1 ( struct buffer *send_buffer, char* data1 )
{
    command_builder_add_char ( send_buffer, COMMAND_START_CHAR );
    command_builder_add_data ( send_buffer, data1 );
    command_builder_add_char ( send_buffer, COMMAND_END_CHAR );
}

void command_builder2 ( struct buffer *send_buffer, char* data1, char* data2 )
{
    command_builder_add_char ( send_buffer, COMMAND_START_CHAR );
    command_builder_add_data ( send_buffer, data1 );
    command_builder_add_char ( send_buffer, COMMAND_DELIMETER );
    command_builder_add_data ( send_buffer, data2 );
    command_builder_add_char ( send_buffer, COMMAND_END_CHAR );
}

void command_builder3 ( struct buffer *send_buffer, char* data1, char* data2, char* data3 )
{
    command_builder_add_char ( send_buffer, COMMAND_START_CHAR );
    command_builder_add_data ( send_buffer, data1 );
    command_builder_add_char ( send_buffer, COMMAND_DELIMETER );
    command_builder_add_data ( send_buffer, data2 );
    command_builder_add_char ( send_buffer, COMMAND_DELIMETER );
    command_builder_add_data ( send_buffer, data3 );
    command_builder_add_char ( send_buffer, COMMAND_END_CHAR );
}

void command_builder4 ( struct buffer *send_buffer, char* data1, char* data2, char* data3, char* data4 )
{
    command_builder_add_char ( send_buffer, COMMAND_START_CHAR );
    command_builder_add_data ( send_buffer, data1 );
    command_builder_add_char ( send_buffer, COMMAND_DELIMETER );
    command_builder_add_data ( send_buffer, data2 );
    command_builder_add_char ( send_buffer, COMMAND_DELIMETER );
    command_builder_add_data ( send_buffer, data3 );
    command_builder_add_char ( send_buffer, COMMAND_DELIMETER );
    command_builder_add_data ( send_buffer, data4 );
    command_builder_add_char ( send_buffer, COMMAND_END_CHAR );
}

void command_builder_add_char ( struct buffer *send_buffer, char data )
{
    send_buffer->data_buffer[send_buffer->write_position] = data;
    send_buffer->write_position++;
    if ( send_buffer->write_position >= BUFFER_LENGTH )
    {
        send_buffer->write_position = 0;
    }
}

void command_builder_add_data ( struct buffer *send_buffer, char *data )
{
    for ( int inx = 0; data[inx] != CHAR_NULL; inx++ )
    {
        command_builder_add_char ( send_buffer, data[inx] );
    }

}

BOOL send_data ( struct buffer *send_buffer )
{
    BOOL send_end;

    if ( send_buffer->write_position == send_buffer->read_position )
    {
        send_end = TRUE;
    }
    else
    {
        send_end = FALSE;
        if ( SPI_send_data ( send_buffer->data_buffer[send_buffer->read_position] ) == TRUE )
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

BOOL SPI_send_data ( char data )
{
    BOOL send_good = FALSE;

    if ( TRUE == TRUE )// test if buffer can accept something
    {
        // add char to SPI send register
        send_good = TRUE;
    }

    return send_good;
}

char strcmp2 ( char* a, char* b )
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

void strcpy2 ( char* rcv, char* source )
{
    int inx;
    for ( inx = 0; source[inx] != CHAR_NULL; inx++ )
    {
        rcv[inx] = source[inx];
    }
    rcv[inx] = CHAR_NULL;

    return;
}




/************************/
// RESPONSES

void send_end_of_transmission ( struct buffer *send_buffer )
{
    command_builder1 ( send_buffer, "*" );
}
