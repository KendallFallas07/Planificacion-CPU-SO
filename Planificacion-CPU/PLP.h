#ifndef PLP_H
#define PLP_H
#include "Lista_Proceso.h"
typedef struct PLP
{
    /* 
        algoritmo,  int numerica(cuando se cambie el numero cambia el algoritmo);(fcfs,roud robin etc)
        un puntero de la lista de 100 procesos
         
    */
    int* policy;
    List *list;
    
}PLP;


void inicializerPLP(PLP* plp);
#endif // PLP_c