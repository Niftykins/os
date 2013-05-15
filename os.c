#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os.h"
#include "list.h"
#include "queue.h"
#include <pthread.h>

Queue* CPU_QUEUE;
Queue* IO_QUEUE;
int CPU = 2, IO = 1;
int CLOCK = 0;

void chomp(char* s) {
	s[strcspn(s,"\n")] = '\0';
}

int* readProcess(char* name) {
	FILE* f;
	int step, type, time;
	int size = 0;
	int* timer = NULL;

	f = fopen(name, "r");

	if(f == NULL)
		perror("Error opening file\n");
	else {

		while (EOF != fscanf(f, "%*d %*d %*d")) {
			size++;
		}
		rewind(f);

		timer = (int*)malloc(size * sizeof(int));
		timer[0] = size; //store array length in 0 as its 1 based anyway

		while(!feof(f)) {
			fscanf(f, "%d %d %d", &step, &type, &time);
			timer[step] = time;
		}

		if(ferror(f))
			perror("Error reading from file\n");

		fclose(f);
	}

	return timer;
}

/*
	Reads the file 'job' for the list of process files and
	adds each entry into the linked list 'jobs' which was
	passed to in.
*/
List* readJobs() {
	FILE* f;
	char* line = NULL;
	size_t n = 0;
	List* jobs;

	jobs = createList();

	f = fopen("job", "r");

	if(f == NULL)
		perror("Error opening file\n");
	else {
		while(getline(&line, &n, f) != -1) {
			chomp(line);
			push(jobs, line);
		}
	
		if(ferror(f))
			perror("Error reading from file\n");

		fclose(f);
	}

	return jobs;
}

void *cpu(void) {
	printf("weow cpu\n");
	if(isEmpty(IO_QUEUE))
		printf("io empty\n");
	else
		printf("io not empty\n");

	return NULL;
}

void *io(void) {
	printf("weow io\n");

	return NULL;
}

int main(void) {
	List* jobs;
	Node* job;
	Queue* pa;
	PA temp;
	pthread_t cpu_thread, io_thread;
	int number_of_jobs;

	jobs = readJobs();
	number_of_jobs = jobs->length;

	pa = createQueue(number_of_jobs);
	IO_QUEUE = createQueue(number_of_jobs);

	printf("io:  ");
	isEmpty(IO_QUEUE);
	printf("cpu:  ");
	isEmpty(pa);

	job = peek(jobs);
	for(int ii=1; ii<=number_of_jobs; ii++) {
		sscanf(job->name, "PID_%d", &temp.pid); //get the PID from file name
		temp.ac = 1;
		temp.state = CPU;
		temp.arrive = 0;
		temp.times = readProcess(job->name);
		temp.time = temp.times[1];
		temp.length = temp.times[0];
		temp.waiting = 0;

		enqueue(pa, temp);
		//printf("rear: %d | %d\n", pa->rear, IO_QUEUE->rear);

		if(job->next != NULL)
			job = job->next;
	}

	freeNode(job);

	CPU_QUEUE = pa;
	display(CPU_QUEUE);

	for(int ii=1, length = CPU_QUEUE->rear; ii<length; ii++) {
		dequeue(CPU_QUEUE);
		display(CPU_QUEUE);
		printf("%d\n", CPU_QUEUE->rear-1);
	}


	printf("io:  ");
	isEmpty(IO_QUEUE);
	printf("cpu: ");
	isEmpty(CPU_QUEUE);

	enqueue(IO_QUEUE, temp);
	display(IO_QUEUE);

	//printf("\ntesting io queue\n");
	//enqueue(IO_QUEUE, temp);
	//display(IO_QUEUE);


//	enqueue(IO_QUEUE, temp);
//	display(IO_QUEUE);

/*	int error[2];
	error[0] = pthread_create( &cpu_thread, NULL, cpu, NULL);
	error[1] = pthread_create( &io_thread, NULL, io, NULL);

	pthread_join(cpu_thread, NULL);
	pthread_join(cpu_thread, NULL);

*/
	//printf("%d\n", isEmpty(IO_QUEUE));

	printf("le fin\n");

	return 1;
}
