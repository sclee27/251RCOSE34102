#ifndef EVALUATION_H
#define EVALUATION_H

#include "process.h"
#include "queue.h"

typedef struct terminated_process{
    int PID;
    int arrival_t;
    int terminated_t;
    struct terminated_process * next;
}Terminated_Process;

typedef struct terminated_header{
    Terminated_Process * head;
    int cnt;
}Terminated_HEADER;


Terminated_Process * Create_TP(Process * p, int cur_clock);

Terminated_HEADER * Create_TQ();

int bursts_sum(int * arr);

int Insert_TP(Terminated_HEADER * terminated_queue, Process * p, int cur_clock);

void Evaluate(Terminated_HEADER * terminated_queue, HEADER * job_que, double * avg_turn_t, double * avg_wait_t);

int Del_TPQueue(Terminated_HEADER * terminated_queue);

void print_TP(Terminated_Process * p);
#endif