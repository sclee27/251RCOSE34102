#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulator.h"
#include "queue.h"
#include "process.h"

// 유저로부터 hyperparameter 입력값을 받는 함수
void input_for_hyperparam(int * Arrival_r, int * Priority_r, int * Interleave_r, int * Each_Burst_r){
    while(1){
        printf("\nEnter Arrive_time_Range : ");
        scanf("%d", Arrival_r);
        if (*Arrival_r > 0) break;
    }
    while (1){
        printf("\nEnter Priority_Range : ");
        scanf("%d", Priority_r);
        if (*Priority_r > 0) break;
    }
    while(1){//Interleave_r positive, odd integer
        printf("\nNumber of interleaving CPU and IO bursts is postive, odd number < 20\nEnter Interleave_MAX : ");
        scanf("%d", Interleave_r);
        // break when input conditions matched
        if ((*Interleave_r < 20) &&(*Interleave_r > 0) && (*Interleave_r%2)) break;
    }
    while (1){
        printf("\nEnter Each_Burst_MAX : ");
        scanf("%d", Each_Burst_r);
        if (*Each_Burst_r > 0) break;
    }

}

// job_Queue, ready_Queue, wait_Queue 생성, Process들 랜덤 생성 후 job_Queue에 Arrival_time 순으로 정렬.
// Random Process Creation에서의 hyperparameter들의 값을 디폴트 값으로 할지, 유저가 직접 입력할지 선택.
// Each Burst Range 를 반환하여 RR이 아닐 때에 process가 preempt 당하지 않도록 time_quantum을 설정할 값을 반환.
// RR일 경우는 이 이후에 schedule함수에서 재선언한다.
int Config(HEADER ** job_q, HEADER ** ready_q, HEADER ** wait_q, int n_processes){
    Process * cur_p;
    NODE * cur_n;
    // Hyperparameters for Random Process Creation 디폴트 값으로 초기화
    int Arrival_r = Arrival_time_Range;
    int Priority_r = Priority_Range;
    int Interleave_r = Interleave_MAX;
    int Each_Burst_r = Each_Burst_MAX;

    int customize = -1;

    // job_que 생성 
    * job_q = Create_Que();
    if (*job_q == NULL){
        printf("Memory allocation failed in ready que header allocation.");
        return 0;
    }
    // customize input 0 또는 1로 입력될 때까지 loop.
    while ((customize != 0)&&(customize != 1)){
        printf("\nEnter 0 for default hyperparameter settings.\n<default>\nArrival_time_Range: %d\nPriority_Range: %d\nInterleave_MAX: %d\nEach_Burst_MAX: %d\n\n Enter 1 for hyperparameter settings.\ninput : ", Arrival_time_Range, Priority_Range, Interleave_MAX, Each_Burst_MAX);
        scanf("%d", &customize);
    }
    if (customize){ // Hyperparameters User setting for random process creation
        input_for_hyperparam(&Arrival_r, &Priority_r, &Interleave_r, &Each_Burst_r);
    }

    // n_processes 개수만큼 프로세스 생성
    for (int i =0; i<n_processes;i++){
        cur_p = Create_Process(i+1, Arrival_r, Priority_r, Interleave_r, Each_Burst_r);
        if (cur_p == NULL) {
            printf("Memory allocation failed in creating process allocation.\n");
            del_Queue(*job_q);
            return 0;
        }
        cur_n = Create_Node(cur_p);
        if (cur_n == NULL) {
            printf("Memory allocation failed in creating node allocation.\n");
            del_Queue(*job_q);
            return 0;
        }
        // job_que에 arrival time 순으로 sort
        push_node(*job_q, cur_n, Arrival_t_check);
    }
    
    // ready_que 생성
    * ready_q = Create_Que();
    if (*ready_q == NULL){
        printf("Memory allocation failed in ready que header allocation.");
        del_Queue(*job_q);
        return 0;
    }
    // wait_que 생성
    * wait_q = Create_Que();
    if (*wait_q == NULL){
        printf("Memory allocation failed in wait que header allocation.");
        del_Queue(*job_q);
        del_Queue(*ready_q);
        return 0;
    }
    return Each_Burst_r+1;
}



