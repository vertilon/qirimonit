#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFF_SIZE 100
#define TRUE 1


void init_output(int read_fd) {
	// Child process which will output to STDOUT metrics provided by parent
	char buf[BUFF_SIZE];
	while(TRUE) {
		if(raise(SIGSTOP) != 0) perror("output: raise SIGSTOP");	// Ask kernel to stop me, so when metric collector will be ready to send data, it can ask kernel to resume me to read the new data
		memset(buf, '\0', BUFF_SIZE);
		if(read(read_fd, &buf, BUFF_SIZE) == -1){
			perror("output: read");
		} else {
			strcat(buf, "\n");
			write(1, buf, strlen(buf));	// Write to parent's STDOUT
		}
	}
	if(close(read_fd) == -1) perror("output: close read");
	exit(EXIT_SUCCESS);

}

