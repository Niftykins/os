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

void push(List* list, char name[]) {
	Node* newNode;

	newNode = (Node*)malloc(sizeof(Node));
	newNode->name = (char*)malloc(FILE_LENGTH * sizeof(char));

	strcpy(newNode->name, name);
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

void freeList(List* list) {
	Node* node, *nextNode;

	node = list->head;

	while (node != NULL) {
		nextNode = node->next;
		free(node->name);
		free(node);
		node = nextNode;
	}

	free(list);
}

Node* peek(List* list) {
	return list->head;
}
