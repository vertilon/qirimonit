#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

// Method to handle SIGINT to kill child and parent on SIGINT
void kill_child(int signum) {
	if(kill(0, SIGINT) == -1) perror("send SIGINT to child");
	exit(EXIT_SUCCESS);
}

void init_kill_handler() {
	if(signal(SIGINT, kill_child) == SIG_ERR) perror("parent: signal handler");	// On SIGINT don't forget to kill stopped children
}
