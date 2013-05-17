typedef struct PA {
	int pid;
	int ac;
	int state;
	int arrive;
	int time;
	int* times;
	int length;
	int wait_io;
	int wait_cpu;
} PA;

typedef struct Times {
	int clock;
	int cpu_idle;
	int cpu;
	int io_idle;
	int io;
} Times;
