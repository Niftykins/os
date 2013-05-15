typedef struct Node {
	void* data;
	struct Node* next;
} Node;

typedef struct {
	Node* head;
	Node* tail;
	int length;
} List;

List* createList();
void push(List*, void*);
Node* pop(List*);
void freeList(List*);
void freeNode(Node*);
Node* peek(List* list);
