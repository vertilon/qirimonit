#ifndef _METRICS_H_
#define _METRICS_H_
 
char *epoch_seconds(void);
void append_load_avg(char[]);
void write_pipe_metric(int, int);
void init_sysinfo_metricc(int);
  
#endif
