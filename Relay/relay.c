/*
USB-RLY06 Linux example.

compile:
	gcc linux-usb-rly06.c -o rly06

By James Henderson, 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

struct termios defaults;							// to store innitial default port settings
struct termios config;								// These will be our new settings

char sbuf[ 10 ];

void closePort( int fd ) {

	if ( tcsetattr( fd, TCSANOW, &defaults ) < 0 ) {
		perror( "tcsetattr config" );   	// Set options for port
	}
	close( fd );
	
}

int openPort( void ) {

	const char *device = "/dev/ttyACM0";
	int fd = open( device, O_RDWR | O_NOCTTY );

	if(fd == -1) {
		perror( "failed to open port\n" );
		exit( 1 );
	
	} else {
		if ( tcgetattr( fd, &defaults ) < 0 ) {
			perror("tcgetattr");  		// Grab snapshot settings  for port
			closePort( fd );
			exit( 1 );
		}

		cfmakeraw( &config );							// make options for raw data
		config.c_cc[ VMIN ]  = 2;
		config.c_cc[ VTIME ] = 50; 							

		if ( tcsetattr( fd, TCSANOW, &config ) < 0 ) {
			perror( "tcsetattr config" );   	// Set options for port
			closePort( fd );
			exit( 1 );		
		}
	}

	return fd;

}

void displayHelp( void )
{
		printf( "Usage: RLY06 -[option] <RELAYSTATE>\n\n" );			// Display help text to screen
		printf( "<RELAYSTATE>\n ");
		printf( "	A six digit binary string such as 010101\n where a 0 is a relay that is off and a 1 is a relay that is on.\n" );
}

int getStates( char bin[] )
{
int sum, x, i, b;

	sum = 0;						// will hold the value of the binary number input
	
	b = 5;
	/*
	Create the binary value input from a character array by looking
	at each element and shifting a 1 or 0 into place in a char.
	*/
	for( i = 0; i < 6; i++ )
	{
		x = 0;						// Clear x before we put anthing into it

		if ( bin[i] == '1' )
		{
			x = 0x01 << b;				// Shift left b times
		}
		else if( bin[i] == '0' )
		{
			x = 0x00 << b;
		}
		else
		{
			printf( "Invalid input.\n\n" );		// If neither a 1 or 0 is found display and error and return 1
			exit( 1 );
		}

		b--;						// Will shift value by 1 less place next loop round

		sum += x;
	}
	return( sum );
}

void writeData( int fd, int nbytes )
{
int bytes;
	
	bytes = write( fd, sbuf, nbytes );					// Write nbytes of data from wbuf
	if(bytes == -1)							// If write returns an error (-1)
	{
		perror( "writeData: Error while trying to write data - " );	
		closePort( fd );
		exit( 1 );
	}
	else if( bytes != nbytes )
	{
		printf( "only %u bytes written out of %u requested\n", bytes, nbytes );
		closePort( fd );
		exit( 1 );	
	}
}


void readData( int fd, int nbytes )
{
int bytes;

	bytes = read( fd, sbuf, nbytes );						// Read nbytes of data into rbuf
	if( bytes == -1 )							// If read returns and error (-1)
	{
		perror( "readData: Error while trying to read data - " );
		closePort( fd );
		exit( 1 );
	}
	else if( bytes != nbytes )
	{
		printf( "Only %u bytes read out of %u requested\n", bytes, nbytes );
		closePort( fd );
		exit( 1 );
	}
}

void displayData( int fd )
{
	sbuf[ 0 ] = 0x5A;					// Sending this byte will make the USBRLY08 return its module number and the version number
	
	writeData( fd, 1 );
	
	readData( fd, 2 );				// Read the data back
	
	printf( "Module ID  : %d\n", sbuf[ 0 ] );		// Display data
	printf( "Software v : %d\n\n", sbuf[ 1 ] );
	
}

void setRelays( int fd, int state )
{	
		sbuf[ 0 ] = 0x5C;				// Set all relay states
		sbuf[ 1 ] = state;			// To value in state
		writeData( fd, 2 );
}

int main(int argc, char *argv[] ) {

	int port = openPort();

	displayData( port );

	if ( argc < 2 ) {
		displayHelp();
		exit( 1 );
	}

	int states = getStates( argv[ 1 ] );

	setRelays( port, states );

	close( port );
		

	return 0;
}
