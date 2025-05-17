#ifndef PROCESS_H
#define PROCESS_H

#define Arrival_time_Range 10
#define Priority_Range 20
#define Interleave_MAX 5
#define Each_Burst_MAX 10

#define RR_time_quantum_T 2 
#define RR_time_quantum_F (Each_Burst_MAX + 2);

typedef struct process{
    int PID;
    int Arrival_t;
    int CPU_IO_t[Interleave_MAX+1];
    int cur_index;
    int priority;
}Process;

Process * Create_Process(int pid);

// Job Queue에서 Ready Queue로 들여올 때 Process 데이터만 복사 후 새로운 프로세스 만들기(편집 가능하도록)필요한가??
Process * Copy_Process(Process * old_process);

int priority_check(Process * p);
int burst_t_check(Process * p);
int Arrival_t_check(Process * p);
int priority_dismiss(Process * p);

int check_end(Process * p);

#endif