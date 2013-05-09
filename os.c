#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os.h"
#include "list.h"
#include "queue.h"

PA* CPU_QUEUE;
PA* IO_QUEUE;
int CPU = 2, IO = 1;

void chomp(char* s) {
	s[strcspn(s,"\n")] = '\0';
}

int cpu() {
	/*	take one PA from queue
		pretend execute for time of CPU needed
		if terminates, print to logB
		if needs io, update PA -> ac++, state=1, arrive+=timeInCPU, time=timeInNewAc, then queue into IO
		print updated PA to logA
		*/

	return 1;
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
			//printf("%d: %d\n",step, timer[step]);
		}
		//printf("\n");

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


int main(void) {
	List* jobs;
	Node* job;
	PA* pa;
	PA temp;

	jobs = readJobs();

	pa = createQueue(jobs->length);
	//pa = (PA*)malloc((jobs->length) * sizeof(PA));
	IO_QUEUE = (PA*)malloc((jobs->length) * sizeof(PA));

	job = peek(jobs);
	for(int ii=1; ii<=jobs->length; ii++) {
		printf("\n%s\n", job->name);

		sscanf(job->name, "PID_%d", &temp.pid);
		temp.ac = 1;
		temp.state = CPU;
		temp.arrive = 0;
		temp.times = readProcess(job->name);
		temp.time = temp.times[1];
		temp.length = temp.times[0];
		temp.waiting = 0;

		printf("PA: %d %d %d %d %d\n", temp.pid, temp.ac, temp.state, temp.arrive, temp.time);
		printf("%d\n", temp.length);
		for(int jj=1; jj<=temp.times[0]; jj++) {
			printf("%d: %d\n", jj, temp.times[jj]);
		}

		enqueue(pa, temp);

		if(job->next != NULL)
			job = job->next;
	}

	freeNode(job);

	CPU_QUEUE = pa;
	display(CPU_QUEUE);

	return 1;
}
