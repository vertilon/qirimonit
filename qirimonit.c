#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "helper.h"
#include "metrics.h"

#define TRUE 1


int main(int argc, char *argv[]) {
	int interval;		// How often to get metric
	pid_t mc_pid;


	if(argc > 1) {
		interval = atoi(argv[1]);
	} else {
		interval = 5;	// Set metrics interval to 5 seconds, if it is not specified as argument
	}


	mc_pid = fork();
	if(mc_pid == -1) {
		perror("fork metric collector");
		return EXIT_FAILURE;
	} else if(mc_pid == 0) {
		init_sysinfo_metricc(interval);
	} else {
		init_kill_handler();
		pause();
	}
	return EXIT_SUCCESS;
}
