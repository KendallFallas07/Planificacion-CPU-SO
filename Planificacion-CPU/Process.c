#ifndef PROCESS_C
#define PROCESS_C

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "Process.h"
#include "PCP.c"

struct tm addTimeNow(){
	struct tm*  t; 
    // Obtener el tiempo actual
	time_t timeUnix;
    timeUnix = time(NULL);
    // Convertir el tiempo UNIX a hora local
	t = localtime(&timeUnix);
	return *t;
}


int add(int ppid)
{
	return ppid + 1;
}
void setNewPID(PCB *lastProcess, PCB *actualProcess)
{
	// Recuperamos el ultimo PID de la lista  de procesos listos
	char *lastPID = lastProcess->PID;

	// Creamos un nuevo PID basado en el ultimo PID
	char *newPID = (char *)malloc(5 * sizeof(char *)); // +1 para el caracter nulo
	strcpy(newPID, lastPID);						   // copiamos el contenido del char*

	for (int i = strlen(newPID) - 1; i >= 0; i--)
	{
		if (newPID[i] < '9')
		{
			newPID[i]++;
			break; // A-000
		}
		else
		{
			newPID[i] = '0';
		}
	}

	// Asignamos el nuevo PID al proceso actual
	actualProcess->PID = newPID;
	// Mediante la funcion add se traera su tiempo de llegada y le dara un random
	actualProcess->PPID = add(lastProcess->PPID);
}

// inicializa el nuevo proceso con los parametros que se van a requerir
PCB *inicialicerProcess()
{
	PCB *process = (PCB*)malloc(sizeof(PCB));
	process->PID = "A-000";
	process->state = 4;
	process->programCounter = 0;
	process->chargeCicleCpu = rand() % (85001); // kendall como arroz!!
	process->priority = 0;	//0 es que notiene apropiatividad - 1 es que si tiene apropiatividad
	process->changeContext = 10 + rand() % (30 - 10 + 1); // ceasar 2024
	process->PPID = 0;
	process->user = "comoArroz!";
	PTM *ptm = (PTM *)malloc(sizeof(PTM));
	ptm->PID = sizeof(process->PID);
	ptm->state = sizeof(process->state);
	ptm->programCounter = sizeof(process->programCounter);
	ptm->chargeCicleCpu = sizeof(process->chargeCicleCpu);
	ptm->priority = sizeof(process->priority);
	ptm->changeContext = sizeof(process->changeContext);
	ptm->PPID = sizeof(process->PPID);
	ptm->user = sizeof(process->user);
	ptm->timeEndEject = sizeof(process->timeEndEject);
	ptm->timeEndIO = sizeof(process->timeEndIO);
	ptm->timeEndNewProcess = sizeof(process->timeEndNewProcess);
	ptm->timeEndReady = sizeof(process->timeEndReady);
	ptm->timeInitEject = sizeof(process->timeInitEject);
	ptm->timeInitIO = sizeof(process->timeInitIO);
	ptm->timeInitNewProcess = sizeof(process->timeInitNewProcess);
	ptm->timeInitReady = sizeof(process->timeInitReady);
	process->pointerToMemory = ptm;
	return process; // como arroz
}

void *resetProcess(PCB *pcb)
{
	pcb->programCounter = 0;
	pcb->chargeCicleCpu = rand() % (85001);
	pcb->priority = 0;	//0 es que notiene apropiatividad - 1 es que si tiene apropiatividad
	pcb->changeContext = 10 + rand() % (30 - 10 + 1);
	return pcb; // como arroz
}

void writeTXT(PCB *pcb)
{
	// necesario para poder crear y acceder al archivo
	FILE *file; // variable del archivo a escribir

	// nombres de los archivos y concatenacion como arroz de C calidad
	char *name = (char*)malloc(256*sizeof(char));
	char *path = "PCB/";
	char *suf = ".txt";

	// Formato de cadena con variables
	sprintf(name, "%s%s%s", path, pcb->PID, suf);

	// creo el txt con numbre de PID
	file = fopen(name, "w");

	// datos estructurados de el procesos actual
	fprintf(file, "[General]\nProceso id = %s\n", pcb->PID);
	fprintf(file, "Estado = %d\n", pcb->state);
	fprintf(file, "Contador de programa = %d\n", pcb->programCounter);
	fprintf(file, "Ciclos de carga CPU = %d\n", pcb->chargeCicleCpu);
	fprintf(file, "Apropiatividad = %d\n", pcb->priority);
	fprintf(file, "Tiempo de llegada a Listo = %d\n", pcb->timeInitReady.tm_sec);
	fprintf(file, "Tiempo de salida de Listo = %d\n", pcb->timeEndReady.tm_sec);
	fprintf(file, "Tiempo de llegada a Ejecucion = %d\n", pcb->timeInitEject.tm_sec);
	fprintf(file, "Tiempo de salida de Ejecucion = %d\n", pcb->timeEndEject.tm_sec);
	fprintf(file, "Tiempo de llegada a E/S = %d\n", pcb->timeInitIO.tm_sec);
	fprintf(file, "Tiempo de salida de E/S = %d\n", pcb->timeEndIO.tm_sec);
	fprintf(file, "Tiempo de llegada a Nuevos Procesos = %d\n", pcb->timeInitNewProcess.tm_sec);
	fprintf(file, "Tiempo de salida de Nuevos Procesos = %d\n", pcb->timeEndNewProcess.tm_sec);
	fprintf(file, "Cambio de contexto = %d\n", pcb->changeContext);
	fprintf(file, "Proceso TiempoDeLLegada = %d\n", pcb->PPID);
	fprintf(file, "Usuario = %s\n", pcb->user);
	fprintf(file, "[Punteros de memoria]\n");

	PTM *temp = (PTM *)malloc(sizeof(PTM));
	temp = (PTM*)pcb->pointerToMemory;
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->PID, temp->PID);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->state, temp->state);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->programCounter, temp->programCounter);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->chargeCicleCpu, temp->chargeCicleCpu);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->priority, temp->priority);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->timeEndEject, temp->timeEndEject);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->timeEndIO, temp->timeEndIO);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->timeEndNewProcess, temp->timeEndNewProcess);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->timeEndReady, temp->timeEndReady);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->timeInitEject, temp->timeInitEject);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->timeInitIO, temp->timeInitIO);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->timeInitNewProcess, temp->timeInitNewProcess);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->timeInitReady, temp->timeInitReady);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->changeContext, temp->changeContext);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->PPID, temp->PPID);
	fprintf(file, "Direccion de memoria = %p, Peso = %d\n", &temp->user, temp->user);

	fclose(file);
}
 // Process.c
 #endif