#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "Process_queue.h"

typedef struct Hilo{

    int id;
    int free;
    PCB *pcb;

}Hilo;

typedef struct Core{

    int id;
    int numHilos;
    Hilo *hilos;

}Core;

typedef struct CPU{

    int id;
    int numCores;
    Core *cores;

} CPU;

typedef struct MACHINE{

    int id;
    int numCpus;
    CPU *cpus;

} MACHINE;


Hilo *crearHilo(int id){

    Hilo *hilo;
    hilo = (Hilo *)malloc(sizeof(Hilo));
    hilo->id = id;
    hilo->pcb = (PCB *)malloc(sizeof(PCB));
    hilo->free = 1;
    return hilo;

}
