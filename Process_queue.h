#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "PCB.h"

typedef struct Process_queue{

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
}QueuesStruct;
