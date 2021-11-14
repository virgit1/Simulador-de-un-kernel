#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "PCB.h"

typedef struct Process_queue
{

    int maxProcesos;
    int tam;
    int primero;
    int ultimo;
    int prioridad;
    PCB *pcbs;

} Process_queue;

typedef struct QueuesStruct
{
    Process_queue *queues;
    int numColas;
} QueuesStruct;

/**************************************************************
 *****************   INICIALIZACION COLAS  ********************
 *************************************************************/

Process_queue *crearQueue(int numProcesos, int prioridad)
{
    Process_queue *q;
    q = (Process_queue *)malloc(sizeof(Process_queue));
    q->pcbs = (PCB *)malloc(sizeof(PCB) * numProcesos * 5);
    q->tam = 0;
    q->maxProcesos = numProcesos;
    q->primero = 0;
    q->ultimo = -1;
    q->prioridad = prioridad;
    return q;
}

QueuesStruct *crearQueuesStruct(int numProcesos, int prioridades)
{
    QueuesStruct *qs;
    qs = (QueuesStruct *)malloc(sizeof(QueuesStruct));
    qs->numColas = prioridades;
    qs->queues = (Process_queue *)malloc(sizeof(QueuesStruct) * prioridades);
    int i;
    for (i = 0; i < prioridades; i++)
    {
        Process_queue *q = crearQueue(numProcesos, prioridades - i);
        qs->queues[i] = *q;
    }
    return qs;
}

/**************************************************************
 *****************   FUNCIONES AUXILIARES  ********************
 *************************************************************/

int colaVacia(Process_queue *q)
{
    if (q->tam == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int quitarPrimero(Process_queue *q)
{
    if (q->tam == 0)
    {
        return 1;
    }
    else
    {
        q->tam--;
        q->primero++;
        return 0;
    }
}

void quitarDeCola(QueuesStruct *qs)
{
    int i;
    for (i = 0; i < qs->numColas; i++)
    {
        if (quitarPrimero(&qs->queues[i]) == 0)
        {
            exit;
        }
    }
}

PCB *primero(Process_queue *q)
{
    if (q->tam == 0)
    {
        printf("Cola Vacia\n");
        return crearPCB(0, 0, 0);
    }
    PCB *pcb = &q->pcbs[q->primero];
    return pcb;
}

PCB *primeroEnCola(QueuesStruct *qs)
{
    int i;
    for (i = 0; i < qs->numColas; i++)
    {
        if (colaVacia(&qs->queues[i]) == 1)
        {
            return primero(&qs->queues[i]);
        }
    }
    return crearPCB(0, 0, 0);
}

void addEnCola(Process_queue *q, PCB *proceso)
{
    if (q->tam != q->maxProcesos)
    {
        q->tam++;
        q->ultimo++;
        q->pcbs[q->ultimo] = *proceso;
    }
    exit;
}

void addCola(QueuesStruct *qs, PCB *proceso)
{
    int i;
    for (i = 0; i < qs->numColas; i++)
    {
        if (proceso->numPrioridad == qs->queues[i].prioridad)
        {
            addEnCola(&qs->queues[i], proceso);
            exit;
        }
    }
}

/**************************************************************
 ********************   BORRAR COLAS  *************************
 *************************************************************/

void borrarCola(Process_queue *q)
{
    free(q);
}

void borrarColas(QueuesStruct *qs)
{
    int i;
    for (i = 0; i < qs->numColas; i++)
    {
        borrarCola(&qs->queues[i]);
    }
    free(qs);
}