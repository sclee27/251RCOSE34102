#include <stdio.h>
#include <stdlib.h>
#include "evaluation.h"


// bursts array 주소를 받아 모든 항 bursts 더한 값을 반환한다.
int bursts_sum(int * arr){
    int sum = 0;
    while (*arr != -1){
        sum += *arr;
        arr += 1;
    }
    return sum;
}

// Terminated Process 생성
Terminated_Process * Create_TP(Process * p, int cur_clock){
    Terminated_Process * new_tp = (Terminated_Process *)malloc(sizeof(Terminated_Process));
    if (new_tp == NULL) return NULL;
    new_tp->PID = p->PID;
    new_tp->arrival_t = p->Arrival_t;
    new_tp->terminated_t = cur_clock;
    new_tp->next= NULL;
    return new_tp;
}
// Terminated HEADER (큐) 생성 
Terminated_HEADER * Create_TQ(){
    Terminated_HEADER * new_tq = (Terminated_HEADER *)malloc(sizeof(Terminated_HEADER));
    if (new_tq == NULL) return NULL;
    new_tq->head = NULL;
    new_tq->cnt = 0;
    return new_tq;
}

// insert Terminated Process into Terminated Queue (process terminated할 때마다 simulator 함수 안에서 적용)
// (Job_Queue와 동일한 순서) arrvial_time 순 + 동일할 때는 PID 작은 순서로 정렬 
// 에러 반환값 -1, 정상 반환값 0
int Insert_TP(Terminated_HEADER * terminated_queue, Process * p, int cur_clock){
    // Create new Terminated Process 
    Terminated_Process * new_tp = Create_TP(p, cur_clock);
    if (new_tp == NULL) {
        printf("Failed memory allocation while saving Terminated Process.\n");
        return -1;
    }

    Terminated_Process * cur_tp = terminated_queue->head;
    Terminated_Process * prev_tp = NULL;

    while(cur_tp != NULL){
        // Arrival_time 더 작을 때 
        if(cur_tp->arrival_t > new_tp->arrival_t) break;
        // Arrival_time 같은데 PID 더 작을 때 (job_queue는 PID순)
        else if ((cur_tp->arrival_t == new_tp->arrival_t) && (cur_tp->PID > new_tp->PID)) break;
        else{
            prev_tp = cur_tp;
            cur_tp = cur_tp->next;
        }
    }
    if (prev_tp == NULL){
        terminated_queue->head = new_tp;
        new_tp->next = cur_tp;
    }
    else{
        prev_tp->next = new_tp;
        new_tp->next = cur_tp;
    }
    terminated_queue->cnt++;
    return 0;
}

// simulator 함수 안에서 while loop 끝난 뒤 사용. 
// simulator 안 while문 안에서 만들어진 terminated_queue와 변경되지 않은 job_queue 값을 이용해 avg_turn_t, avg_wait_t 계산 + 포인터에 저장
void Evaluate(Terminated_HEADER * terminated_queue, HEADER * job_que, double * avg_turn_t, double * avg_wait_t){
    if (terminated_queue->cnt != job_que->count){
        printf("job queue different from terminated queue...(Count)\n");
        *avg_turn_t = -1.1;
        *avg_wait_t = -1.1;
        return;
    }

    Terminated_Process * cur_tp = terminated_queue->head;
    NODE * curnode = job_que->head;

    int turn_t_sum = 0;
    int wait_t_sum = 0;
    int turnaround;

    while (cur_tp != NULL){
        if (cur_tp->PID != curnode->p->PID){
            printf("job queue different from terminated queue...(Order)\n");
            *avg_turn_t = -1.1;
            *avg_wait_t = -1.1;
            return;
        }
        // turn_t_sum에 current Process의 turnaround time 더하기
        print_TP(cur_tp);
        turnaround = (cur_tp->terminated_t - cur_tp->arrival_t);
        printf("turnaround : %d\n", turnaround);
        turn_t_sum += turnaround;
        // wait_t_sum에 current Process의 waiting time 더하기
        wait_t_sum += turnaround - bursts_sum(curnode->p->CPU_IO_t);
        
        cur_tp = cur_tp->next;
        curnode = curnode->next;
    }

    *avg_turn_t = ((double) turn_t_sum) / terminated_queue->cnt;
    *avg_wait_t = ((double) wait_t_sum) / terminated_queue->cnt;
}

// Terminated_Queue 제거&free
int Del_TPQueue(Terminated_HEADER * terminated_queue){
    if (terminated_queue == NULL){
        return -1;
    }
    Terminated_Process * cur_tp = terminated_queue->head;
    Terminated_Process * del_tp;
    while(cur_tp != NULL){
        del_tp = cur_tp;
        cur_tp = cur_tp ->next;
        free(del_tp);
    }
    free(terminated_queue);
    return 0;
}

void print_TP(Terminated_Process * p){
    printf("PID : %d\tArrival : %d\tTerminated : %d\n", p->PID, p->arrival_t, p->terminated_t);
}