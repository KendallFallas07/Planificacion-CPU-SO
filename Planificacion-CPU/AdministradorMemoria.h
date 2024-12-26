#ifndef ADMINISTRADORMEMORIA_H
#define ADMINISTRADORMEMORIA_H

#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "Process.c"

typedef struct NodeLinked
{
	PCB *myProcess;
	struct NodeLinked *next;
	int inicio;
	int tamano;
	bool asignado;
	pthread_mutex_t mutexNodeLinked;

} NodeLinked;

typedef struct LinkedList
{
	NodeLinked *first;
	NodeLinked *last;
	NodeLinked *lastConsult;
	int capacity;
	int length;
	int *blockSize; // Tamano del bloque
	pthread_mutex_t mutexLinkedList;

} LinkedList;

typedef struct PMP
{

	LinkedList *list;

} PMP;

void setBlockSize(PMP *pmp);

int maxValue(PCB *pcb);

int totalValue(PCB *pcb);

PCB *getMemoryPointer(LinkedList *listReady, char *PID);

LinkedList *createLinkedList(int capacity);

void *allocateMemoryForNodeLinked(NodeLinked *nodeLinked);

void *allocateMemoryForLinkedList(LinkedList *linkedList);

PCB * allocateMemoryForPCB(LinkedList *list);
void *allocateMemoryForPTM(PTM *ptm);

void freeMemory(void *memory);

NodeLinked *createLinkedNode(PCB *myProcess);
bool isEmptyLL(LinkedList *linkedList);

void insertToLinkedList(LinkedList *list, PCB *myProcess);

void showNodeLinkedList(NodeLinked *nodeList);
void showLinkedList(LinkedList *linkedList);

void asignarSegmentoSiguienteAjuste(PCB *myProcess, LinkedList *linkedList);

#endif