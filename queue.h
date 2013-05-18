typedef struct {
	PA* pa; //array of PA structs
	int max; //maximum length of the queue
	int front; //index of first element
	int rear; //index of last used element
} Queue;

Queue* createQueue(int);
void enqueue(Queue*, PA); 
PA dequeue(Queue*);
void shift(Queue*);
int isEmpty(Queue*);
void freeQueue(Queue*);
