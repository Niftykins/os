#include "os.h"
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

int front = 1,rear = 1, max;

PA* createQueue(int size) {
    size++;
    PA* queue;
    max = size;

    printf("f: %d, r: %d, m: %d\n", front, rear, max);
    queue = (PA*)malloc((size) * sizeof(PA));

    return queue;
}

void enqueue(PA* queue, PA element) {
    if(rear == max) {
        printf("overflow\n");
    } else {
        queue[rear] = element;
        rear++;
    }
}
 
PA dequeue(PA* queue) {
    PA returner;
    if(rear == front) {
        printf("underflow\n");
    } else {
        returner = queue[front];
        shift(queue);
        rear--;
    }

    return returner;
}

//shuffle the contents of the array
void shift(PA* queue) {
    for(int ii = 0; ii != rear - 1; ii++) {
        *(queue+ii) = *(queue+ii+1);
    }
}


int isEmpty(PA* queue) {
    printf("f: %d, r: %d, m: %d\n", front, rear, max);

    if(front == rear)
        return 1;
    else
        return 0;
}

void display(PA* queue) {
    if(front == rear)
        printf("Queue is empty\n");
    else {
        printf("| pid| arr|time|times\n");
        for(int ii=front; ii<rear; ii++) {
            printf("| %02d | %02d | %02d | ", queue[ii].pid, queue[ii].arrive, queue[ii].time);

            for(int jj=1; jj<=queue[ii].times[0]; jj++) {
                printf("%d ", queue[ii].times[jj]);
            }
            printf("\n");
        }
    }
}