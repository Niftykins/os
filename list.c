#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <string.h>

int FILE_LENGTH = 10;

List* createList() {
	List* list;

	list = (List*)malloc(sizeof(List));
	list->head = NULL;
	list->tail = NULL;
	list->length = 0;

	return list;
}

void push(List* list, void* data) {
	Node* newNode;

	printf("<%s\n", (char*)data);

	newNode = (Node*)malloc(sizeof(Node));
	newNode->data = malloc(FILE_LENGTH * sizeof(Node));

	memcpy(newNode->data, data, sizeof(Node));
	newNode->next = NULL;
	list->length++;

	if(list->head == NULL) { /* if empty */
		list->head = newNode;
		list->tail = newNode;
	} else {
		list->tail->next = newNode;
		list->tail = list->tail->next;
	}
}

Node* pop(List* list) {
	Node* temp;

	temp = list->head;
	if(list->head->next == list->tail->next) { /* if one element */
		list->head = NULL; /* wasn't removing last element without */
		list->tail = NULL; /* this if statement */
		list->length--;
	} else {	
		list->head = list->head->next;
		list->length--;
	
		if(list->head == NULL) /* if empty */
			temp = NULL;
		}
	return temp;
}

void freeList(List* list) {
	Node* node, *nextNode;

	node = list->head;

	while (node != NULL) {
		nextNode = node->next;
		free(node->data);
		free(node);
		node = nextNode;
	}

	free(list);
}

void freeNode(Node* node) {
	free(node->data);
	free(node);
}

Node* peek(List* list) {
	return list->head;
}
