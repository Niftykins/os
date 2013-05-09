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
Node* pop(List*);
void freeList(List*);
void freeNode(Node*);
void printList(List*);
Node* peek(List* list);
