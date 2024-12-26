#ifndef LISTA_PROCESO_C
#define LISTA_PROCESO_C

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "Lista_Proceso.h"
#include "Process.h"

Node *createNode(PCB *myProcess)
{
	Node *node = (Node *)malloc(sizeof(Node));
	node->myProcess = myProcess;
	node->next = NULL;
	return node;
}

void showNode(Node *node)
{

	printf("*\t%s", node->myProcess->PID);
	printf("\t\t%d", node->myProcess->state);
	printf("\t\t%d", node->myProcess->programCounter);
	printf("\t\t\t%d", node->myProcess->priority);
	printf("\t\t%d", node->myProcess->chargeCicleCpu);
	printf("\t\t\t%d", node->myProcess->changeContext);
	printf("\t\t\t%d", node->myProcess->PPID);
	printf("\t\t%s", node->myProcess->user);
	printf("\t*\n");
}

List *createList(int capacity)
{

	List *list = (List *)malloc(sizeof(List));
	list->firt = list->last = NULL;
	list->capacity = capacity;
	list->length = 0;
	pthread_mutex_init(&(list->syncList), NULL);
	return list;
}

bool isEmpty(List *list)
{

	return list->firt == NULL;
}

void insert(List *list, PCB *myProcess, PCB *lastProces)
{

	// Utilizamos un mutex para evitar que mas de un hilo inserte el mismo tiempo
	pthread_mutex_lock(&(list->syncList));

	// lista de solicitudes nuevas
	if (lastProces != NULL)
	{

		if (isEmpty(list))
		{
			srand(time(NULL));
			setNewPID(lastProces, myProcess);
			Node *node = createNode(myProcess);
			list->firt = node;
			list->last = node;
			list->length++;
			pthread_mutex_unlock(&(list->syncList));
			return;
		}
		else if (list->capacity > list->length)
		{

			setNewPID(lastProces, myProcess);
			Node *node = createNode(myProcess);
			list->last->next = node;
			list->last = node;
			list->length++;
			pthread_mutex_unlock(&(list->syncList));
			return;
		}
	}
	// lista de listos
	if (isEmpty(list))
	{

		srand(time(NULL));
		Node *node = createNode(myProcess);
		list->firt = node;
		list->last = node;
		list->length++;
	}
	else if (list->capacity > list->length)
	{

		setNewPID(list->last->myProcess, myProcess);
		Node *node = createNode(myProcess);
		list->last->next = node;
		list->last = node;
		list->length++;
	}

	// Despues de todos los casos posibles de inserccion, desbloqueamos el mutex
	// para que otros hilos puedan acceder al metodo insertar
	pthread_mutex_unlock(&(list->syncList));
}

// inserta al final de la lista
void insertLast(List *list, PCB *myProcess)
{

	// Utilizamos un mutex para evitar que mas de un hilo inserte el mismo tiempo
	pthread_mutex_lock(&(list->syncList));

	if (isEmpty(list))
	{
		Node *node = createNode(myProcess);
		list->firt = node;
		list->last = node;
		list->length++;
	}
	else if (list->capacity > list->length)
	{
		Node *node = createNode(myProcess);
		list->last->next = (Node *)malloc(sizeof(Node));
		list->last->next = node;
		list->last = node;
		list->length++;
	}

	// Despues de todos los casos posibles de inserccion, desbloqueamos el mutex
	// para que otros hilos puedan acceder al metodo insertar
	pthread_mutex_unlock(&(list->syncList));
}

void showList(List *list)
{

	// Bloquear para evitar que se modifique algun valor mientras recorro la lista
	pthread_mutex_lock(&(list->syncList));

	Node *nodeAux = list->firt;

	while (nodeAux != NULL)
	{
		showNode(nodeAux);
		nodeAux = nodeAux->next;
	}

	// Desbloquear el mutex y que todo siga igual
	pthread_mutex_unlock(&(list->syncList));
}

void deleteNode(List *list, Node *node)
{
	// Evitar que ningun hilo acceda a la lista hasta que algun nodo se elimine
	pthread_mutex_lock(&(list->syncList));

	if (isEmpty(list))
	{
		pthread_mutex_unlock(&(list->syncList));
		return;
	}
	else if (node == list->firt)
	{
		// se reordena la lista
		list->firt = list->firt->next;
		if (node == list->last)
		{
			list->last = NULL;
		}
		list->length--;
	}
	else if (node == list->last)
	{
		for (Node *actual = list->firt; actual != NULL; actual = actual->next)
		{

			if (actual->next == node)
			{
				actual->next = NULL;
				list->last = actual;
				list->length--;
			}
		}
	}
	else
	{
		for (Node *actual = list->firt; actual != NULL; actual = actual->next)
		{

			if(actual->next == node){
				actual->next=actual->next->next;
				list->length--;
			}
		}
	}
	// se libera memoria
	node = NULL;
	free(node);

	// Desbloquear el mutex y que la ejecucion siga normal
	pthread_mutex_unlock(&(list->syncList));
}

void insertToReady(List *listReady, PCB *pcb)
{

	// Utilizamos un mutex para evitar que mas de un hilo inserte el mismo tiempo
	pthread_mutex_lock(&(listReady->syncList));
	for (Node *actual = listReady->firt; actual != NULL; actual = actual->next)
	{
		if (actual->myProcess->PPID > pcb->PPID)
		{
			Node *node = createNode(pcb);
			node->next = actual;
			if (actual == listReady->firt)
			{
				listReady->firt = node;
			}
			listReady->length++;
			pthread_mutex_unlock(&(listReady->syncList));
			return;
		}
		else if ((actual->next != NULL) && (actual->next->myProcess->PPID > pcb->PPID))
		{
			Node *node = createNode(pcb);
			node->next = actual->next;
			actual->next = node;
			listReady->length++;
			pthread_mutex_unlock(&(listReady->syncList));
			return;
		}
		else
		{
			pthread_mutex_unlock(&(listReady->syncList));
			insertLast(listReady, pcb);
		}
	}
	pthread_mutex_unlock(&(listReady->syncList));
	return;
}

#endif
