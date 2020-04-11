#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUFF_SIZE 100
#define TRUE 1

// Method to handle SIGINT to kill child and parent on SIGINT
void kill_child( int signum ) {
	if( kill( 0, SIGINT ) == -1 ) perror( "send SIGINT to child");
	exit( EXIT_SUCCESS );
}

int main( int argc, char *argv[]) {
	int interval;		// How often to get metric
	int fildes[2];
	char buf[BUFF_SIZE];
	int i;	// General purpose counter


	if( argc > 1 ) {
		interval = atoi( argv[1]);
	} else {
		interval = 5;	// Set metrics interval to 5 seconds, if it is not specified as argument
	}

	if( pipe( fildes ) == -1 ) perror( "pipe");	// Create pipe for interprocess IO communication

	switch( fork( )) {
	case -1:
		perror( "fork");
		break;
	case 0:
		// Child process which wilkl output to STDOUT metrics provided by parent
		if( close( fildes[1]) == -1 ) perror( "client: close write");	// Close write fd of pipe, since I'm not going to write to it
		while( TRUE ) {
			for( i = 0; i < BUFF_SIZE; i++ ) buf[i] = '\0';
			if( raise( SIGSTOP ) != 0 ) perror( "client: raise SIGSTOP");	// Ask kernel to stop me, so when server will be ready to send data, it can ask kernel to resume me to read the new data
			if( read( fildes[0], &buf, BUFF_SIZE ) == -1 ){
				perror( "client: read");
			} else {
				strcat( buf, "\n" );
				write( 1, buf, strlen( buf ) );	// Write to parent's STDOUT
			}
		}
		if( close( fildes[0]) == -1 ) perror( "client: close read");
		exit( EXIT_SUCCESS );
	default:
		if( signal( SIGINT, kill_child ) == SIG_ERR ) perror( "server: seignal handler");	// On SIGINT don't forget to kill stopped child
		if( close( fildes[0]) == -1 ) perror( "server: close read");	// Close the read fd of the pipe since it is not needed for server

		const int number = 3;
		double a[number];
		long current_time;
		char string_time[20];
		int wstatus;

		while( TRUE ) {
			memset( buf, '\0', BUFF_SIZE );
			memset( string_time, '\0', 20 );

			// Get current seconds since EPOCH
			current_time =  (long)time( NULL );
			if( current_time == -1 ) perror( "time");

			// Assuming that data will be send in csv format with ',' as a delimiter
			if( sprintf( string_time, "%ld,", current_time ) == -1 ) perror( "server: sprintf time");
			// Write time to buffer
			strcat( buf, string_time );

			if( getloadavg( a, number ) == -1 ) perror( "getloadavg");

			for( i = 0; i < number; i++ ) {
				char string_metric[5];
				if( sprintf( string_metric, "%.2f,", a[i] ) == -1 ) perror( "server: sprintf metric");
				strcat( buf, string_metric );
			}

			// Wait until child will be in STOPped state
			pid_t w = waitpid( -1, &wstatus, WUNTRACED );
			if( w == -1 ) {
				perror( "server: wait for child failed");
				break;
			} else if( WIFSTOPPED( wstatus )) {
				// Send data to pipe
				if( write( fildes[1], buf, BUFF_SIZE ) == -1 ) perror( "server: write");
				// CONTIUNE child process to be able to read sent data
				if( kill( 0, SIGCONT ) == -1 ) perror( "server: send SIGCONT to child");
			} else {
				printf( "client is not waiting for data");
			}

			// Get next metric in `interval` seconds
			sleep( interval );
		}
		if( close( fildes[1]) == -1 ) perror( "server: close write");
	}
	return EXIT_SUCCESS;
}
