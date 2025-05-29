#include <stdio.h>
#include <stdlib.h>
#include "simulator.h"

int pre_stop_condition(Process * CPU_running_p, HEADER * ready_que, int check_for_ready_q(Process *p)){
    // CPU running process보다 우선순위 높으면(CPU burst time OR priority value 작으면) preempt
    if (ready_que->head == NULL) return 0;
    else if (check_for_ready_q(CPU_running_p) > check_for_ready_q(ready_que->head->p)) return 1;
    else return 0;
}

int nonpre_stop_condition(Process * CPU_running_p, HEADER * ready_que, int check_for_ready_q(Process *p)){
    return 0;
}



int IO_Management(HEADER * wait_que){
    if (wait_que->head == NULL) return 0;
    NODE * curnode = wait_que->head;
    int flag = 0;
    while (curnode != NULL){
        (curnode->p->CPU_IO_t)[curnode->p->cur_index]--;
        if (!burst_t_check(curnode->p)) flag = 1; // wait_que 중 IO_time 0된 Process 있으면 flag 1로 작동.
        curnode = curnode->next;
    }
    return flag;
}

int CPU_Management(Process * CPU_running_p, int * left_time_quantum, int clock){
    if (CPU_running_p == NULL) return 0;
    
    // decrement each counter
    ((CPU_running_p ->CPU_IO_t)[CPU_running_p->cur_index])--;
    *left_time_quantum = *left_time_quantum - 1;
    //printf("%d : Process [%d] (%d) CPU burst time left.\n", clock+1, CPU_running_p->PID, CPU_running_p->CPU_IO_t[CPU_running_p->cur_index]);
    // burst 끝나면 1
    if (!burst_t_check(CPU_running_p)) return 1;
    else return 0;
}

void wait2ready(HEADER * wait_que, HEADER * ready_que, int check_for_ready_q(Process * p), int clock){
    NODE * newnode;
    Process * IOcomplete_p;
    while ((wait_que->head != NULL )&&(!burst_t_check(wait_que->head->p))){
        IOcomplete_p = pop_node(wait_que);
        IOcomplete_p->cur_index++;
        printf("%d : Process [%d] completed IO burst, re-entering ready_queue.\n", clock, IOcomplete_p->PID);
        //printf("cur_index %d---------------------next index value : %d\n", IOcomplete_p->cur_index, IOcomplete_p->CPU_IO_t[(IOcomplete_p->cur_index) + 1]);
        newnode = Create_Node(IOcomplete_p);
        if (newnode == NULL) {
            printf("Failed memory allocation during creating node for ready queue. While simulator IO wait_queue management.\n");
            return;
        }
        push_node(ready_que, newnode, check_for_ready_q);
    }
}

void wait_or_terminate(Process * CPU_running_p, HEADER * wait_que, Terminated_HEADER * terminated_queue, int clock){
    //printf("cur_index %d---------------------next index value : %d\n", CPU_running_p->cur_index, CPU_running_p->CPU_IO_t[(CPU_running_p->cur_index) + 1]);
    if (check_end(CPU_running_p)){
        // terminated
        printf("%d : Process [%d] terminated.\n", clock, CPU_running_p->PID);
        // Terminated_queue에 현재 종료된 process 값 Terminated_Process 데이터로 바꿔 넣기.
        Insert_TP(terminated_queue, CPU_running_p, clock);
        // CPU_running_p Process * 타입 free 
        free(CPU_running_p);
    }
    else{
        // has IO burst left
        CPU_running_p->cur_index++;
        NODE * curnode = Create_Node(CPU_running_p);
        if (curnode == NULL) {
            printf("Failed memory allocation during creating node for wait queue. While simulator base stop condition.\n");
            return;
        }
        push_node(wait_que, curnode, burst_t_check);
        printf("%d : Process [%d] entered wait_que\n", clock, CPU_running_p->PID);
    }
}

