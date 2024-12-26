#include <stdio.h>
#include <stdlib.h>
#include "PCP.c"
#include "PLP.c"

int main(char* args)
{ 
    PCP *pcp = (PCP *)malloc(sizeof(PCP)); // puntero de planificador a corto plazo
    PLP *plp = (PLP *)malloc(sizeof(PLP)); // puntero de planificador a largo plazo
    
    iniciaCicloEjecucion(pcp, plp); // iniciamos el ciclo de ejecicion
    return 0;
}