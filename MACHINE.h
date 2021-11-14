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

    int numCpus;
    CPU *cpus;

} MACHINE;

/**************************************************************
 ****************   CREAR LAS STRUCTURAS    *******************
 *************************************************************/
 

Hilo *crearHilo(int id){

    Hilo *hilo;
    hilo = (Hilo *)malloc(sizeof(Hilo));
    hilo->id = id;
    hilo->pcb = (PCB *)malloc(sizeof(PCB));
    hilo->free = 1;
    return hilo;

}

Core *crearCore(int id, int numHilos){

    Core *core;
    core = (Core*)malloc(sizeof(Core));
    core->id = id;
    core->numHilos = numHilos;
    core->hilos = (Hilo *)malloc(sizeof(Hilo)*numHilos);
    return core; 

}

CPU *crearCPU(int id, int cores){

    CPU *cpu;
    cpu = (CPU*)malloc(sizeof(CPU));
    cpu->id = id;
    cpu->numCores = cores;
    cpu->cores = (Core *)malloc(sizeof(Core)*cores);
    return cpu;
}

MACHINE *crearMachine(int cpus, int cores, int hilos){

    MACHINE *machine;
    machine = (MACHINE*)malloc(sizeof(MACHINE));
    machine->numCpus = cpus;
    machine->cpus = (CPU*)malloc(sizeof(CPU)*cpus);

    int i, j, k;
    for(i= 0; i<cpus; i++){
        CPU *cpus = crearCPU(i, cores); 
        for(j = 0; j<cores; cores++){
            Core *cores = crearCore(j, hilos);
            for(k = 0; k<hilos; k++){
                Hilo *hilos = crearHilo(k);
                cores->hilos[k] = *hilos;
            }
            cpus->cores[j] = *cores;
        }
        machine->cpus[i] = *cpus;
    }
    return machine;

}

/**************************************************************
 *************   VER ESTADO DE LA ESTRUCTURA   ****************
 *************************************************************/

void verHilo(Hilo *hilo){
    if(hilo->free == 0){
        verPCB(hilo->pcb);
    }else {
        printf("Libre \n");
    }
}

void verCore(Core *core){
    int i;
    for(i = 0; i<core->numHilos; i++){
        printf("Hilo %d: ", core->hilos[i].id);
        verHilo(&core->hilos[i]);

    }
}

void verCPU(CPU *cpu){
    int i;
    for(i = 0; i<cpu->numCores; i++){
        printf("Core %d:  \n", cpu->cores[i].id);
        verCore(&cpu->cores[i]);
    }
}

void VerEstado(MACHINE *machine){
    int i;
    printf("MACHINE: \n");
    for(i = 0; i<machine->numCpus; i++){
        printf("Cpu %d:  \n", machine->cpus[i].id);
        verCPU(&machine->cpus[i]);
    }

}

/**************************************************************
 *************   VER ESTADO DE LA ESTRUCTURA   ****************
 *************************************************************/

int insertarPCBenHilo(Hilo *hilo, PCB *pcb){
    if(hilo->free == 1){
        hilo->pcb = pcb;
        hilo->free = 0;
        return 1;
    } else {
        return 0;
    }
}

int insertarPCBenCore(Core *core, PCB *pcb){
    int i;
    for(i=0; i<core->numHilos; i++){
        if(insertarPCBenHilo(&core->hilos[i], pcb) == 0){
            return 1;
        }
    }
    return 0;
}

int insertarPCBenCPU(CPU *cpu, PCB *pcb){
    int i;
    for(i=0; i<cpu->numCores; i++){
        if(insertarPCBenHilo(&cpu->cores[i], pcb) == 0){
            return 1;
        }
    }
    return 0;
}

int insertarPCB(MACHINE *machine, PCB *pcb){
    int i;
    if(pcb->pid != 0){
        for(i = 0; i<machine->numCpus; i++){
            if(insertarPCBenCPU(&machine->cpus[i], pcb) == 0){
                return 1;
            }
        }
    }
    return 0;
}

/**************************************************************
 **********   CONTROL DEL TIEMPO EN LOS PROCESOS  *************
 *************************************************************/

void downTimeHilo(Hilo *hilo){
    if(hilo->free == 0){
        if(downTimePCB(hilo->pcb) == 1){
            hilo->pcb = crearPCB(0,0,0);
            hilo->free = 1;
        }
    }
}

void downTimeCore(Core *core){
    int i;
    for(i = 0; i<core->numHilos; i++){
        downTimeHilo(&core->hilos[i]);
    }
}


void downTimeCPU(CPU *cpu){
    int i;
    for(i = 0; i<cpu->numCores; i++){
        downTimeoHilo(&cpu->cores[i]);
    }
}

void downTime(MACHINE *machine){
    int i;
    for(i = 0; i<machine->numCpus; i++){
        downTimeCPU(&machine->cpus[i]);
    }
}