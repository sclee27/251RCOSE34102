#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define Arrival_time_Range 20
#define CPU_time_Range 20
#define IO_time_Range 20
#define Priority_Range 20

#define FCFS 1    // Priority check을 check함수로 + all priority set to 1
#define np_SJF 2  // CPU time check을 check함수로 
#define np_Pri 3  // Priority check을 check함수로
#define RR 4      // Priority check을 check함수로 + all priority set to 1 + Timer()
#define p_SJF 5   // CPU time check을 check함수로 + check When ready que pushed
#define p_Pri 6   // Priority check을 check함수로 + check When ready que pushed

// what to do with wait que????!!

typedef struct process{
    int PID;
    int Arrival_t;
    int CPU_t;
    int IO_t;
    int priority;
}Process;

typedef struct process_node{
    Process * p;
    struct process_node * next;
}NODE;

typedef struct header{
    int count;
    struct process_node * head;
}HEADER;

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


NODE * Create_Node(Process * new_p){
    NODE * newnode = (NODE *)malloc(sizeof(NODE));
    if (newnode == NULL){
        printf("failed memory allocation during Node Creation.\n");
        return NULL;
    }
    else{
        newnode->p =new_p;
        newnode->next = NULL;
        return newnode;
    }
}

HEADER * Create_Que(){
    HEADER * h = (HEADER *)malloc(sizeof(HEADER));
    if (h == NULL){
        return NULL;
    }
    h->count = 0;
    h->head = NULL;
    return h;
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

// 2개의 프로세스 비교, new_process가 old_process보다 우선순위가 높으면 1, 낮으면 -1, 같으면 0.
int compare_nodes(Process * new_process, Process * old_process, int check(Process * p)){
    int result = check(new_process) - check(old_process);
    if (result<0){
        //new_process의 CPU time 또는 priority value 더 작음 (우선순위 더 높다.)
        return -1;
    }
    else if (result > 0){
        //new_process의 CPU time 또는 priority value 더 큼 (우선순위 더 낮다.)
        return 1;
    }
    else{
        return 0;
    }
}


// traverse 하며 compare_nodes로 value 비교 후 위치 넣음 compare_nodes에서.
void push_node(HEADER * header, NODE * newnode, int check(Process * p)){
    if (!(header->count)){
        header->head = newnode;
    }
    else{
        NODE * curnode = header->head;
        NODE * prevnode = NULL;
        while((curnode != NULL) && (compare_nodes(newnode->p, curnode->p, check)>=0)){
            prevnode = curnode;
            curnode = curnode->next;
        }
        if (prevnode == NULL){
            header->head = newnode;
            newnode->next = curnode;
        }
        else {
            prevnode->next = newnode;
            newnode->next = curnode;
        }
    }
    header->count++;
}

Process * pop_node(HEADER * header){
    if (!(header->count)){
        printf("The que is empty\n");
        return NULL;
    }
    else{
        NODE * delnode = header->head;
        Process * popped_process = delnode->p;
        header->head = header->head->next;
        header->count--;
        free(delnode);
        return popped_process;
    }
}


void del_Queue(HEADER * queue){
    NODE * cur_n = queue->head;
    NODE * del_n;
    while(cur_n != NULL){
        free(cur_n->p);
        del_n = cur_n;
        cur_n = cur_n->next;
        free(del_n);
    }
    free(queue);
}

int empty_Queue(HEADER * queue){
    return (queue->count == 0);
}

void Config(HEADER ** job_q, HEADER ** ready_q, HEADER ** wait_q, int n_processes){
    Process * cur_p;
    NODE * cur_n;
    // job_que에 프로세스 생성 후 arrival time 순으로 sort
    * job_q = Create_Que();
    if (*job_q == NULL){
        printf("Memory allocation failed in ready que header allocation.");
    }
    for (int i =0; i<n_processes;i++){
        cur_p = Create_Process(i+1);
        if (cur_p == NULL) return -1;
        cur_n = Create_Node(cur_p);
        if (cur_n == NULL) return -1;
        push_node(*job_q, cur_n, Arrival_t_check);
    }
    // ready_que 생성
    * ready_q = Create_Que();
    if (*ready_q == NULL){
        printf("Memory allocation failed in ready que header allocation.");
    }
    // wait_que 생성
    * wait_q = Create_Que();
    if (*wait_q == NULL){
        printf("Memory allocation failed in wait que header allocation.");
    }

}

int pre_stop_condition(Process * running_p, Process * newly_entered_p, int check_for_ready_q(Process *p)){
    if (check_for_ready_q(running_p) > check_for_ready_q(newly_entered_p)) return 1; // CPU running process보다 우선순위 높으면(CPU burst time OR priority value 작으면) stop
    else return 0;
}

int nonpre_stop_condition(Process * running_p, Process * newly_entered_p, int check_for_ready_q(Process *p)){
    return 0;
}

int base_stop_condition(Process * running_p, int current_time_quantum){
    if (!(running_p->CPU_t)) return 1; // CPU running process 끝나면 stop
    else if (!current_time_quantum) return 1; // current time quantum 0이면 stop  // RR이 아닌 경우에는 simulator()에서 매우 큰 값으로 설정해 무시할 조건.
    else return 0;
}



int simulator(HEADER * job_queue, HEADER ** ready_que, HEADER ** wait_que,  int check_for_ready_q(Process * p), int preemptive_stop_condition(Process * running_p, Process * newly_entered_p, int check_for_ready_q(Process *p)), int time_quantum){
    int clock = 0;
    int left_time_quantum = time_quantum;
    Process * next_entering_p = pop_node(job_queue);
    Process * CPU_running_p = NULL;
    Process * IO_running_p = NULL;
    Process * preemptive_temp;
    NODE * curnode;
    
    while(!(empty_Queue(*ready_que) && empty_Queue(*wait_que) && (next_entering_p == NULL))){
        // job_que에서 arrival time 되면 ready queue로 process 이동
        if ((next_entering_p != NULL) && (next_entering_p->Arrival_t == clock)){
            // preemptive일때는 next_enterin_p 들어올 때 check => CPU preemption 일어나야 한다면 그 둘의 프로세스를 교체한다.
            if (preemptive_stop_condition(CPU_running_p, next_entering_p, check_for_ready_q) && (CPU_running_p != NULL)){
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
        if (base_stop_condition(CPU_running_p)){
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

//printf("%d %lf %lf %lf %d", p->PID, p->Arrival_t, p->CPU_t, p->IO_t, p->priority);



void schedule(int policy){
    switch(policy){
        case FCFS:
            int RR_time_limit = CPU_time_Range+2;

        case RR:
            check = priority_check;
            // all priority set to 1
            int RR_time_limit = 
            // set timer
            break;
    
        case np_SJF:
        case p_SJF:
            check = CPUt_check;
            //check when ready q pushed
            break;

        case np_Pri:
        case p_Pri:
            check = priority_check;
            //check when ready q pushed
            break;

    }
}


int main(int argc, char ** argv){
    // input 확인, 처리
    if (argc != 3){
        printf("Program_name\tNumber_of_processes\tScheduler\n%d\tFCFS\n%d\tnon-preemptive SJF\n%d\tnon-preemptive Priority\n%d\tRound Robin\n%d\tpreemptive SJF\n%d\tpreemptive Priority\n", FCFS, np_SJF, np_Pri, RR, p_SJF, p_Pri);
        printf("rerun the program.");
        return -1;
    }
    int n_processes = atoi(argv[1]);
    int scheduler = atoi(argv[2]);
    if ((n_processes < 1)||(scheduler <1)||(scheduler > 6)){
        printf("Wrong input numbers. n_processes has to be positive integer, scheduler should be 1 to 6.\n");
        return -1;
    }

    // 시뮬레이터 설정, 프로세스 생성, job_queue 정비
    srand(time(NULL));
    HEADER * job_queue, * ready_queue, *wait_queue;
    // process 생성 완료, job_queue 정렬 완료(Arrival_time 순서로)
    Config(&job_queue, &ready_queue, &wait_queue, n_processes);
    //시뮬레이션 가동
    //simulator(job_queue, scheduler);
    //free()

    return 0;
}