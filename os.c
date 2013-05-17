#define _XOPEN_SOURCE 600

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os.h"
#include "list.h"
#include "queue.h"
#include <unistd.h>

int CPU = 2, IO = 1;
Times Timer = {0};

Queue *IO_QUEUE, *CPU_QUEUE, *FINISHED;

pthread_mutex_t lock;
pthread_barrier_t barr, barr2;

void chomp(char* s) {
    s[strcspn(s,"\n")] = '\0';
}

void wipeLogs() {
    FILE* f;
    f = fopen("log-A", "w");
    fclose(f);
    f = fopen("log-B", "w");
    fclose(f);
}

void writeA(PA pa) {
    FILE* f;
    f = fopen("log-A", "a");
    fprintf(f, "New Process:\nPID=%d\nAC=1\nState=CPU\nArrive=0\nTime=%d\n\n", pa.pid, pa.time);
    fclose(f);
}

void writeB(PA pa, int terminated) {
    FILE* f;
    f = fopen("log-B", "a");

    switch(terminated) {
        case 0: fprintf(f, "Finishing %s Activity:\n", pa.state==CPU?"I/O":"CPU"); break;
        case 1: fprintf(f, "Process PID-%d is terminated.\n", pa.pid); break;
    }

    fprintf(f, "PID=%d\n", pa.pid);
    fprintf(f, "AC=%d\n", pa.ac);
    fprintf(f, "State=%s\n", pa.state==CPU?"CPU":"I/O");
    fprintf(f, "Arrive=%d\n", pa.arrive);
    fprintf(f, "Time=%d\n\n", pa.time);

    fclose(f);
}

int* readProcess(char* name) {
    FILE* f;
    int step, type, time, size = 0, *timer;

    f = fopen(name, "r");

    while (EOF != fscanf(f, "%*d %*d %*d")) {
        size++;
    }
    rewind(f);

    timer = (int*)malloc((size+1) * sizeof(int));
    timer[0] = size; /*store array length in 0 as its 1 based anyway*/

    while(!feof(f)) {
        fscanf(f, "%d %d %d", &step, &type, &time);
        timer[step] = time;
    }

    fclose(f);

    return timer;
}

List* readJobs(char* n) {
    FILE* f;
    char line[20];
    List* jobs;

    jobs = createList();

    f = fopen(n ? n : "job", "r");

    if(f == NULL)
        printf("Probably no file by that name\n");

    while(fgets(line, 20, f) != NULL) {
        chomp(line);
        push(jobs, line);
    }

    fclose(f);

    return jobs;
}

PA getPA(Queue* queue) {
    pthread_mutex_lock(&lock);
    PA temp = dequeue(queue);
    pthread_mutex_unlock(&lock);

    return temp;
}

void *Cpu(void* zz) {
    PA* current = NULL;
    PA actual;

    while(FINISHED->rear != FINISHED->max) {
        Timer.cpu++;
        if(isEmpty(CPU_QUEUE) && current == NULL) { //just wait instantly
            Timer.cpu_idle++;
            //printf("\t> cpu is empty\n");
            pthread_barrier_wait(&barr2);
        } else { //do stuff to current PA
            pthread_barrier_wait(&barr2);

            if(current == NULL && !isEmpty(CPU_QUEUE)) {
                actual = getPA(CPU_QUEUE);  
                current = &actual;

                //printf("New CPU pid_%d start: %d, arrive: %d\n", current->pid, Timer.clock, current->arrive);
            }

            if(current != NULL && current->time != 0) {
                current->time--;
            }

            if(current != NULL && current->time == 0) {
                //printf("Finished CPU pid_%d fin: %d\n", current->pid, Timer.clock+1);
                if(current->ac == current->times[0]) { //no more activities
                    enqueue(FINISHED, actual);
                    writeB(actual, 1);
                    //printf("\t\tPID_%d HAS FINISHED AT %d\n", current->pid, Timer.clock+1);
                    //printf("\t\tc: %d i: %d\n", current->wait_cpu, current->wait_io);
                    fprintf(stderr, ".");
                    current = NULL;
                } else {
                    current->state = IO;
                    current->wait_cpu += (Timer.clock+1-current->arrive-current->times[current->ac]);
                    current->arrive = Timer.clock+1;
                    current->ac++;
                    current->time = current->times[current->ac];
                    writeB(actual, 0);
                    pthread_mutex_lock(&lock);
                    enqueue(IO_QUEUE, actual);
                    pthread_mutex_unlock(&lock);
                    current = NULL;
                }
            }
        }
        pthread_barrier_wait(&barr);
    }

    return NULL;
}

