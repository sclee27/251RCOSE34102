#ifndef PROCESS_H
#define PROCESS_H

#define Arrival_time_Range 10
#define Priority_Range 20
#define Interleave_MAX 5
#define Each_Burst_MAX 10

#define RR_time_quantum_default 2

typedef struct process{
    int PID;
    int Arrival_t;
    int CPU_IO_t[20];
    int cur_index;
    int priority;
}Process;

// Random Process Creation
Process * Create_Process(int pid, int Arrival_r, int Priority_r, int Interleave_r, int Each_burst_r);

int priority_check(Process * p);

int burst_t_check(Process * p);

int check_end(Process * p);

int Arrival_t_check(Process * p);

int priority_dismiss(Process * p);

#endif