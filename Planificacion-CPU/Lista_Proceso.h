#ifndef LISTA_PROCESO_H
#define LISTA_PROCESO_H

#include <stdio.h>
#include "Process.h"
#include "Lista_Proceso.h"
#include <pthread.h>
#include "stdbool.h"


typedef struct Node
{
	PCB *myProcess;
	struct Node *next;
	pthread_mutex_t syncNode; //Mutex del nodo para sincronizacion
} Node;

typedef struct List
{
	Node *firt;
	Node *last;
	int capacity;
	int length;
	pthread_mutex_t syncList; //Mutex de la lista para sincronizacion

} List;

Node *createNode(PCB *myProcess);

void showNode(Node *node);

List *createList(int capacity);

bool isEmpty(List *list);

void insert(List *list, PCB *myProcess, PCB *lastProces);

void insertLast(List *list, PCB *myProcess);

void showList(List *list);

void deleteNode(List *list, Node *node);

void insertToReady(List *listReady, PCB *pcb);

#endif


