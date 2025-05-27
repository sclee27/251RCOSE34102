#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

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
    return ((queue == NULL) || (queue->count == 0));
}