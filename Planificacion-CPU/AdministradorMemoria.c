#ifndef ADMINISTRADORMEMORIA_C
#define ADMINISTRADORMEMORIA_C
#include "AdministradorMemoria.h"

int ultimoProcesoIngresado = 0;

void setBlockSize(PMP *pmp)
{
    pmp->list->blockSize = (int *)malloc(sizeof(int)); // Espacio en memoria

    *(pmp->list->blockSize) = sizeof(PCB);
}

int maxValue(PCB *pcb)
{
    int pesoTotal = 100;
    return pesoTotal;
}

int totalValue(PCB *pcb)
{
    srand(time(NULL));
    int peso = rand() % maxValue(pcb);

    return peso;
}

PCB *getMemoryPointer(LinkedList *listReady, char *PID)
{

    pthread_mutex_lock(&listReady->mutexLinkedList);
    // metodo
    NodeLinked *current = listReady->first;
    while (current != NULL)
    {
        if (current->myProcess->PID == PID)
        {
            return current->myProcess; // Devuelve el puntero al PCB
        }
        current = current->next;
    }
    pthread_mutex_unlock(&listReady->mutexLinkedList);
    return NULL;
}

LinkedList *createLinkedList(int capacity)
{

    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    list->first = list->last = NULL;
    list->capacity = capacity;
    list->length = 0;
    pthread_mutex_init(&(list->mutexLinkedList), NULL);
    return list;
}

void *allocateMemoryForNodeLinked(NodeLinked *nodeLinked)
{
    // Calcular el tamaño necesario para el nodeLinked
    pthread_mutex_init(&(nodeLinked->mutexNodeLinked), NULL);
    size_t size = sizeof(*nodeLinked); // Tamaño de la estructura, no del puntero
    void *memory = malloc(size);       // Realizar la asignación de memoria
    return memory;                     // Devolver el puntero a la memoria reservada
}

void *allocateMemoryForLinkedList(LinkedList *linkedList)
{
    // Calcular el tamaño necesario para el linkedList
    size_t size = sizeof(*linkedList); // Tamaño de la estructura, no del puntero

    void *memory = malloc(size); // Realizar la asignación de memoria

    if (memory == NULL)
    {
        // Manejar el caso en que malloc falle
        // Imprimir un mensaje de error o realizar alguna acción adecuada
        printf("Error: No se pudo asignar memoria para LinkedList.\n");
        exit(EXIT_FAILURE); // Otra acción según sea necesario
    }

    // Inicializar el mutex si se asignó correctamente la memoria
    pthread_mutex_init(&(linkedList->mutexLinkedList), NULL);
    return memory; // Devolver el puntero a la memoria reservada
}

PCB *allocateMemoryForPCB(LinkedList *list)
{
    PCB *pcb = inicialicerProcess();
    insertToLinkedList(list, pcb);
    return pcb;
}

void *allocateMemoryForPTM(PTM *ptm)
{
    // Calcular el tamaño necesario para el PTM
    size_t size = sizeof(PTM);
    void *memory = malloc(size); // Realizar la asignación de memoria
    return memory;               // Devolver el puntero a la memoria reservada
}

void freeMemory(void *memory)
{
    free(memory);
}

NodeLinked *createLinkedNode(PCB *myProcess)
{
    NodeLinked *nodeLinked = allocateMemoryForNodeLinked(nodeLinked);
    nodeLinked->myProcess = myProcess;
    nodeLinked->next = NULL;
    nodeLinked->asignado = true;
    nodeLinked->tamano = sizeof(myProcess);
    return nodeLinked;
}

bool isEmptyLL(LinkedList *linkedList)
{
    return linkedList->first == NULL;
}

