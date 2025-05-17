#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulator.h"
#include "queue.h"
#include "process.h"

// job_Queue, ready_Queue, wait_Queue 생성, Process들 랜덤 생성 후 job_Queue에 Arrival_time 순으로 정렬.
int Config(HEADER ** job_q, HEADER ** ready_q, HEADER ** wait_q, int n_processes){
    Process * cur_p;
    NODE * cur_n;
    // job_que에 프로세스 생성 후 arrival time 순으로 sort
    * job_q = Create_Que();
    if (*job_q == NULL){
        printf("Memory allocation failed in ready que header allocation.");
        return 0;
    }
    for (int i =0; i<n_processes;i++){
        cur_p = Create_Process(i+1);
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
    return 1;
}



//printf("%d %lf %lf %lf %d", p->PID, p->Arrival_t, p->CPU_t, p->IO_t, p->priority);


int main(int argc, char ** argv){
    // input 확인, 처리
    if (argc != 3){
        printf("Program_name\tNumber_of_processes\tScheduler\n%d\tFCFS\n%d\tnon-preemptive SJF\n%d\tnon-preemptive Priority\n%d\tRound Robin\n%d\tpreemptive SJF\n%d\tpreemptive Priority\n", FCFS, np_SJF, np_Pri, RR, p_SJF, p_Pri);
        printf("rerun the program.");
        return -1;
    }
    int n_processes = atoi(argv[1]);
    int scheduler_policy = atoi(argv[2]);
    if ((n_processes < 1)||(scheduler_policy <1)||(scheduler_policy > 6)){
        printf("Wrong input numbers. n_processes has to be positive integer, scheduler should be 1 to 6.\n");
        return -1;
    }

    // policy 인식, 프로세스 생성, job_queue 정비
    int (* check_for_ready_q)(Process *);
    int (* pnp_stop_condition) (Process *, HEADER *, int (*) (Process *));
    int time_quantum;

    srand(time(NULL));
    HEADER * job_queue, * ready_queue, *wait_queue;
    
    // process 생성 완료, job_queue 정렬 완료(Arrival_time 순서로)
    Config(&job_queue, &ready_queue, &wait_queue, n_processes);

    // 시뮬레이터 설정, 0일 때 error 처리
    if (!schedule(scheduler_policy, &check_for_ready_q, &pnp_stop_condition, &time_quantum)){
        del_Queue(job_queue);
        del_Queue(wait_queue);
        del_Queue(ready_queue);
        return 0;
    }

    //시뮬레이션 가동
    simulator(job_queue, ready_queue, wait_queue, check_for_ready_q, pnp_stop_condition, time_quantum);
    printf("------------------\nSimulation Ended\n");

    del_Queue(job_queue);
    del_Queue(wait_queue);
    del_Queue(ready_queue);
    printf("Deleted All queues\n");
    return 1;
}