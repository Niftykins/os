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
		free(node->name);
		free(node);
		node = nextNode;
	}

	free(list);
}

void freeNode(Node* node) {
	free(node->name);
	free(node);
}

void printList(List* list) {
	Node* curr;
	curr = list->head;

	if(list->head == NULL) {
		printf("List is empty.\n");
	} else {
		printf("List is: ");
	
		while(curr != NULL) {
			printf("%s, ", curr->name);
			curr = curr->next;
		}
		printf("\n");
	}
}
Node* peek(List* list) {
	return list->head;
}
