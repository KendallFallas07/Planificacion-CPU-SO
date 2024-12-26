#ifndef TBPROCESS_C
#define TBPROCESS_C
#include <stdio.h>
#include "tbProcess.h"
#include "Lista_Proceso.c"
#include "PLP.c"
#include "PCP.c"

double averageWaitingTime(PCP *pcp)
{
	if ((pcp->Ready->list->length > 0) || (pcp->inputOutput->list->length > 0))
	{

		double tiempoTotal = 0.0;

		Node *actual = pcp->Ready->list->firt;

		while (actual != NULL)
		{
			// Calcula el tiempo de respuesta para cada proceso
			tiempoTotal = tiempoTotal + (actual->myProcess->timeEndReady.tm_sec - actual->myProcess->timeInitReady.tm_sec);
			actual = actual->next;
		}
		if (pcp->inputOutput->list->length > 0)
		{
			Node *actualIO = pcp->inputOutput->list->firt;

			while (actualIO != NULL)
			{
				tiempoTotal = tiempoTotal + (actualIO->myProcess->timeEndReady.tm_sec - actualIO->myProcess->timeInitReady.tm_sec);
				actualIO = actualIO->next;
			}
		}
		if (pcp->Execution->processActual != NULL)
		{
			tiempoTotal = (pcp->Execution->processActual->timeEndReady.tm_sec - pcp->Execution->processActual->timeInitReady.tm_sec);
		}
		return tiempoTotal / pcp->Ready->list->length + pcp->inputOutput->list->length + 1;
	}
	else
	{
		return 0.0;
	}
}

double averageIOTime(PCP *pcp)
{
	if ((pcp->Ready->list->length > 0) || (pcp->inputOutput->list->length > 0))
	{

		double tiempoTotal = 0.0;

		Node *actual = pcp->Ready->list->firt;

		while (actual != NULL)
		{
			// Calcula el tiempo de respuesta para cada proceso
			tiempoTotal = tiempoTotal + (actual->myProcess->timeEndIO.tm_sec - actual->myProcess->timeInitIO.tm_sec);
			actual = actual->next;
		}
		if (pcp->inputOutput->list->length > 0)
		{
			Node *actualIO = pcp->inputOutput->list->firt;

			while (actualIO != NULL)
			{
				tiempoTotal = tiempoTotal + (actualIO->myProcess->timeEndIO.tm_sec - actualIO->myProcess->timeInitIO.tm_sec);
				actualIO = actualIO->next;
			}
		}
		if (pcp->Execution->processActual != NULL)
		{
			tiempoTotal = (pcp->Execution->processActual->timeEndIO.tm_sec - pcp->Execution->processActual->timeInitIO.tm_sec);
		}
		return tiempoTotal / pcp->Ready->list->length + pcp->inputOutput->list->length + 1;
	}
	else
	{
		return 0.0;
	}
}

double averageExecutionTime(PCP *pcp)
{

	if ((pcp->Ready->list->length > 0) || (pcp->inputOutput->list->length > 0))
	{

		double tiempoTotal = 0.0;

		Node *actual = pcp->Ready->list->firt;

		while (actual != NULL)
		{
			// Calcula el tiempo de respuesta para cada proceso
			tiempoTotal = tiempoTotal + (actual->myProcess->timeEndEject.tm_sec - actual->myProcess->timeInitEject.tm_sec);
			actual = actual->next;
		}
		if (pcp->inputOutput->list->length > 0)
		{
			Node *actualIO = pcp->inputOutput->list->firt;

			while (actualIO != NULL)
			{
				tiempoTotal = tiempoTotal + (actualIO->myProcess->timeEndEject.tm_sec - actualIO->myProcess->timeInitEject.tm_sec);;
				actualIO = actualIO->next;
			}
		}
		if (pcp->Execution->processActual != NULL)
		{
			tiempoTotal = (pcp->Execution->processActual->timeEndEject.tm_sec - pcp->Execution->processActual->timeInitEject.tm_sec);
		}
		return tiempoTotal / pcp->Ready->list->length + pcp->inputOutput->list->length + 1;
	}
	else
	{
		return 0.0;
	}
}

double averageResponseTime(PCP *pcp)
{
	return averageExecutionTime(pcp) + averageIOTime(pcp) + averageWaitingTime(pcp);
}

char *barOfUtilizationCPU(float utilizationOfCPU){

	
	if(utilizationOfCPU > 0 && utilizationOfCPU <= 20){

		return "■";

	} else if(utilizationOfCPU > 20 && utilizationOfCPU <= 40){

		return "■ ■";

	} else if(utilizationOfCPU > 40 && utilizationOfCPU <= 60){

		return "■ ■ ■";

	} else if(utilizationOfCPU > 60 && utilizationOfCPU <= 80){

		return "■ ■ ■ ■";

	} else {

		return "■ ■ ■ ■ ■";

	}
	
}

float getUtilizationOfCPU(PCP *pcp)
{

	// Pregunto si alguien se esta ejecutando
	if (pcp->Execution->processActual != NULL)
	{

		if (*(pcp->policy) == 1)
		{

			if (pcp->Execution->processActual->programCounter > 42500)
			{
				// La carga del proceso es mas que el procesamiento de CPU
				return 100.0f;
			}
			else
			{
				// El CPU pudo sacar el proceso
				return (pcp->Execution->processActual->programCounter / 42500.0f) * 100.0f;
			}
		}
		else
		{

			if ((pcp->Execution->processActual->programCounter) * 100 / (*(pcp->quantum) * 800) > 100)
			{
				// La carga del proceso es mas que el procesamiento de CPU
				return 100.0f;
			}
			else
			{
				// El CPU pudo sacar el proceso
				return ((pcp->Execution->processActual->programCounter) * 100.0f / (*(pcp->quantum) * 800.0f));
			}

		}
	}
	else
	{
		// No hay nadie ejecutandose
		return 0.0f;
	}
}