// RR일 때만 time_quantum 초기화. 다른 알고리즘은 time_quantum에 저장된 값을 그대로 사용
int schedule(int policy, int (** check_for_ready_q)(Process *), int (** pnp_stop_condition) (Process *, HEADER *, int (*) (Process *)), int * time_quantum){
    int input = 0;
    switch(policy){
        case FCFS:
            *check_for_ready_q = priority_dismiss; // priority value 전부 무시, 선착순으로 
            *pnp_stop_condition = nonpre_stop_condition;
            break;
        case RR:
            // time_quantum 유저 설정 기능
            printf("\nDefault time quantum : %d\nTo set a different time quantum, enter 1. Else, enter 0.\ninput : ", RR_time_quantum_default);
            scanf("%d", &input);
            if (input){
                // time_quantum customized value
                printf("Enter time quantum : ");
                scanf("%d", &input);
                if (input < 1){
                    printf("Wrong input, time_quantum should be positive integer. Setting time_quantum to default...\n");
                    input = RR_time_quantum_default;
                }
                *time_quantum = input;
                printf("\n");
            }
            else{
                // default time_quantum
                *time_quantum = RR_time_quantum_default;
            }
            *check_for_ready_q = priority_dismiss; // priority value 전부 무시, 선착순으로
            *pnp_stop_condition = nonpre_stop_condition;
            break;
        case np_SJF:
            *check_for_ready_q = burst_t_check; // CPU 남은 burst time으로 판단 설정.
            *pnp_stop_condition = nonpre_stop_condition;
            break;
        case p_SJF:
            *check_for_ready_q = burst_t_check; // CPU 남은 burst time으로 판단 설정.
            *pnp_stop_condition = pre_stop_condition;
            break;

        case np_Pri:
            *check_for_ready_q = priority_check; // 우선수위 priority value로 판단 설정.
            *pnp_stop_condition = nonpre_stop_condition;
            break;
        case p_Pri:
            *check_for_ready_q = priority_check; // 우선수위 priority value로 판단 설정.
            *pnp_stop_condition = pre_stop_condition;
            break;
        case ALL:
            break;
        default:
            printf("policy number should be integer of 1 to 6.\n");
            return 0;
    }
    return 1;
}


int simulator(HEADER * job_queue, HEADER * ready_queue, HEADER * wait_queue, int check_for_ready_q(Process * p), int pnp_stop_condition(Process * CPU_running_p, HEADER * ready_que, int check(Process *p)), int full_time_quantum, double * avg_turn_t, double * avg_wait_t){
    int clock = 0;
    int left_time_quantum = full_time_quantum;
    
    // next_entering_p를 job_Queue 시작

    // job용 노드, copied process
    NODE * next_entering_node = job_queue->head;
    Process * next_copied_P;

    Process * CPU_running_p = NULL;
    Process * preempt_p;
    NODE * curnode;
    int CPU_finished = 0;
    int interrupt = 0;
    
    // Terminated Queue 생성 
    Terminated_HEADER * terminated_queue = Create_TQ();
    if (terminated_queue == NULL) return -1;

    printf("\n\nSimulation Started\n---------Gantt Chart---------\n");

    // simulation 시작 (ready_que, wait_que, job_que empty, No running CPU여야 종료)

    while(!(empty_Queue(ready_queue) && empty_Queue(wait_queue) && (next_entering_node == NULL)&&(CPU_running_p == NULL))){
        // ENTER
        // Enter Ready Q : job_que에서 arrival time 되면 ready queue로 process 이동
        while ((next_entering_node != NULL) && (next_entering_node->p->Arrival_t == clock)){
            next_copied_P = Copy_Process(next_entering_node->p);
            printf("%d : Process [%d] arrived\n", clock, next_copied_P->PID);
            // next_entering_p를 ready_que에 넣는다.
            curnode = Create_Node(next_copied_P);
            if (curnode == NULL) {
                printf("Failed memory allocation during creating node for ready queue.\n");
                return -1;
            }
            push_node(ready_queue, curnode, check_for_ready_q); // check는 지정 필요!! scheduler마다 다르게
            next_entering_node = next_entering_node->next;
        }

        // Enter CPU
        // CPU 작동 중 없는데, ready_que에 process 있다면 작동
       if ((CPU_running_p == NULL) && (!empty_Queue(ready_queue))){
            CPU_running_p = pop_node(ready_queue);
            // new process running in CPU
            printf("%d : CPU starts running Process [%d].\n", clock, CPU_running_p->PID);
            //set up time_quantum
            left_time_quantum = full_time_quantum;
        }

        else if ((CPU_running_p != NULL) && pnp_stop_condition(CPU_running_p, ready_queue, check_for_ready_q)){
            preempt_p = CPU_running_p;
            CPU_running_p = pop_node(ready_queue);
            printf("%d : Process [%d] (%d) preempted by Process [%d] (%d)\n", clock, preempt_p->PID, check_for_ready_q(preempt_p), CPU_running_p->PID, check_for_ready_q(CPU_running_p));
            // 실행하던 process를 ready_que에 넣는다.
            curnode = Create_Node(preempt_p);
            if (curnode == NULL) {
                printf("Failed memory allocation during creating node for ready queue.\n");
                return -1;
            }
            push_node(ready_queue, curnode, check_for_ready_q);
            //set up time_quantum
            left_time_quantum = full_time_quantum;
        }
        
        //TIME ELAPSE (Change)
        // CPU decrement
        CPU_finished = CPU_Management(CPU_running_p, &left_time_quantum, clock);

        // IO decrement
        interrupt = IO_Management(wait_queue);

        // CLOCK MOVED
        clock++;
        printf("\n%d : \n", clock);
        //printf("%d : time quantum (%d)\n", clock, left_time_quantum);

        // RESULT (Check)

        // finished IO -> Enter ready que
        if (interrupt){
            // There exists a process in wait_que that completed its IO burst
            wait2ready(wait_queue, ready_queue, check_for_ready_q, clock);
            interrupt = 0;
        }
        // finished CPU-> enter wait que or terminate
        if (CPU_finished){
            wait_or_terminate(CPU_running_p, wait_queue, terminated_queue, clock);
            CPU_running_p = NULL;
            left_time_quantum = full_time_quantum;
        }
        // finished time_quantum
        else if (!left_time_quantum){
            // CPU_running_p를 ready_que에 넣는다.
            curnode = Create_Node(CPU_running_p);
            if (curnode == NULL) {
                printf("Failed memory allocation during creating node for ready queue.\n");
                return -1;
            }
            push_node(ready_queue, curnode, check_for_ready_q);
            printf("%d : Process [%d] re-entered ready_que, due to time_quantum\n", clock, CPU_running_p->PID);
            CPU_running_p = NULL;
            left_time_quantum = full_time_quantum;
        }
    }
    // while LOOP 종료, ready_queue, empty_queue empty
    printf("-----------------------------\nSimulation Ended\n");
    Evaluate(terminated_queue, job_queue, avg_turn_t, avg_wait_t);
    printf("\navg turnaround time : %.2lf\tavg wait time : %.2lf\n\n\n", *avg_turn_t, *avg_wait_t);
    Del_TPQueue(terminated_queue);
    return 0;
}
// job_queue 차 있는 상태, ready_queue & wait_queue 비어있는 상태로 종료, terminated_queue 삭제




