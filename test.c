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

int priority_check(Process * p){
    return p->priority;
}

int CPUt_check(Process * p){
    return p->CPU_t;
}

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
        printf("Memory allocation failure.");
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

HEADER * Create_Que(){
    HEADER * h = (HEADER *)malloc(sizeof(HEADER));
    if (h == NULL){
        return NULL;
    }
    h->count = 0;
    h->head = NULL;
    return h;
}

void Config(HEADER ** ready_q, HEADER ** wait_q){
    * ready_q = Create_Que();
    if (*ready_q == NULL){
        printf("Memory allocation failed in ready que header allocation.");
    }
    * wait_q = Create_Que();
    if (*wait_q == NULL){
        printf("Memory allocation failed in wait que header allocation.");
    }
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


// traverse 하며 compare_nodes로 value 비교 후 위치 넣음 compare_nodes에서 RR, FCFS이면 priority 1로 통일할 것.
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

NODE * Create_Node(Process * new_p){
    NODE * newnode = (NODE *)malloc(sizeof(NODE));
    if (newnode == NULL){
        printf("failed memory allocation for node.\n");
        return NULL;
    }
    else{
        newnode->p =new_p;
        newnode->next = NULL;
        return newnode;
    }
}



int main(void){
    srand(time(NULL));
    Process * p = Create_Process(1);
    printf("%d %lf %lf %lf %d", p->PID, p->Arrival_t, p->CPU_t, p->IO_t, p->priority);
    free(p);
    return 0;
}

scheduler