// Datos de la tabla de procesos
void toStringProcess(PCP *pcp)
{

	struct processTable myProcessTable;
	// Variable politica
	char *politica;
	// variables de tiempo
	char *tbTime;
	int *processNumber;
	pthread_mutex_lock(&pcp->Ready->list->syncList);
	pthread_mutex_lock(&pcp->inputOutput->list->syncList);
	double averageTimeWaiting = averageWaitingTime(pcp);
	double averageTimeResponse = averageResponseTime(pcp);
	double averageTimeIO = averageIOTime(pcp);
	double averageTimeExecution = averageExecutionTime(pcp);
	float utilizationCPU = getUtilizationOfCPU(pcp);
	pthread_mutex_unlock(&pcp->Ready->list->syncList);
	pthread_mutex_unlock(&pcp->inputOutput->list->syncList);

	tbTime = "************************************************************************Tabla de Procesos************************************************************************";

	myProcessTable.toStringTableHeader = tbTime;
	// Printe de la cabecera de la tabla de tiempos promedios en listos
	printf("%s\n", myProcessTable.toStringTableHeader);

	printf("%s", "Tiempo promedio de espera en Listo: ");

	printf("%f\n", averageTimeWaiting);

	printf("%s", "Tiempo promedio en ejeuccion: ");

	printf("%f\n", averageTimeExecution);

	printf("%s", "Tiempo promedio en espera por E/S: ");

	printf("%f\n", averageTimeIO);

	printf("%s", "Tiempo promedio de repuesta: ");

	printf("%f\n", averageTimeResponse);

	printf("%s", "Utilizacion de la CPU: ");

	printf("%.2f %%\n", utilizationCPU);

	printf("%s", "Cantidad de procesos: ");

	printf("%d \n", pcp->Ready->list->length + pcp->inputOutput->list->length + 1);


	tbTime = "*****************************************************************************************************************************************************************";

	myProcessTable.toStringTableHeader = tbTime;

	printf("%s\n\n", myProcessTable.toStringTableHeader);
	printf("%s", "Politica:\n");

	if (*(pcp->policy) == 1)
	{
		politica = "******************************************************************************FCFS*******************************************************************************";
	}
	else
	{
		politica = "*******************************************************************************RR********************************************************************************";
		printf("\nQuantum actual: %d\n", *(pcp->quantum));
		printf("Grafico: %s \n", barOfUtilizationCPU(utilizationCPU));
	}
	myProcessTable.toStringTableHeader = politica;

	printf("%s\n\n", myProcessTable.toStringTableHeader);
	// Cabecera
	char *header = "**********************************************************************PROCESO EN EJECUCION***********************************************************************\n"
				   "*\tPID\t\tEstado\t\tContador de Programa\tPrioridad\tCiclos CPU\tCambio de Contexto\tTiempo de Llegada\tUsuario\t\t*\n";
	// printf("%s",myProcessTable.toStringTableHeader);
	myProcessTable.toStringTableHeader = header; //
	printf("%s", myProcessTable.toStringTableHeader);

	if (pcp->Execution->processActual != NULL)
	{
		printf("*\t%s", pcp->Execution->processActual->PID);
		printf("\t\t%d", pcp->Execution->processActual->state);
		printf("\t\t%d", pcp->Execution->processActual->programCounter);
		printf("\t\t\t%d", pcp->Execution->processActual->priority);
		printf("\t\t%d", pcp->Execution->processActual->chargeCicleCpu);
		printf("\t\t\t%d", pcp->Execution->processActual->changeContext);
		printf("\t\t\t%d", pcp->Execution->processActual->PPID);
		printf("\t\t%s", pcp->Execution->processActual->user);
		printf("\t*\n");
	}
	// Se agrega el valor de cabecera
	char *end = "*****************************************************************************************************************************************************************";

	myProcessTable.toStringTableEnd = end;

	printf("%s", myProcessTable.toStringTableEnd);

	printf("\n\n\n\n");
	char *header2 = "*************************************************************************TABLA DE LISTO**************************************************************************\n"
					"*\tPID\t\tEstado\t\tContador de Programa\tPrioridad\tCiclos CPU\tCambio de Contexto\tTiempo de Llegada\tUsuario\t\t*\n";

	// Se agrega el valor de cabecera
	myProcessTable.toStringTableHeader = header2; //
	printf("%s", myProcessTable.toStringTableHeader);
	showList(pcp->Ready->list);
	// Final
	char *end2 = "*****************************************************************************************************************************************************************";

	// Se agrega el valor de cabecera
	myProcessTable.toStringTableEnd = end2;

	printf("%s", myProcessTable.toStringTableEnd);

	printf("\n\n\n\n");

	//------------------------------------------------------------------------------------------------------------------------------------------------

	char *headerWaiting = "************************************************************************TABLA DE ESPERA E/S**********************************************************************\n"
						  "*\tPID\t\tEstado\t\tContador de Programa\tPrioridad\tCiclos CPU\tCambio de Contexto\tTiempo de Llegada\tUsuario\t\t*\n";

	myProcessTable.toStringTableHeader = headerWaiting;

	printf("%s", myProcessTable.toStringTableHeader);

	showList(pcp->inputOutput->list);

	char *endWaiting = "*****************************************************************************************************************************************************************";

	// Se agrega el valor de el final de la lista
	myProcessTable.toStringTableEnd = endWaiting;

	printf("%s", myProcessTable.toStringTableEnd);

	printf("\n");
}
#endif