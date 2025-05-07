#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"


typedef struct process_node{
    Process * p;
    struct process_node * next;
}NODE;

typedef struct header{
    int count;
    struct process_node * head;
}HEADER;


NODE * Create_Node(Process * new_p);

HEADER * Create_Que();

// 2개의 프로세스 비교, new_process가 old_process보다 우선순위가 높으면 1, 낮으면 -1, 같으면 0.
int compare_nodes(Process * new_process, Process * old_process, int check(Process * p));

// traverse 하며 compare_nodes로 value 비교 후 value 오름차순으로 정렬 삽입. (value 같을 때는 후순서로 들어간다.)
void push_node(HEADER * header, NODE * newnode, int check(Process * p));

Process * pop_node(HEADER * header);

void del_Queue(HEADER * queue);

int empty_Queue(HEADER * queue);

#endif