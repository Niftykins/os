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

/*
    Remove the new line (\n) character from the end of the line
    and replace it with the terminating character
*/
void chomp(char* s) {
    s[strcspn(s,"\n")] = '\0';
}

/*
    Open both files and wipe them on run, to ensure clean log at the end
*/
void wipeLogs() {
    FILE* f;
    f = fopen("log-A", "w");
    fclose(f);
    f = fopen("log-B", "w");
    fclose(f);
}

/*
    Write each new process to log-A
*/
void writeA(PA pa) {
    FILE* f;
    f = fopen("log-A", "a");
    fprintf(f, "New Process:\nPID=%d\nAC=1\nState=CPU\nArrive=0\nTime=%d\n\n", pa.pid, pa.time);
    fclose(f);
}

/*
    Write queue switches of each PA to the log-B file
*/
void writeB(PA pa, int terminated) {
    FILE* f;
    f = fopen("log-B", "a");

    switch(terminated) { //print different header based on if its terminated or not
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

/*
    Do the calcuations for % and waiting and print/write to log-B
*/
void writeFinal(int cpu, int io, int num) {
    FILE* f;
    float wait_cpu, wait_io, util_cpu, util_io;

    wait_cpu = (float)cpu / (float)num;
    wait_io = (float)io / (float)num;
    util_cpu = (float)(Timer.cpu - Timer.cpu_idle) / (float)Timer.cpu * 100;
    util_io = (float)(Timer.io - Timer.io_idle) / (float)Timer.io * 100;

    printf("Average waiting time in CPU queue: %.2f\n", wait_cpu);
    printf("Average waiting time in IO queue: %.2f\n", wait_io);
    printf("CPU utilization: %.2f%%\n", util_cpu);
    printf("IO utilization: %.2f%%\n", util_io);

    f = fopen("log-B", "a"); //append

    fprintf(f, "Average waiting time in CPU queue: %.2f\n", wait_cpu);
    fprintf(f, "Average waiting time in IO queue: %.2f\n", wait_io);
    fprintf(f, "CPU utilization: %.2f%%\n", util_cpu);
    fprintf(f, "IO utilization: %.2f%%\n", util_io);

    fclose(f);
}

/*
    Reads the process file
    only %d %d %d format works
*/
int* readProcess(char* name) {
    FILE* f;
    int step, type, time, size = 0, *timer;

    f = fopen(name, "r");

    while (EOF != fscanf(f, "%*d %*d %*d")) {
        size++; //count the number of lines in the file
    }
    rewind(f); //back to top of file

    timer = (int*)malloc((size+1) * sizeof(int));
    timer[0] = size; /*store array length in 0 as its 1 based anyway*/

    while(!feof(f)) {
        fscanf(f, "%d %d %d", &step, &type, &time);
        timer[step] = time;
    }

    fclose(f);
    return timer;
}

/*
    Read the jobs file and create a list of job filenames
*/
List* readJobs(char* n) {
    FILE* f;
    char line[20]; //ample space for file name length
    List* jobs;

    jobs = createList(); //create a new list for job names

    f = fopen(n ? n : "job", "r"); //if null, open "job"

    if(f == NULL) //file not found
        printf("Probably no file by the name: %s\n", n);
    else
        printf("Reading jobs file: %s\n", n?n:"job");

    while(fgets(line, 20, f) != NULL) {
        chomp(line); //remove \n
        push(jobs, line);
    }

    fclose(f);
    return jobs;
}

/*
    Get and return a PA from a specified queue
*/
PA getPA(Queue* queue) {
    pthread_mutex_lock(&lock);
    PA temp = dequeue(queue);
    pthread_mutex_unlock(&lock);
    return temp;
}

// CPU THREAD
void *Cpu(void* zz) {
    PA* current = NULL;
    PA actual;

    while(FINISHED->rear != FINISHED->max) {
        Timer.cpu++;
        if(isEmpty(CPU_QUEUE) && current == NULL) { //just wait instantly
            Timer.cpu_idle++;
            pthread_barrier_wait(&barr2);
        } else { //do stuff to current PA
            pthread_barrier_wait(&barr2);

            if(current == NULL && !isEmpty(CPU_QUEUE)) {
                actual = getPA(CPU_QUEUE);  
                current = &actual;
            }

            if(current != NULL && current->time != 0) {
                current->time--;
            }

            if(current != NULL && current->time == 0) {
                if(current->ac == current->times[0]) { //no more activities
                    enqueue(FINISHED, actual);
                    writeB(actual, 1);
                    fprintf(stderr, ".");
                    current = NULL;
                } else { //finished with activities remaining
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

//IO THREAD
void *Io(void* zz) {
    PA* current = NULL;
    PA actual;

    while(FINISHED->rear != FINISHED->max) {
        Timer.io++;
        if(isEmpty(IO_QUEUE) && current == NULL) { //just wait instantly
            Timer.io_idle++;
            pthread_barrier_wait(&barr2);
        } else { //do stuff to current PA
            pthread_barrier_wait(&barr2);

            if(current == NULL && !isEmpty(IO_QUEUE)) {
                actual = getPA(IO_QUEUE); //working on nothing, pull one from queue 
                current = &actual;
            }
        
            if(current != NULL && current->time != 0) {
                current->time--; //decrement remaining activity time
            }

            if(current != NULL && current->time == 0) { //io activity has finished, push to cpu
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
        pthread_barrier_wait(&barr); //half on each cycle
    }

    return NULL;
}

//CLOCK THREAD
void *Clock(void* zz) {
    pthread_t cpu_thread, io_thread;
    
    fprintf(stderr, "\nSimulating ");
    
    pthread_create( &cpu_thread, NULL, Cpu, NULL);
    pthread_create( &io_thread, NULL, Io, NULL);

    while(FINISHED->rear != FINISHED->max) {
        usleep(010000);
        Timer.clock++;
        pthread_barrier_wait(&barr); //halt on each cycle
    }
    printf("\n");

    pthread_join(cpu_thread, NULL);
    pthread_join(io_thread, NULL);

    return NULL;
}

int main(int argc, char* argv[]) {
    List* jobs; Node* job;
    int number_of_jobs, wait_io = 0, wait_cpu = 0, final;
    PA temp = {0}; //init to 0
    pthread_t clock_thread;

    wipeLogs();
    
    jobs = readJobs(argv[1]);
    number_of_jobs = jobs->length;

    CPU_QUEUE = createQueue(number_of_jobs); //create all three queues
    IO_QUEUE = createQueue(number_of_jobs);
    FINISHED = createQueue(number_of_jobs);

    job = peek(jobs);
    for(int ii=1; ii<=number_of_jobs; ii++) {
        sscanf(job->name, "PID_%d", &temp.pid); //get the PID from file name
        temp.ac = 1;
        temp.state = CPU;
        temp.arrive = 0;
        temp.times = readProcess(job->name); //read and assign time values
        temp.time = temp.times[1];
        temp.length = temp.times[0];
        temp.wait_io = 0;
        temp.wait_cpu = 0;

        printf("New process: PID_%d\n", temp.pid);

        enqueue(CPU_QUEUE, temp);
        writeA(temp);

        if(job->next != NULL) job = job->next;
    }

    freeList(jobs);

    pthread_barrier_init(&barr, NULL, 3); //synchronization barrier
    pthread_barrier_init(&barr2, NULL, 2); //barrier to halt io/cpu execution
    pthread_mutex_init(&lock, NULL); //mutex to lock queues during operations

    pthread_create(&clock_thread, NULL, Clock, NULL); //spawn clock thread
    pthread_join(clock_thread, NULL); //wait until clock is done

    //calculate the total waiting time for cpu and io
    for(int ii=0; ii<number_of_jobs; ii++) {
        temp = dequeue(FINISHED);
        wait_cpu += temp.wait_cpu;
        wait_io += temp.wait_io;
        final = temp.times[temp.times[0]];
        free(temp.times); //free the allocated times arrays
    }

    Timer.io -= final; //io thread keeps running while the final cpu process
    Timer.io_idle -= final; //is being done - so deduct that time for calc

    writeFinal(wait_cpu, wait_io, number_of_jobs);

    freeQueue(FINISHED); //free all the queues
    freeQueue(CPU_QUEUE);
    freeQueue(IO_QUEUE);

    return 1;
}
