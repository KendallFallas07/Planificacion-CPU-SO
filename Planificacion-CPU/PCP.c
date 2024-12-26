#ifndef PCP_C
#define PCP_C

#include "PCP.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "tbProcess.c"
#include "Process.c"
#include "AdministradorMemoria.c"
int IOtime()
{
    srand(time(NULL));
    return 1 + rand() % 10;
}
void setApropiatividad(PCB *pcb)
{
    pcb->priority = 1;
}

void setPoliticaAdministrativa(PCP *pcp, int policy)
{
    pcp->policy = &policy;
}
// para cambiar el estado
void changeState(PCB *pcb, int value)
{
    pcb->state = value;
}

void *threadIO(void *arg)
{
    // inzumos
    IOargs *data = (IOargs *)arg;
    // sacamos el tiempo de E/S
    int t = IOtime();
    // esperamos
    sleep(t);
    // referenciamos el nodo
    Node *node = data->node;
    // asignamos el tiempo que duro en E/S
    // guardamos los datos
    writeTXT(data->node->myProcess);
    // se resetea el proceso
    resetProcess(node->myProcess);
    // se asigna nuevo estado
    node->myProcess->state = 1;
    // se inserta el nuevo nodo reseteado a la lista de listos
    insertLast(data->pcp->Ready->list, node->myProcess);
    // se elimina el nodo de la lista de IO
    deleteNode(data->pcp->inputOutput->list, data->node);

    return NULL;
}

void InsertIO(PCP *pcp)
{
    // inzumos
    pthread_t counter;
    // cambia el estado
    pcp->Execution->processActual->state = 3;
    // insertar en IO
    insertLast(pcp->inputOutput->list, pcp->Execution->processActual);
    // creamos la estructura que envia los inzumos
    IOargs *data = (IOargs *)malloc(sizeof(IOargs));
    data->node = pcp->inputOutput->list->last;
    data->pcp = pcp;
    // creamos la rutina
    pthread_create(&counter, NULL, threadIO, (void *)data);
}

// Hilo para ejecutar los procesos en ejecuccion
void *excute(void *arg)
{

    // inzumos necesarios para el ciclo de ejecucion
    argsFCFS *data = (argsFCFS *)arg;
    PCP *pcp = data->pcp;

    // asigna tiempo de llegada
    pcp->Execution->processActual->timeInitEject = addTimeNow();
    int rafaga = 42500; // cantidad de rafaga en fcfs
    /// utilizando RR
    if (*(pcp->policy) == 2)
    {
        rafaga = *(pcp->quantum) * 800;
    }

    // ejecucion del proceso
    for (int i = 0; i < rafaga; i++)
    {
        usleep(1000000 / rafaga);
        pcp->Execution->processActual->programCounter++;
        if (pcp->Execution->processActual->chargeCicleCpu == pcp->Execution->processActual->programCounter)
        {
            ////////////////salgo de ejecucion y voy a E/S //////////////////////////////////
            InsertIO(pcp);
            return NULL;
        }
    }
    pcp->Execution->processActual->timeEndEject = addTimeNow();
    // cambiamos el estados y lo enviamos a la cola de listos
    changeState(pcp->Execution->processActual, 1);
    // lo guardamos de nuevo en la lista de listos
    pcp->Execution->processActual->timeInitReady = addTimeNow();
    insertToReady(pcp->Ready->list, pcp->Execution->processActual);

    // se libera el campo en ejecucion//
    pcp->Execution->processActual = NULL;

    return NULL; // termina ejecucion
}

