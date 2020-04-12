#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include "outputs.h"

#define BUFF_SIZE 100
#define TRUE 1


char *epoch_seconds() {
	long current_time;
	char *string_time = malloc(20);

	// Get current seconds since EPOCH
	current_time =  (long)time(NULL);
	if(current_time == -1) perror("time");

	// Assuming that data will be send in csv format with ',' as a delimiter
	if(sprintf(string_time, "%ld", current_time) == -1) perror("metric collector: sprintf time");
	return string_time;
}

char *load_avg() {
	const int number = 3;
	double a[number];
	char *string_load_avg = malloc(BUFF_SIZE);

	if(getloadavg(a, number) == -1) perror("getloadavg");

	int i;
	for(i = 0; i < number; i++) {
		char string_metric[5];
		// Assuming that data will be send in csv format with ',' as a delimiter
		if(sprintf(string_metric, "%.2f,", a[i]) == -1) perror("metric collector: sprintf metric");
		strcat(string_load_avg, string_metric);
	}
	return string_load_avg;
}

void write_pipe_metric(int write_fd, pid_t out_pid) {
	// Wait until child will be in STOPped state
	int wstatus;
	pid_t w = waitpid(out_pid, &wstatus, WUNTRACED);
	char buf[BUFF_SIZE];
	if(w == -1) {
		perror("metric collector: wait for child failed");
		exit(EXIT_FAILURE);
	} else if(WIFSTOPPED(wstatus)) {
		memset(buf, '\0', BUFF_SIZE);
		strcat(buf, epoch_seconds());
		strcat(buf, ",");
		// Write load average to buffer
		strcat(buf, load_avg());

		// Send data to pipe
		if(write(write_fd, &buf, BUFF_SIZE) == -1) perror("metric collector: write");
		// CONTIUNE child process to be able to read sent data
		if(kill(out_pid, SIGCONT) == -1) perror("metric collector: send SIGCONT to child");
	} else {
		perror("metric collector: output is not waiting for data");
	}
}

void init_sysinfo_metricc(int interval) {

	int fildes[2];
	pid_t out_pid;

	if(pipe(fildes) == -1) perror("pipe");	// Create pipe for interprocess IO communication
	out_pid = fork();
	if(out_pid == -1) {
		perror("fork output");
		exit(EXIT_FAILURE);
	} else if(out_pid == 0) {
		if(close(fildes[1]) == -1) perror("output: close write");	// Close write fd of pipe, since I'm not going to write to it
		init_output(fildes[0]);
	} else {
		char buf[BUFF_SIZE];
		if(close(fildes[0]) == -1) perror("metric collector: close read");	// Close the read fd of the pipe since it is not needed for metric collector
		while(TRUE) {
			write_pipe_metric(fildes[1], out_pid);
			// Get next metric in `interval` seconds
			sleep(interval);
		}
	}
	if(close(fildes[1]) == -1) perror("metric collector: close write");
}

