typedef struct {
	PA* PCB;
	int length;
	int max;
	int front;
	int rear;
} Queue;

PA* createQueue(int);
void enqueue(PA*, PA); 
PA dequeue(PA*);
void shift(PA*);
void display(PA*);
int isEmpty(PA*);
