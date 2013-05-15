typedef struct {
	PA* pa;
	int length;
	int max;
	int front;
	int rear;
} Queue;

Queue* createQueue(int);
void enqueue(Queue*, PA); 
PA dequeue(Queue*);
void shift(Queue*);
void display(Queue*);
int isEmpty(Queue*);
