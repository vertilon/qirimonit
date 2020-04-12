#ifndef _METRICS_H_
#define _METRICS_H_
 
char *epoch_seconds(void);
char *load_avg(void);
void write_pipe_metric(int, int);
void init_sysinfo_metricc(int);
  
#endif
