#include <stdio.h>
#include <stdlib.h>
#include "process.h"

Process * Create_Process(int pid){
    Process * p = (Process *)malloc(sizeof(Process));
    if (p == NULL){
        printf("Memory allocation failure during Process Creation.");
        return NULL;
    }
    p->PID = pid;
    p->Arrival_t = (rand()% Arrival_time_Range) + 1;
    p->CPU_t = (rand()% CPU_time_Range) + 1;
    p->IO_t = (rand()% IO_time_Range) + 1;
    p->priority = (rand()% Priority_Range) + 1;
    printf("PID:%d\tArrival time:%d\tCPU burst time:%d\tI/O burst time:%d\tPriority:%d\n", p->PID, p->Arrival_t, p->CPU_t, p->IO_t, p->priority);
    return p;
}

// Job Queue에서 Ready Queue로 들여올 때 Process 데이터만 복사 후 새로운 프로세스 만들기(편집 가능하도록)필요한가??
Process * Copy_Process(Process * old_process){
    Process * new_process = (Process *)malloc(sizeof(Process));
    if (new_process == NULL){
        printf("Memory allocation failure during Process Copy.\n");
        return NULL;
    }
    *new_process = *old_process;
   printf("Process %d copied.\n", new_process->PID);
   return new_process;
}


int priority_check(Process * p){
    return p->priority;
}

int CPUt_check(Process * p){
    return p->CPU_t;
}

int Arrival_t_check(Process * p){
    return p->Arrival_t;
}

int priority_dismiss(Process * p){
    return 1;
}