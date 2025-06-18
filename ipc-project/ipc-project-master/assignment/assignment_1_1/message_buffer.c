#include "message_buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int shmid;
void *memory_segment=NULL;

int init_buffer(MessageBuffer **buffer) {
    if ((shmid = shmget(KEY, sizeof(MessageBuffer), IPC_CREAT|IPC_EXCL|0666)) == -1){
        return -1;
    }
    if ((memory_segment = shmat(shmid, NULL, 0)) == (void*)-1){
        return -1;
    }
    *buffer = (MessageBuffer*)memory_segment;
    (*buffer)->is_empty = 0;
    printf("init buffer\n");
    return 0;
}

int attach_buffer(MessageBuffer **buffer) {
    if ((shmid = shmget(KEY, sizeof(MessageBuffer), 0666)) == -1) return -1;
    if ((memory_segment = shmat(shmid, NULL, 0)) == (void *) - 1) return -1;
    *buffer = (MessageBuffer *)memory_segment;
    printf("attach buffer\n");
    printf("\n");
    return 0;
}

int detach_buffer() {
    if (shmdt(memory_segment) == -1) {
        printf("shmdt error!\n\n");
        return -1;
    }

    printf("detach buffer\n\n");
    return 0;
}

int destroy_buffer() {
    if(shmctl(shmid, IPC_RMID, NULL) == -1) {
        printf("shmctl error!\n\n");
        return -1;
    }

    printf("destroy shared_memory\n\n");
    return 0;
}

int produce(MessageBuffer **buffer, int sender_id, int data, int account_id) {
    Message new;
    new.sender_id = sender_id;
    new.data = data;
    (*buffer)->account_id = account_id;

    if ((*buffer)->is_empty == BUFFER_SIZE) return -1;
    (*buffer)->messages[(*buffer)->is_empty] = new;
    (*buffer)->is_empty++;
    printf("produce message\n");

    return 0;
}

int consume(MessageBuffer **buffer, Message **message) {
    if ((*buffer)->is_empty == 0) return -1;

    *message = (Message *)(&((*buffer)->messages[(*buffer)->is_empty-1]));

    (*buffer)->is_empty--;
    return 0;
}
