#include <stdio.h>
#include <stdlib.h>
#include "process.h"

// Process 생성, Process * 타입 동적 할당 후 저장
Process * Create_Process(int pid, int Arrival_r, int Priority_r, int Interleave_r, int Each_burst_r){
    Process * p = (Process *)malloc(sizeof(Process));
    if (p == NULL){
        printf("Memory allocation failure during Process Creation.");
        return NULL;
    }
    p->PID = pid;
    p->Arrival_t = (rand()% Arrival_r) + 1;
    p->priority = (rand()% Priority_r) + 1;
    printf("\nPID:%d\tArrival time:%d\tPriority:%d\n", p->PID, p->Arrival_t, p->priority);
    printf("CPU, IO bursts : ");
    int ran;
    //p->CPU_IO_t bursts 개수 = 홀수, Interleave_MAX보다 작게
    int bursts = (((rand()%Interleave_r)/2)*2 + 1);
    // bursts index에 end state
    p->CPU_IO_t[bursts] = -1;
    // 짝수 index에는 CPU_burst, 홀수 index에는 IO_burst 시간 담겨있음.
    for (int index = 0; index < bursts; index++){
        // 각 burst time 정하기
        ran = (rand()%Each_burst_r) + 1;
        p->CPU_IO_t[index] = ran;
        printf("%d  ", ran);
    }
    printf("\n");
    // cur_index 저장 변수
    p->cur_index = 0;

    return p;
}

// Job_queue에서 Ready_queue로 arrive할 때 Job_queue 동일하게 유지하기 위해 Process 복사.
// Process 주소를 받아 Process 값들을 복사한 뒤 새로운 Process 주소 동적할당
// 주소를 반환
Process * Copy_Process(Process * old_process){
    if (old_process == NULL) return NULL;
    
    Process * new_process = (Process *)malloc(sizeof(Process));
    if (new_process == NULL){
        printf("Memory allocation failure during Process Copy.\n");
        return NULL;
    }
    new_process->PID = old_process->PID;
    new_process->Arrival_t = old_process->Arrival_t;
    for (int i =0; i<20;i++){
        new_process->CPU_IO_t[i] = old_process->CPU_IO_t[i];
        if (new_process->CPU_IO_t[i] == -1) break;
    }
    new_process->cur_index = old_process->cur_index;
    new_process->priority = old_process->priority;
    return new_process;
}

// Ready_queue에서 Scheduling (comparison) 기준으로 사용되는
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