// 모든 스케쥴링 알고리즘 시뮬레이션 진행
int Simulate_All(HEADER * job_queue, HEADER * ready_queue, HEADER * wait_queue, int (** check_for_ready_q)(Process *), int (** pnp_stop_condition) (Process *, HEADER *, int (*) (Process *)), int * time_quantum, double * avg_turn_t, double * avg_wait_t){

    // False_time_quantum 값 int변수에 저장해놓기.
    int False_time_quantum = *time_quantum;

    // 각 알고리즘마다 iterate.
    for (int i = 1;i<7;i++){
        printf("\n\n\n");
        print_Algorithm(i);
        printf(" Scheduling\n");
        // 각 알고리즘마다 check_for_ready_q, pnp_stop_condition 함수 초기화, time_quantum도 초기화화
        if (!schedule(i, check_for_ready_q, pnp_stop_condition, time_quantum)){
            del_Queue(job_queue);
            del_Queue(wait_queue);
            del_Queue(ready_queue);
            return -1;
        }
        simulator(job_queue, ready_queue, wait_queue, *check_for_ready_q, *pnp_stop_condition, *time_quantum, &(avg_turn_t[i-1]), &(avg_wait_t[i-1]));
        // ready_queue, wait_queue 전부 empty_queue인 상태, Just Created 상태로 종료하였다.
        // Job_queue 그대로, terminated_queue는 삭제.
        // evaluate도 완료되어 avg_turn_t, avg_wait_t 배열의 i-1 index에 저장됨.

        // time_quantum 초기화
        *time_quantum = False_time_quantum;
    }
    printf("\n\n\nComparison\n\n");
    for (int i = 1; i<7;i++){
        print_Algorithm(i);
        printf(" Scheduling\n");
        printf("Average Turnaround time : %.2lf\t\tAverage Waiting time : %.2lf\n\n", avg_turn_t[i-1], avg_wait_t[i-1]);
    }
    return 0;
}

// job_queue 차 있는 상태, ready_queue & wait_queue 비어있는 상태로 종료, terminated_queue 삭제




// Algorithm 숫자 코드를 알고리즘 이름으로 출력 
void print_Algorithm(int policy){
    switch(policy){
        case FCFS:
            printf("FCFS");
            break;
        case RR:
            printf("Round Robin");
            break;
        case np_SJF:
            printf("SJF");
            break;
        case p_SJF:
            printf("preemptive SJF");
            break;

        case np_Pri:
            printf("Priority");
            break;
        case p_Pri:
            printf("preemptive Priority");
            break;
        default:
    }
}