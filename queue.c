#include "os.h"
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"


Queue* createQueue(int size) {
    size++;
    Queue* q;

    q = (Queue*)malloc(sizeof(Queue));
    q->pa = (PA*)malloc(size * sizeof(PA));

    q->front = 1;
    q->rear = 1;
    q->max = size;

    printf("f: %d, r: %d, m: %d\n", q->front, q->rear, q->max);

    return q;
}

void enqueue(Queue* q, PA element) {
    if(q->rear == q->max) {
        printf("overflow\n");
    } else {
        q->pa[q->rear] = element;
        q->rear++;
    }
}
 
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

//shuffle the contents of the array
void shift(Queue* q) {
    for(int ii = 0; ii != q->rear - 1; ii++) {
        *(q->pa+ii) = *(q->pa+ii+1);
    }
}


int isEmpty(Queue* q) {
    printf("f: %d, r: %d, m: %d\n", q->front, q->rear, q->max);

    if(q->front == q->rear)
        return 1;
    else
        return 0;
}

void display(Queue* q) {
    if(q->front == q->rear)
        printf("Queue is empty\n");
    else {
        printf("|pid|state|arrive|time|times\n");
        for(int ii=q->front; ii<q->rear; ii++) {
            printf("| %02d|  %02d |  %02d  | %02d | ", q->pa[ii].pid, q->pa[ii].state, q->pa[ii].arrive, q->pa[ii].time);

            for(int jj=1; jj<=q->pa[ii].times[0]; jj++) {
                printf("%d ", q->pa[ii].times[jj]);
            }
            printf("\n");
        }
    }
}