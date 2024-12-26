#ifndef tbProcces_c
#define tbProcces_c
#include <stdio.h>
// Estructura de la tabla de procesos
struct processTable
{
	char *toStringTableHeader;
	char *toStringTableData;
	char *toStringTableEnd;
};
double averageWaitingTime(PCP *pcp);

double averageIOTime(PCP *pcp);

double averageExecutionTime(PCP *pcp);

double averageResponseTime(PCP *pcp);

char *barOfUtilizationCPU(float utilizationOfCPU);

float getUtilizationOfCPU(PCP *pcp);

void toStringProcess(PCP *pcp);

#endif // tbProcess_c


