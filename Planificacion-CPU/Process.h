#ifndef PROCESS_H 
#define PROCESS_H
#include <stdio.h>
#include <time.h>
typedef struct PCB
{
	char *PID;
	int state; // listo = 1, ejecucion = 2, E/S = 3 , nuevo Proceso = 4
	int programCounter;//hola
	int priority;
	int chargeCicleCpu;
	struct tm timeInitEject;
	struct tm timeEndEject;
	struct tm timeInitNewProcess;
	struct tm timeEndNewProcess;
	struct tm timeInitIO;
	struct tm timeEndIO;
	struct tm timeInitReady;
	struct tm timeEndReady;
	int changeContext;
	int PPID;
	char *user;
	void *pointerToMemory;
} PCB;

typedef struct pointerToMemory
{
	int PID;
	int state; // listo = 1, ejecucion = 2, E/S = 3 , nuevo Proceso = 4
	int programCounter;
	int priority;
	int chargeCicleCpu;
	int timeInitEject;
	int timeEndEject;
	int timeInitNewProcess;
	int timeEndNewProcess;
	int timeInitIO;
	int timeEndIO;
	int timeInitReady;
	int timeEndReady;
	int changeContext;
	int pointerToMemory;
	int PPID;
	int user;
} PTM;



struct tm addTimeNow();

int add(int ppid);

void setNewPID(PCB *lastProcess, PCB *actualProcess);

PCB *inicialicerProcess();

void *resetProcess(PCB *pcb);

void writeTXT(PCB *pcb);

#endif