bool isSumLessThat250(List *listReady, List *listIO, List *listNewProc)
{
    if (listReady->length + listIO->length + listNewProc->length < 250)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool checkListReadyToInsert(List *list)
{
    if (list->length < 150)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool checkListReadyToInsertFromNewProc(List *listReady, List *listNew)
{
    if (listReady->length < 150 && listNew->length > 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void orderListToReady(List *listReady, List *listNew)
{
    int cant = 150 - listReady->length;
    for (int i = 0; i < cant; i++)
    {
        insertLast(listReady, listNew->firt->myProcess);
        deleteNode(listNew, listNew->firt);
    }
}

// rutina encargada de crear procesos cada cierto tiempo
void *createProcess(void *arg)
{
    PCB *last = inicialicerProcess();
    argsFCFS *data = (argsFCFS *)arg;
    int createData = 1;
    //
    while (isSumLessThat250(data->pcp->Ready->list, data->pcp->inputOutput->list, data->plp->list))
    {
        if (checkListReadyToInsertFromNewProc(data->pcp->Ready->list, data->plp->list))
        {
            orderListToReady(data->pcp->Ready->list, data->plp->list);
        }
        // primero ver si puedo insertar en la lista de listos
        else if (checkListReadyToInsert(data->pcp->Ready->list))
        {
            insert(data->pcp->Ready->list, allocateMemoryForPCB(data->pcp->pmp->list), last);
            data->pcp->Ready->list->last->myProcess->timeInitReady = addTimeNow();
            changeState(data->pcp->Ready->list->last->myProcess, 1);
            last = data->pcp->Ready->list->last->myProcess;
            writeTXT(last);
        }
        else
        {
            insert(data->plp->list, allocateMemoryForPCB(data->pcp->pmp->list), last);
            data->plp->list->last->myProcess->timeInitNewProcess = addTimeNow();
            changeState(data->plp->list->last->myProcess, 4);
            last = data->plp->list->last->myProcess;
            writeTXT(last);
        }
        sleep(2);
    }
    return NULL;
}

void insertQuantum(PCP *pcp)
{
    int q;
    printf("Ingrese la cantidad de quantum deseado : \n");
    scanf("%d", &q);
    pcp->quantum = &q;
}

void normalizeRR(PCP *pcp, int *normalizeQuantum)
{

    if (*normalizeQuantum == 20)
    {
        int total = pcp->inputOutput->list->length + pcp->Ready->list->length;
        double pIO = pcp->inputOutput->list->length / total;
        double pReady = pcp->Ready->list->length / total;
        if (pIO > 0.75)
        {

            pcp->quantum = pcp->quantum - 37;
            system("clear");
            printf("Alerta: Balanceo de colas de E/S del planificador de CPU\nSe está realizando un balanceo de las colas de E/S del planificador de CPU.");
            sleep(2);
        }
        else if (pReady > 0.75)
        {
            system("clear");

            pcp->quantum = pcp->quantum + 27;
            printf("Alerta: Balanceo de colas de listos del planificador de CPU\nSe está realizando un balanceo de las colas de listos del planificador de CPU.");
            sleep(2);
        }
        *normalizeQuantum = 0;
    }
    else
    {
        *normalizeQuantum = *normalizeQuantum + 1;
    }
}

char getChar(char *message)
{
    char c = ' ';
    printf("%s", message);
    scanf("%c", &c);
    return c;
}

char *getString(char *message)
{
    char *c = (char *)malloc(sizeof(char) * 5); // Asignar memoria para la cadena
    if (c == NULL)
    {
        printf("Error: No se pudo asignar memoria\n");
        exit(1);
    }
    printf("%s", message);
    scanf("%s", c); // No se necesita "&" antes de "c"
    return c;
}

char getInt(char *message)
{
    int i = 0;
    printf("%s", message);
    scanf("%d", &i);
    scanf("%*[^\n]");
    return i;
}

// Hilo para mostrar tabla
void *pthreadShowTable(void *arg)
{
    argsFCFS *data = (argsFCFS *)arg;

    // un bucle while que muestra a tabla con los datos
    while (*(data->pcp->showTable))
    {
        // Muestra la tabla
        fflush(stdout);
        toStringProcess(data->pcp);
        fflush(stdout);
        // descansa
        usleep(62000);
        system("clear");
    }
    printf("%s\n", "X: Para cambiar de algoritmo\nM: Mostrar la tabla\nA: Apropiatividad(SOLO RR)\n");
    return NULL;
}

void checkTable(argsFCFS *data)
{
    if (*(data->pcp->showTable))
    {
        *(data->pcp->showTable) = false;
    }
    else
    {
        *(data->pcp->showTable) = true;
    }
}

List *listAging(PCP *pcp)
{
    List *list = createList(5);
    // Inicialización del nodo actual como el primer nodo de la lista de procesos.
    PCB vect[5];
    int i = 0;
    for (Node *current = pcp->Ready->list->firt; current != NULL; current = current->next)
    {
        if (i == 5)
        {
            if (current->myProcess->chargeCicleCpu > vect[0].chargeCicleCpu)
            {
                vect[0] = *current->myProcess;
            }
            else if (current->myProcess->chargeCicleCpu > vect[1].chargeCicleCpu)
            {
                vect[1] = *current->myProcess;
            }
            else if (current->myProcess->chargeCicleCpu > vect[2].chargeCicleCpu)
            {
                vect[2] = *current->myProcess;
            }
            else if (current->myProcess->chargeCicleCpu > vect[3].chargeCicleCpu)
            {
                vect[3] = *current->myProcess;
            }
            else if (current->myProcess->chargeCicleCpu > vect[4].chargeCicleCpu)
            {
                vect[4] = *current->myProcess;
            }
        }
        else if (current->myProcess->chargeCicleCpu > *(pcp->quantum) * 800)
        {
            vect[i] = *current->myProcess;
            i++;
        }
    }
    if (&vect[0] != NULL)
    {
        insertLast(list, &vect[0]);
    }
    if (&vect[1] != NULL)
    {
        insertLast(list, &vect[1]);
    }
    if (&vect[2] != NULL)
    {
        insertLast(list, &vect[2]);
    }
    if (&vect[3] != NULL)
    {
        insertLast(list, &vect[3]);
    }
    if (&vect[4] != NULL)
    {
        insertLast(list, &vect[4]);
    }

    return list;
}

List *mostWasteCPU(PCP *pcp)
{

    List *list = createList(5);
    // Inicialización del nodo actual como el primer nodo de la lista de procesos.

    // Iteración a través de la lista de procesos.
    PCB vect[5];
    int i = 0;

    for (Node *current = pcp->Ready->list->firt; current != NULL; current = current->next)
    {
        if (i == 5)
        {
            if (current->myProcess->chargeCicleCpu < vect[0].chargeCicleCpu)
            {
                vect[0] = *current->myProcess;
            }
            else if (current->myProcess->chargeCicleCpu < vect[1].chargeCicleCpu)
            {
                vect[1] = *current->myProcess;
            }
            else if (current->myProcess->chargeCicleCpu < vect[2].chargeCicleCpu)
            {
                vect[2] = *current->myProcess;
            }
            else if (current->myProcess->chargeCicleCpu < vect[3].chargeCicleCpu)
            {
                vect[3] = *current->myProcess;
            }
            else if (current->myProcess->chargeCicleCpu < vect[4].chargeCicleCpu)
            {
                vect[4] = *current->myProcess;
            }
        }
        else if (current->myProcess->chargeCicleCpu < *(pcp->quantum) * 800)
        {
            vect[i] = *current->myProcess;
            i++;
        }
    }
    if (vect[0].state == 1)
    {
        insertLast(list, &vect[0]);
    }
    else
    {
        insertLast(list, NULL);
    }
    if (vect[1].state == 1)
    {
        insertLast(list, &vect[1]);
    }
    else
    {
        insertLast(list, NULL);
    }
    if (vect[2].state == 1)
    {
        insertLast(list, &vect[2]);
    }
    else
    {
        insertLast(list, NULL);
    }
    if (vect[3].state == 1)
    {
        insertLast(list, &vect[3]);
    }
    else
    {
        insertLast(list, NULL);
    }
    if (vect[4].state == 1)
    {
        insertLast(list, &vect[4]);
    }
    else
    {
        insertLast(list, NULL);
    }

    return list;
}

void *inputThread(void *arg)
{
    argsFCFS *data = (argsFCFS *)arg;
    char c;
    int o = 0;
    // hilo que mostrara la tabla
    pthread_t threadTabla;
    char *message = "X: Para cambiar de algoritmo\nM: Mostrar la tabla\nA: Apropiatividad(SOLO RR)\n";
    char *optionPolicy = "\nPoliticas disponibles:\n1-First Come First Served\n2-Round Robin\n>>>:";

    while (1)
    {
        system("clear");
        c = getChar(message);
        if (c == 'X' || c == 'x')
        {
            o = getInt(optionPolicy);
            scanf("%*[^\n]");
            if (o == 1)
            {
                *(data->pcp->policy) = 1;
            }
            else if (o == 2)
            {
                *(data->pcp->policy) = 2;
                insertQuantum(data->pcp);
            }
            else
            {
                printf("%s", "\nOpcion invalida\n");
            }
        }
        // M
        else if (c == 'M' || c == 'm')
        {
            checkTable(data);
            // TODO aca se debe sacar el metodo
            pthread_create(&threadTabla, NULL, pthreadShowTable, arg);
        }
        else if ((c == 'A' || c == 'a') && *(data->pcp->policy) == 2)
        {
            List *list = mostWasteCPU(data->pcp);
            if (list->firt->myProcess != 0x0)
            {
                printf("%s\n", list->firt->myProcess->PID);
            }
            else
            {
                printf("-------\n");
            }
            if (list->firt->next->myProcess != 0x0)
            {
                printf("%s\n", list->firt->next->myProcess->PID);
            }
            else
            {
                printf("-------\n");
            }
            if (list->firt->next->next->myProcess != 0x0)
            {
                printf("%s\n", list->firt->next->next->myProcess->PID);
            }
            else
            {
                printf("-------\n");
            }
            if (list->firt->next->next->next->myProcess != 0x0)
            {
                printf("%s\n", list->firt->next->next->next->myProcess->PID);
            }
            else
            {
                printf("-------\n");
            }
            if (list->firt->next->next->next->next->myProcess != 0x0)
            {
                printf("%s\n", list->firt->next->next->next->next->myProcess->PID);
            }
            else
            {
                printf("-------\n");
            }

            char *pidAPropiaticidad = getString("Digite el proceso el cual quiera aplicar apropiatividad:\n");

            fflush(stdout);
            printf("%s", pidAPropiaticidad);

            if (data->pcp->Ready->list > 0)
            {
                Node *current = data->pcp->Ready->list->firt;
                Node *prev = NULL;
                while (current != NULL)
                {
                    if (strcmp(current->myProcess->PID, pidAPropiaticidad) == 0)
                    {
                        // Se encontró el proceso con el PID deseado
                        if (prev != NULL)
                        {
                            // Desconectar el nodo del lugar actual en la lista
                            prev->next = current->next;
                        }
                        else
                        {
                            // El nodo a mover ya está al principio de la lista
                            break;
                        }

                        // Conectar el nodo al principio de la lista
                        current->next = data->pcp->Ready->list->firt;
                        data->pcp->Ready->list->firt = current;

                        printf("\nSe ha movido el proceso con el PID %s al principio de la lista.\n", pidAPropiaticidad);
                        break; // Salir del bucle interno ya que se ha movido el proceso
                    }

                    prev = current;
                    current = current->next; // Avanzar al siguiente nodo
                }

                if (current == NULL)
                {
                    // No se encontró ningún proceso con el PID especificado en la lista actual
                    printf("No se encontró ningún proceso con el PID especificado.\n");
                }
                // current->myProcess->priority =1;
                printf("\nse ha aplicado la apropiatividad a el proceso: %s\n", pidAPropiaticidad);
                sleep(1);
            }
            else
            {
                printf("\nno se encuestra ese proceso! \n");
            }
        }
    }

    return NULL;
}

// rutina de listos a ejecucion
void *threadReadyToExecut(void *arg)
{

    argsFCFS *data = (argsFCFS *)arg;
    PCP *pcp = data->pcp;
    // cambio el estado porque va aejecucio
    pcp->Ready->list->firt->myProcess->timeEndReady = addTimeNow();
    changeState(pcp->Ready->list->firt->myProcess, 2);
    // se pone el proceso en ejecucion
    pcp->Execution->processActual = pcp->Ready->list->firt->myProcess;
    // elimino el nodo de la lista
    deleteNode(pcp->Ready->list, pcp->Ready->list->firt);
}

int getPolicy(PCP *pcp)
{

    double readyRatio = pcp->Ready->list->length / 150;
    double ioRatio = pcp->inputOutput->list->length / 150;
    double avgResponse = averageResponseTime(pcp);

    if (readyRatio > 0.7 || ioRatio > 0.5 || avgResponse > 7)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

// Funcion para alternar las politicas de acuerdo a lo que venga por parametro utilizando
void iniciaCicloEjecucion(PCP *pcp, PLP *plp)
{
    // variables globales de ciclo de ejecucion para administrar politicas
    inicializerPCP(pcp);
    inicializerPLP(plp);
    // se va empezar a crear procesos
    pthread_t createFCFS;
    argsFCFS data;
    data.pcp = pcp;
    data.plp = plp;

    // hilo encargado de crear procesos cada cierto tiempo
    if (pthread_create(&createFCFS, NULL, createProcess, (void *)&data) == 1)
    {
        perror("error en el hilo de cracion de procesos");
    }

    // inicializar el proceso de ejecucion
    pcp->Execution->processActual = inicialicerProcess();

    // hilos del contexto de ejecucion
    pthread_t Ready;
    pthread_t ex;
    pthread_t io;

    // aca escucha atravez del hilo
    pthread_t thread;
    if (pthread_create(&thread, NULL, inputThread, (void *)&data) == 1)
    {
        perror("error en el hilo de escuchar los botones");
    }

    // detecta si es la primera vez que va ser usado RR
    bool firstUseRR = true;
    int *normalizeQuantum = (int *)malloc(sizeof(int));

    // para el cambio automatico
    int change = 0;
    //  Mantiene funcionando el ciclo de ejecuccion y se aplicaran los cambio de politica
    while (true)
    {

        // cambio automatico de politica
        if (change == 10)
        {
            *(pcp->policy) = getPolicy(pcp);
        }
        else
        {
            change++;
        }

        // camino si es FCFS
        if (*(pcp->policy) == 1 && pcp->Ready->list->firt != NULL)
        {
            // listos
            pthread_create(&Ready, NULL, threadReadyToExecut, (void *)&data);
            pthread_join(Ready, NULL);
            // ejecucion
            pthread_create(&ex, NULL, excute, (void *)&data);
            pthread_join(ex, NULL);
        }
        else if (*(pcp->policy) == 2 && pcp->Ready->list->firt != NULL)
        {
            // listos
            pthread_create(&Ready, NULL, threadReadyToExecut, (void *)&data);
            pthread_join(Ready, NULL);
            // ejecucion
            pthread_create(&ex, NULL, excute, (void *)&data);
            pthread_join(ex, NULL);
            // normalizacion y balanceo de colas
            normalizeRR(pcp, normalizeQuantum);
        }
    }
}

void inicializerPCP(PCP *pcp)
{
    // inicializamos memoria
    pcp->policy = (int *)malloc(sizeof(int));
    pcp->inputOutput = (IO *)malloc(sizeof(IO));
    pcp->inputOutput->list = createList(150);
    pcp->Execution = (EX *)malloc(sizeof(EX));
    pcp->Ready = (Ready *)malloc(sizeof(Ready));
    pcp->Execution->processActual = inicialicerProcess();
    pcp->showTable = (bool *)malloc(sizeof(bool));
    pcp->pcpAuto = (bool *)malloc(sizeof(bool));
    pcp->quantum = (int *)malloc(sizeof(int));
    pcp->Ready->policy = (int *)malloc(sizeof(int));
    pcp->Ready->list = createList(150);
    // asignamos valores
    *(pcp->policy) = 1;
    *(pcp->pcpAuto) = true;
    *(pcp->quantum) = 1;
    *(pcp->Ready->policy) = 1;
    *(pcp->showTable) = false;
    //inicializador de memoria PMP
    pcp->pmp = (PMP *)malloc(sizeof(PMP)); //
    pcp->pmp->list = (LinkedList *)malloc(sizeof(LinkedList)); //Inicializar lista de PMP
    setBlockSize(pcp->pmp);
}

void inicializerPLP(PLP *plp)
{
    // inicializar memoria
    plp->list = createList(100);
    plp->policy = (int *)malloc(sizeof(int));
    // asigno valores
    *(plp->policy) = 1;
}

#endif