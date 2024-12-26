#ifndef PCP_H
#define PCP_H

#include "Lista_Proceso.h"
#include <stdio.h>
#include <pthread.h>
#include "Process.h"
#include <stdbool.h>
#include "PLP.h"
#include "AdministradorMemoria.h"
typedef struct Ready
{
    List* list;
    int* policy;

} Ready;

typedef struct inputOutput
{
    List *list;
} IO;

typedef struct execution
{
    PCB *processActual;
} EX;

typedef struct PCP
{
    int* policy;
    IO* inputOutput;
    EX* Execution;
    Ready* Ready;
    bool* showTable;
    bool* pcpAuto;
    int* quantum;
    PMP* pmp;
} PCP;

typedef struct argsFCFS
{
    PCP *pcp;
    PLP *plp;
} argsFCFS;


typedef struct IOargs
{
    PCP *pcp;
    Node *node;
} IOargs;

//metodos
void inicializerPCP(PCP* pcp);

void iniciaCicloEjecucion(PCP* pcp, PLP* plp);

int getPolicy(PCP *pcp);

int IOtime();

void setApropiatividad(PCB *pcb);

void setPoliticaAdministrativa(PCP *pcp, int policy);

void changeState(PCB *pcb, int value);

void *threadReady(void *arg);

void *threadIO(void *arg);

void checkTable(argsFCFS *data);

#endif


