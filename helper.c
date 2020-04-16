#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Method to handle SIGINT to kill child and parent on SIGINT
void kill_child(int signum) {
	char error_msg[25];
	sprintf(error_msg, "send %d to child", signum);
	if(kill(0, SIGKILL) == -1) perror(error_msg);
	exit(EXIT_SUCCESS);
}

void init_kill_handler() {
	if(signal(SIGINT, kill_child) == SIG_ERR) perror("parent: signal handler");	// On SIGINT don't forget to kill stopped children
	if(signal(SIGTERM, kill_child) == SIG_ERR) perror("parent: signal handler");
}

void write_pid_file() {
	FILE *fp;

	fp = fopen("/var/run/qirimonit.pid", "w+");
	if(fp == NULL) perror("can't open pid file for write");
	if(fprintf(fp, "%d", getpid()) < 0) perror("can't write to pid file");
	if(fclose(fp) != 0) perror("close pid file");
}
