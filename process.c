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
    p->priority = (rand()% Priority_Range) + 1;
    printf("PID:%d\tArrival time:%d\tPriority:%d\n", p->PID, p->Arrival_t, p->priority);
    printf("CPU, IO bursts : ");
    int ran;
    //p->CPU_IO_t bursts 개수 = 홀수, Interleave_MAX보다 작게
    int bursts = (((rand()%Interleave_MAX)/2)*2 + 1);
    // bursts index에 end state
    p->CPU_IO_t[bursts] = -1;
    // 짝수 index에는 CPU_burst, 홀수 index에는 IO_burst 시간 담겨있음.
    for (int index = 0; index < bursts; index++){
        // 각 burst time 정하기
        ran = (rand()%Each_Burst_MAX) + 1;
        p->CPU_IO_t[index] = ran;
        printf("%d  ", ran);
    }
    printf("\n");
    // cur_index 저장 변수
    p->cur_index = 0;

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

int burst_t_check(Process * p){
    int cur = p->cur_index;
    return (p->CPU_IO_t)[cur];
}

// Process 모든 burst 끝났으면 1, 아니면 0
int check_end(Process * p){
    int next_index = p->cur_index + 1;
    if (p->CPU_IO_t[next_index] == -1) return 1;
    else return 0;
}

int Arrival_t_check(Process * p){
    return p->Arrival_t;
}

int priority_dismiss(Process * p){
    return 1;
}