void *Io(void* zz) {
    PA* current = NULL;
    PA actual;

    while(FINISHED->rear != FINISHED->max) {
        Timer.io++;
        if(isEmpty(IO_QUEUE) && current == NULL) { //just wait instantly
            Timer.io_idle++;
            //printf("\t> io is empty\n");
            pthread_barrier_wait(&barr2);
        } else { //do stuff to current PA
            pthread_barrier_wait(&barr2);

            if(current == NULL && !isEmpty(IO_QUEUE)) {
                actual = getPA(IO_QUEUE);  
                current = &actual;

                //printf("New IO pid_%d start: %d arrive: %d\n", current->pid, Timer.clock, current->arrive);
            }
        
            if(current != NULL && current->time != 0) {
                current->time--;
            }

            if(current != NULL && current->time == 0) { //io activity has finished, push to cpu
                //printf("Finished IO pid_%d fin: %d\n", current->pid, Timer.clock+1);
                current->state = CPU;
                current->wait_io += (Timer.clock+1-current->arrive-current->times[current->ac]);
                current->arrive = Timer.clock+1;
                current->ac++;
                current->time = current->times[current->ac];
                writeB(actual, 0);
                pthread_mutex_lock(&lock);
                enqueue(CPU_QUEUE, actual);
                pthread_mutex_unlock(&lock);
                current = NULL;
            }
        }
        pthread_barrier_wait(&barr);
    }

    return NULL;
}

void *Clock(void* zz) {
    pthread_t cpu_thread, io_thread;
    
    fprintf(stderr, "Simulating ");
    
    pthread_create( &cpu_thread, NULL, Cpu, NULL);
    pthread_create( &io_thread, NULL, Io, NULL);

    while(FINISHED->rear != FINISHED->max) {
        usleep(010000);
        Timer.clock++;

        //printf("\n\tClock is now: %d\n", CLOCK);

        pthread_barrier_wait(&barr);
    }

    printf("\n");

    pthread_join(cpu_thread, NULL);
    pthread_join(io_thread, NULL);

    return NULL;
}

int main(int argc, char* argv[]) {
    List* jobs; Node* job;
    int number_of_jobs, wait_io = 0, wait_cpu = 0;
    PA temp; 
    pthread_t clock_thread;

    wipeLogs();
    
    jobs = readJobs(argv[1]);
    number_of_jobs = jobs->length;

    CPU_QUEUE = createQueue(number_of_jobs);
    IO_QUEUE = createQueue(number_of_jobs);
    FINISHED = createQueue(number_of_jobs);

    job = peek(jobs);
    for(int ii=1; ii<=number_of_jobs; ii++) {
        sscanf(job->name, "PID_%d", &temp.pid); //get the PID from file name
        temp.ac = 1;
        temp.state = CPU;
        temp.arrive = 0;
        temp.times = readProcess(job->name);
        temp.time = temp.times[1];
        temp.length = temp.times[0];
        temp.wait_io = 0;
        temp.wait_cpu = 0;

        enqueue(CPU_QUEUE, temp);
        writeA(temp);

        if(job->next != NULL) job = job->next;
    }

    freeNode(job);

    pthread_barrier_init(&barr, NULL, 3); //barrier all must reach
    pthread_barrier_init(&barr2, NULL, 2); //barrier to half io/cpu execution
    pthread_mutex_init(&lock, NULL);

    pthread_create( &clock_thread, NULL, Clock, NULL);
    pthread_join(clock_thread, NULL);

    for(int ii=0; ii<number_of_jobs; ii++) {
        temp = dequeue(FINISHED);
        wait_cpu += temp.wait_cpu;
        wait_io += temp.wait_io;
    }

    //io keeps going during last cpu burst - trim end slightly
    Timer.io -= temp.times[temp.times[0]];
    Timer.io_idle -= temp.times[temp.times[0]];

    printf("Average waiting time in CPU queue: %.2f\n", (float)wait_cpu/(float)number_of_jobs);
    printf("Average waiting time in IO queue: %.2f\n", (float)wait_io/(float)number_of_jobs);
    printf("CPU utilization: %.2f%%\n", (float)(Timer.cpu-Timer.cpu_idle)/(float)Timer.cpu*100);
    printf("IO utilization: %.2f%%\n", (float)(Timer.io-Timer.io_idle)/(float)Timer.io*100);

    return 1;
}