void insertToLinkedList(LinkedList *list, PCB *myProcess)
{

    // Utilizamos un mutex para evitar que mas de un hilo inserte el mismo tiempo
    pthread_mutex_lock(&(list->mutexLinkedList));

    if (isEmptyLL(list))
    {

        NodeLinked *node = createLinkedNode(myProcess);
        list->first = node;
        list->last = node;
        list->length++;
        pthread_mutex_unlock(&(list->mutexLinkedList));
        return;
    }
    else if (list->capacity > list->length)
    {

        NodeLinked *node = createLinkedNode(myProcess);
        list->last->next = node;
        list->last = node;
        list->length++;
        pthread_mutex_unlock(&(list->mutexLinkedList));
        return;
    }

    // Despues de todos los casos posibles de inserccion, desbloqueamos el mutex
    // para que otros hilos puedan acceder al metodo insertar
    pthread_mutex_unlock(&(list->mutexLinkedList));
}

void showNodeLinkedList(NodeLinked *nodeList)
{
    printf("*\t%s", nodeList->myProcess->PID);
    printf("\t\t%d", nodeList->myProcess->state);
    printf("\t\t%d", nodeList->myProcess->programCounter);
    printf("\t\t\t%d", nodeList->myProcess->priority);
    printf("\t\t%d", nodeList->myProcess->chargeCicleCpu);
    printf("\t\t\t%d", nodeList->myProcess->changeContext);
    printf("\t\t\t%d", nodeList->myProcess->PPID);
    printf("\t\t%s", nodeList->myProcess->user);
    printf("\t*\n");
}

void showLinkedList(LinkedList *linkedList)
{

    NodeLinked *nodeList = linkedList->first;

    while (nodeList != NULL)
    {

        showNodeLinkedList(nodeList);
        nodeList = nodeList->next;
    }
}
void asignarSegmentoSiguienteAjuste(PCB *myProcess, LinkedList *linkedList)
{

    int tamanoProceso = totalValue(myProcess);
    NodeLinked *nodoActual = linkedList->first;
    NodeLinked *nodoSiguienteAjuste = NULL;

    while (nodoActual != NULL)
    {
        if (nodoActual->inicio == ultimoProcesoIngresado)
        {
            while (nodoActual != NULL)
            {
                if (!nodoActual->asignado && nodoActual->tamano >= tamanoProceso)
                {
                    nodoSiguienteAjuste = nodoActual;
                    break;
                }
                nodoActual = nodoActual->next;
            }
            break;
        }
        else
        {
            nodoActual = nodoActual->next;
        }
    }

    if (nodoSiguienteAjuste != NULL)
    {
        nodoSiguienteAjuste->asignado = true;
        if (nodoSiguienteAjuste->tamano >= tamanoProceso)
        {
            int nuevoTamanoHueco = nodoSiguienteAjuste->tamano - tamanoProceso;
            nodoSiguienteAjuste->tamano = tamanoProceso;
            printf("El proceso %s ha sido asignado con tamano %d\n\n", myProcess->PID, tamanoProceso);

            if (nuevoTamanoHueco > 0)
            {
                // Crear un nuevo nodo para el hueco
                NodeLinked *nuevoNodoHueco = createLinkedNode(NULL);
                nuevoNodoHueco->inicio = nodoSiguienteAjuste->inicio + tamanoProceso;
                nuevoNodoHueco->tamano = nuevoTamanoHueco;
                nuevoNodoHueco->asignado = false;

                // Insertar el nuevo nodo después del nodo actual
                nuevoNodoHueco->next = nodoSiguienteAjuste->next;
                nodoSiguienteAjuste->next = nuevoNodoHueco;

                // Actualizar el valor de ultimoProcesoIngresado
                ultimoProcesoIngresado = nodoSiguienteAjuste->inicio;

                // Incrementar el tamaño de la lista enlazada
                linkedList->length++;
            }
        }
    }
    else
    {
        printf("No hay suficiente espacio para asignar el proceso %s con tamano %d\n\n", myProcess->PID, tamanoProceso);
    }
}

#endif