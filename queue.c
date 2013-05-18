#include "os.h"
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

/*
    Creates a Queue of specified size and allocates memory
    for the PA array which will be used to store each of the
    process activities
*/
Queue* createQueue(int size) {
    Queue* q;

    size++;
    q = (Queue*)malloc(sizeof(Queue));
    q->pa = (PA*)malloc(size * sizeof(PA));

    q->front = 1;
    q->rear = 1;
    q->max = size;

    return q;
}

/*
    Adds a new PA to Queue q
*/
void enqueue(Queue* q, PA element) {
    if(q->rear == q->max) {
        printf("overflow\n");
    } else {
        q->pa[q->rear] = element;
        q->rear++;
    }
}
 
/*
    Removes and returns first PA in the Queue q
    Reindexes the queue so the first index is always used
*/
PA dequeue(Queue* q) {
    PA returner;
    if(q->front == q->max) {
        printf("underflow\n");
    } else {
        returner = q->pa[q->front];
        shift(q);
        q->rear--;
    }

    return returner;
}

/*
    Reindexing function for when elements get removed
*/
void shift(Queue* q) {
    for(int ii = 0; ii != q->rear - 1; ii++) {
        *(q->pa+ii) = *(q->pa+ii+1);
    }
}

/*
    Function to check if the Queue is currently empty or not
*/
int isEmpty(Queue* q) {
    if(q->front == q->rear) return 1;
    else return 0;
}

/*
    Free the memory used by the Queue
*/
void freeQueue(Queue* q) {
    free(q->pa);
    free(q);
}
