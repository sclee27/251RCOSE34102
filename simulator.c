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
    printf("%d : Process [%d] (%d) CPU burst time left.\n", clock+1, CPU_running_p->PID, CPU_running_p->CPU_IO_t[CPU_running_p->cur_index]);
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
        printf("---------------------%d : Process [%d] completed IO burst, re-entering ready_queue.\n", clock, IOcomplete_p->PID);
        //printf("cur_index %d---------------------next index value : %d\n", IOcomplete_p->cur_index, IOcomplete_p->CPU_IO_t[(IOcomplete_p->cur_index) + 1]);
        newnode = Create_Node(IOcomplete_p);
        if (newnode == NULL) {
            printf("Failed memory allocation during creating node for ready queue. While simulator IO wait_queue management.\n");
            return;
        }
        push_node(ready_que, newnode, check_for_ready_q);
    }
}

void wait_or_terminate(Process * CPU_running_p, HEADER * wait_que, int clock){
    //printf("cur_index %d---------------------next index value : %d\n", CPU_running_p->cur_index, CPU_running_p->CPU_IO_t[(CPU_running_p->cur_index) + 1]);
    if (check_end(CPU_running_p)){
        // terminated
        printf("---------------------%d : Process [%d] terminated.\n", clock, CPU_running_p->PID);
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
        printf("---------------------%d : Process [%d] entered wait_que\n", clock, CPU_running_p->PID);
    }
}

int schedule(int policy, int (** check_for_ready_q)(Process *), int (** pnp_stop_condition) (Process *, HEADER *, int (*) (Process *)), int * time_quantum){
    int input = 0;
    switch(policy){
        case FCFS:
            *check_for_ready_q = priority_dismiss; // priority value 전부 무시, 선착순으로 
            *pnp_stop_condition = nonpre_stop_condition;
            break;
        case RR:
            // time_quantum 유저 설정 기능
            printf("Default time quantum : %d\nTo set a different time quantum, enter 1. Else, enter 0.\ninput : ", RR_time_quantum_default);
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
        default:
            printf("policy number should be integer of 1 to 6.\n");
            return 0;
    }
    return 1;
}


int simulator(HEADER * job_queue, HEADER * ready_que, HEADER * wait_que,  int check_for_ready_q(Process * p), int pnp_stop_condition(Process * CPU_running_p, HEADER * ready_que, int check(Process *p)), int full_time_quantum){
    int clock = 0;
    int left_time_quantum = full_time_quantum;
    Process * next_entering_p = pop_node(job_queue);
    Process * CPU_running_p = NULL;
    Process * preempt_p;
    NODE * curnode;
    int CPU_finished = 0;
    int interrupt = 0;
    
    while(!(empty_Queue(ready_que) && empty_Queue(wait_que) && (next_entering_p == NULL)&&(CPU_running_p == NULL))){
        // ENTER
        // Enter Ready Q : job_que에서 arrival time 되면 ready queue로 process 이동
        while ((next_entering_p != NULL) && (next_entering_p->Arrival_t == clock)){
            printf("%d : Process [%d] arrived\n", clock, next_entering_p->PID);
            // next_entering_p를 ready_que에 넣는다.
            curnode = Create_Node(next_entering_p);
            if (curnode == NULL) {
                printf("Failed memory allocation during creating node for ready queue.\n");
                return -1;
            }
            push_node(ready_que, curnode, check_for_ready_q); // check는 지정 필요!! scheduler마다 다르게
            next_entering_p = pop_node(job_queue);
        }

        // Enter CPU
        // CPU 작동 중 없는데, ready_que에 process 있다면 작동
       if ((CPU_running_p == NULL) && (!empty_Queue(ready_que))){
            CPU_running_p = pop_node(ready_que);
            // new process running in CPU
            printf("---------------------%d : CPU starts running Process [%d].\n", clock, CPU_running_p->PID);
            //set up time_quantum
            left_time_quantum = full_time_quantum;
        }

        else if ((CPU_running_p != NULL) && pnp_stop_condition(CPU_running_p, ready_que, check_for_ready_q)){
            preempt_p = CPU_running_p;
            CPU_running_p = pop_node(ready_que);
            printf("---------------------%d : Process [%d] (%d) preempted by Process [%d] (%d)\n", clock, preempt_p->PID, check_for_ready_q(preempt_p), CPU_running_p->PID, check_for_ready_q(CPU_running_p));
            // 실행하던 process를 ready_que에 넣는다.
            curnode = Create_Node(preempt_p);
            if (curnode == NULL) {
                printf("Failed memory allocation during creating node for ready queue.\n");
                return -1;
            }
            push_node(ready_que, curnode, check_for_ready_q);
            //set up time_quantum
            left_time_quantum = full_time_quantum;
        }
        
        //TIME ELAPSE (Change)
        // CPU decrement
        CPU_finished = CPU_Management(CPU_running_p, &left_time_quantum, clock);

        // IO decrement
        interrupt = IO_Management(wait_que);

        // CLOCK MOVED
        clock++;
        //printf("%d : time quantum (%d)\n", clock, left_time_quantum);

        // RESULT (Check)

        // finished IO -> Enter ready que
        if (interrupt){
            // There exists a process in wait_que that completed its IO burst
            wait2ready(wait_que, ready_que, check_for_ready_q, clock);
            interrupt = 0;
        }
        // finished CPU-> enter wait que or terminate
        if (CPU_finished){
            wait_or_terminate(CPU_running_p, wait_que, clock);
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
            push_node(ready_que, curnode, check_for_ready_q);
            printf("---------------------%d : Process [%d] re-entered ready_que, due to time_quantum\n", clock, CPU_running_p->PID);
            CPU_running_p = NULL;
            left_time_quantum = full_time_quantum;
        }
    }
        // while LOOP 종료, 모든 큐 empty
        return 0;
}