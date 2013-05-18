typedef struct Node {
	char* name;
	struct Node* next;
} Node;

typedef struct {
	Node* head;
	Node* tail;
	int length;
} List;

List* createList();
void push(List*, char*);
void freeList(List*);
Node* peek(List* list);
