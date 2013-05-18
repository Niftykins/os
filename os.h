typedef struct PA {
	int pid; //process id
	int ac; //activity counter
	int state; //CPU 2 or IO 1
	int arrive; //arrival time
	int time; //time of current activity
	int* times; //array of times
	int length; //length of array
	int wait_io; //how long its been waiting in io
	int wait_cpu; //waiting in cpu
} PA;

typedef struct Times {
	int clock; //global clock cycles
	int cpu_idle; //cpu idle cycles
	int cpu; //total cpu cycles
	int io_idle; //io idle cycles
	int io; //total io cycles
} Times;