int main(int argc, char ** argv){
    // input [process 개수, scheduler algorithm 종류 코드] 확인, 처리
    if (argc != 3){
        printf("Program_name\tNumber_of_processes\tScheduler\n%d\tFCFS\n%d\tnon-preemptive SJF\n%d\tnon-preemptive Priority\n%d\tRound Robin\n%d\tpreemptive SJF\n%d\tpreemptive Priority\n%d\tFor ALL algorithms & Compare\n", FCFS, np_SJF, np_Pri, RR, p_SJF, p_Pri, ALL);
        printf("rerun the program.");
        return -1;
    }
    int n_processes = atoi(argv[1]);
    int scheduler_policy = atoi(argv[2]);
    if ((n_processes < 1)||(scheduler_policy <1)||(scheduler_policy > 7)){
        printf("Wrong input numbers. n_processes has to be positive integer, scheduler should be 1 to 7.\n");
        printf("Program_name\tNumber_of_processes\tScheduler\n\n%d\tFCFS\n%d\tnon-preemptive SJF\n%d\tnon-preemptive Priority\n%d\tRound Robin\n%d\tpreemptive SJF\n%d\tpreemptive Priority\n%d\tFor ALL algorithms & Compare\n", FCFS, np_SJF, np_Pri, RR, p_SJF, p_Pri, ALL);
        printf("rerun the program.");
        return -1;
    }

    // policy 인식, 프로세스 생성, job_queue 정비
    int (* check_for_ready_q)(Process *);
    int (* pnp_stop_condition) (Process *, HEADER *, int (*) (Process *));

    int time_quantum;
    srand(time(NULL));
    HEADER * job_queue, * ready_queue, *wait_queue;
    
    // process 생성, job_queue 정렬(Arrival_time 순서로)
    time_quantum = Config(&job_queue, &ready_queue, &wait_queue, n_processes);
    
    if (!time_quantum){ // time_quantum 0, Config 에러 반환값일 경우 처리
        return -1;
    }

    // 시뮬레이터 algorithm 설정, 0일 때 error 처리
    if (!schedule(scheduler_policy, &check_for_ready_q, &pnp_stop_condition, &time_quantum)){
        del_Queue(job_queue);
        del_Queue(wait_queue);
        del_Queue(ready_queue);
        return -1;
    }

    // hyperparamter 설정, 랜덤 프로세스 생성, 정렬된 job_queue와 비어있는 wait_queue, ready_queue 생성,
    // schedule 알고리즘을 위한 기본 도구 함수들까지 설정 완료된 상태. ALL일 때는 schedule 이루어지지 않았음.
    // 모든 설정 완료

    //시뮬레이션 가동
    /*
    if (scheduler_policy == 7){
        for (int i = 1;i<7;i++){
            printf("");
            if (!schedule(i, &check_for_ready_q, &pnp_stop_condition, &time_quantum)){
                del_Queue(job_queue);
                del_Queue(wait_queue);
                del_Queue(ready_queue);
                return -1;
            }
            simulator(job_queue, ready_queue, wait_queue, check_for_ready_q, pnp_stop_condition, time_quantum);
            printf("------------------\nSimulation Ended\n");
        }
    }
    else {
        simulator(job_queue, ready_queue, wait_queue, check_for_ready_q, pnp_stop_condition, time_quantum);
        printf("------------------\nSimulation Ended\n");
    }
    */

    simulator(job_queue, ready_queue, wait_queue, check_for_ready_q, pnp_stop_condition, time_quantum);
    printf("------------------\nSimulation Ended\n");

    del_Queue(job_queue);
    del_Queue(wait_queue);
    del_Queue(ready_queue);
    printf("Deleted All queues\n");
    return 0;
}