#include <stdio.h>
#include <stdlib.h>
#include "simulator.h"

int pre_stop_condition(Process * running_p, Process * newly_entered_p, int check_for_ready_q(Process *p)){
    if (check_for_ready_q(running_p) > check_for_ready_q(newly_entered_p)) return 1; // CPU running process보다 우선순위 높으면(CPU burst time OR priority value 작으면) stop
    else return 0;
}

int nonpre_stop_condition(Process * running_p, Process * newly_entered_p, int check_for_ready_q(Process *p)){
    return 0;
}

int base_stop_condition(Process * running_p, int current_time_quantum){
    if (running_p == NULL) return 0;
    else if (!(running_p->CPU_t)) return 1; // CPU running process 끝나면 stop
    else if (!current_time_quantum) return 1; // current time quantum 0이면 stop  // RR이 아닌 경우에는 simulator()에서 매우 큰 값으로 설정해 무시할 조건.
    else return 0;
}

int schedule(int policy, int (** check_for_ready_q)(Process *), int (** pnp_stop_condition) (Process *, Process *, int (*) (Process *)), int * time_quantum){
    *time_quantum = RR_time_quantum_F; // RR_time_quantum 최대한도로 설정해 무시.
    switch(policy){
        case FCFS:
            *check_for_ready_q = priority_dismiss; // priority value 전부 무시, 선착순으로 
            *pnp_stop_condition = nonpre_stop_condition;
            break;
        case RR:
            *time_quantum = RR_time_quantum_T; // RR_time_quantum 실제로 설정.
            *check_for_ready_q = priority_dismiss; // priority value 전부 무시, 선착순으로
            *pnp_stop_condition = nonpre_stop_condition;
            break;
        case np_SJF:
            *check_for_ready_q = CPUt_check; // CPU 남은 burst time으로 판단 설정.
            *pnp_stop_condition = nonpre_stop_condition;
            break;
        case p_SJF:
            *check_for_ready_q = CPUt_check; // CPU 남은 burst time으로 판단 설정.
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


int simulator(HEADER * job_queue, HEADER ** ready_que, HEADER ** wait_que,  int check_for_ready_q(Process * p), int pnp_stop_condition(Process * running_p, Process * newly_entered_p, int check(Process *p)), int time_quantum){
    int clock = 0;
    int left_time_quantum = time_quantum;
    Process * next_entering_p = pop_node(job_queue);
    Process * CPU_running_p = NULL;
    Process * IO_running_p = NULL;
    Process * preemptive_temp;
    NODE * curnode;
    
    while(!(empty_Queue(*ready_que) && empty_Queue(*wait_que) && (next_entering_p == NULL))){
        // job_que에서 arrival time 되면 ready queue로 process 이동
        while ((next_entering_p != NULL) && (next_entering_p->Arrival_t == clock)){
            // preemptive일때는 next_enterin_p 들어올 때 check => CPU preemption 일어나야 한다면 그 둘의 프로세스를 교체한다.
            if ((CPU_running_p != NULL) && pnp_stop_condition(CPU_running_p, next_entering_p, check_for_ready_q)){
                preemptive_temp = CPU_running_p;
                CPU_running_p = next_entering_p;
                next_entering_p = preemptive_temp;
                left_time_quantum = time_quantum;
            }
            // next_enterin_p를 ready_que에 넣는다.
            curnode = Create_Node(next_entering_p);
            if (curnode == NULL) {
                printf("Failed memory allocation during creating node for ready queue.");
                return -1;
            }
            push_node(*ready_que, curnode, check_for_ready_q); // check는 지정 필요!! scheduler마다 다르게
            next_entering_p = pop_node(job_queue);
        }

        //TIME ELAPSE

        // CPU 작동 중 없는데, ready_que에 방금 들어왔다면 작동. + CPU 작동 중이라면 time 깎기
        if ((CPU_running_p == NULL) && (!empty_Queue(*ready_que))){
            CPU_running_p = pop_node(*ready_que);
            // new process running in CPU
            //set up time_quantum
            left_time_quantum = time_quantum;
            // decrement each counter
            CPU_running_p ->CPU_t--;
            left_time_quantum--;
        }
        else if (CPU_running_p != NULL){
            // decrement each counter
            CPU_running_p ->CPU_t--;
            left_time_quantum--;
        }

        // IO running processes to decrement time cntr

        

        // IF CPU execution terminated,
        if (base_stop_condition(CPU_running_p, left_time_quantum)){
            if (CPU_running_p->CPU_t) {
                // CPU_t left => back to ready_queue
                curnode = Create_Node(CPU_running_p);
                if (curnode == NULL) {
                    printf("Failed memory allocation during creating node for ready queue. While base stop condition.");
                    return -1;
                }
                push_node(*ready_que, curnode, check_for_ready_q);
            }
            else{
                // it has completed its CPU burst time.
                // Maybe add option to go to waiting queue?
                // Maybe print what has been terminated?
                printf("Process PID %d terminated at clock %d\n", CPU_running_p->PID, clock);
                free(CPU_running_p);
            }
            CPU_running_p = NULL;
        }
        clock++;
    }
    return 0;
}