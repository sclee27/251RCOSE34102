#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct process{
    int PID;
    double Arrival_t;
    double CPU_t;
    double IO_t;
    int priority;
}Process;

typedef struct process_node{
    Process * p;
    struct process_node * next;
}NODE;


Process * Create_Process(){
    Process* p = (Process *) malloc(sizeof(Process));
    if (p == NULL){
        printf("Cannot create new process\n");
        return NULL;
    }
    p->PID = rand()%1000;
    p->Arrival_t = (rand()%1000) / 100.0;
    p->CPU_t = (rand()%1000) / 100.0;
    p->IO_t = (rand()%1000) / 100.0;
    p->priority = rand()%20;
    return p;
}

void Config(){
    NODE * ready_q = (NODE *)malloc(sizeof(NODE));
    ready_q->next = NULL;
    NODE * wait_q = (NODE *)malloc(sizeof(NODE));
    wait_q->next = NULL;
    return ready_q, wait_q;
}

int main(void){
    srand(10);
    Process * p = Create_Process();
    printf("%d %lf %lf %lf %d", p->PID, p->Arrival_t, p->CPU_t, p->IO_t, p->priority);
    free(p);
    return 0;
}