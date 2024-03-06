#include "cvs.h"
// Funkcja tworzenia kolejki
void enqueue(mutex_t* mutex, endpoint_t process, int status) {
    if (mutex->rear == QUEUE_SIZE) {
        return;
    }
    mutex->queue[mutex->rear] = process;
    mutex->status[mutex->rear] = status;
    mutex->rear = mutex->rear + 1;
}

endpoint_t dequeue(mutex_t* mutex) {
    endpoint_t process = mutex->queue[0];
    for (int i = 0; i < mutex->rear - 1; i++) {
        mutex->queue[i] = mutex->queue[i + 1];
        mutex->status[i] = mutex->status[i + 1];
    }
    mutex->queue[mutex->rear - 1] = 0;
    mutex->status[mutex->rear - 1] = 0;
    mutex->rear = mutex->rear - 1;
    return process;
}

endpoint_t front(mutex_t* mutex) {
    return mutex->queue[0];
}

void delete_from_queue(mutex_t* mutex, endpoint_t process) {
    for (int i = 0; i < mutex->rear; i++) {
        if (mutex->queue[i] == process) {
            for (int j = i; j < mutex->rear - 1; j++) {
                mutex->queue[j] = mutex->queue[j + 1];
                mutex->status[j] = mutex->status[j + 1];
            }
            mutex->queue[mutex->rear - 1] = 0;
            mutex->status[mutex->rear - 1] = 0;
            mutex->rear = mutex->rear - 1;
            return;
        }
    }
}

