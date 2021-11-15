#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "Process_queue.h"

typedef struct Hilo
{

    int id;
    int free;
    PCB *pcb;

} Hilo;

typedef struct Core
{

    int id;
    int numHilos;
    Hilo *hilos;

} Core;

typedef struct CPU
{

    int id;
    int numCores;
    Core *cores;

} CPU;

typedef struct MACHINE
{

    int numCpus;
    CPU *cpus;

} MACHINE;

/**************************************************************
 ***************   INICIALIZACION ESTRUCTURAS  ****************
 *************************************************************/

Hilo *crearHilo(int id)
{

    Hilo *hilo;
    hilo = (Hilo *)malloc(sizeof(Hilo));
    hilo->id = id;
    hilo->pcb = (PCB *)malloc(sizeof(PCB));
    hilo->free = 1;
    return hilo;
}

Core *crearCore(int id, int numHilos)
{

    Core *core;
    core = (Core *)malloc(sizeof(Core));
    core->id = id;
    core->numHilos = numHilos;
    core->hilos = (Hilo *)malloc(sizeof(Hilo) * numHilos);
    return core;
}

CPU *crearCPU(int id, int cores)
{

    CPU *cpu;
    cpu = (CPU *)malloc(sizeof(CPU));
    cpu->id = id;
    cpu->numCores = cores;
    cpu->cores = (Core *)malloc(sizeof(Core) * cores);
    return cpu;
}

MACHINE *crearMachine(int cpus, int cores, int hilos)
{

    MACHINE *machine;
    machine = (MACHINE *)malloc(sizeof(MACHINE));
    machine->numCpus = cpus;
    machine->cpus = (CPU *)malloc(sizeof(CPU) * cpus);

    int i, j, k;
    for (i = 0; i < cpus; i++)
    {
        CPU *cpu = crearCPU(i, cores);
        for (j = 0; j < cores; j++)
        {
            Core *core = crearCore(j, hilos);
            for (k = 0; k < hilos; k++)
            {
                Hilo *hilo = crearHilo(k + j*hilos);
                core->hilos[k] = *hilo;
            }
            cpu->cores[j] = *core;
        }
        machine->cpus[i] = *cpu;
    }
    return machine;
}

/**************************************************************
 *************   VER ESTADO DE LA ESTRUCTURA   ****************
 *************************************************************/

void verHilo(Hilo *hilo)
{
    if (hilo->free == 0)
    {
        verPCB(hilo->pcb);
    }
    else
    {
        printf("Libre \n");
    }
}

void verCore(Core *core)
{
    int i;
    for (i = 0; i < core->numHilos; i++)
    {
        printf("                Hilo %d: ", core->hilos[i].id);
        verHilo(&core->hilos[i]);
    }
}

void verCPU(CPU *cpu)
{
    int i;
    for (i = 0; i < cpu->numCores; i++)
    {
        printf("            Core %d:  \n", cpu->cores[i].id);
        verCore(&cpu->cores[i]);
    }
}

void verEstado(MACHINE *machine)
{
    int i;
    printf("    MACHINE: \n");
    for (i = 0; i < machine->numCpus; i++)
    {
        printf("        CPU %d:  \n", machine->cpus[i].id);
        verCPU(&machine->cpus[i]);
    }
}

/**************************************************************
 *************   VER ESTADO DE LA ESTRUCTURA   ****************
 *************************************************************/

int insertarPCBenHilo(Hilo *hilo, PCB *pcb)
{
    if (hilo->free == 1)
    {
        hilo->pcb = pcb;
        hilo->free = 0;
        return 0;
    }
    else
    {
        return 1;
    }
}

int insertarPCBenCore(Core *core, PCB *pcb)
{
    int i;
    for (i = 0; i < core->numHilos; i++)
    {
        if (insertarPCBenHilo(&core->hilos[i], pcb) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int insertarPCBenCPU(CPU *cpu, PCB *pcb)
{
    int i;
    for (i = 0; i < cpu->numCores; i++)
    {
        if (insertarPCBenCore(&cpu->cores[i], pcb) == 0)
        {
            return 0;
        }
    }
    return 1;
}

int insertarPCB(MACHINE *machine, PCB *pcb)
{
    int i;
    if (pcb->pid != 0)
    {
        for (i = 0; i < machine->numCpus; i++)
        {
            if (insertarPCBenCPU(&machine->cpus[i], pcb) == 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

/**************************************************************
 **********   CONTROL DEL TIEMPO EN LOS PROCESOS  *************
 *************************************************************/

void downTimeHilo(Hilo *hilo)
{
    if (hilo->free == 0)
    {
        if (downTimePCB(hilo->pcb) == 1)
        {
            hilo->pcb = crearPCB(0, 0, 0);
            hilo->free = 1;
        }
    }
}

void downTimeCore(Core *core)
{
    int i;
    for (i = 0; i < core->numHilos; i++)
    {
        downTimeHilo(&core->hilos[i]);
    }
}

void downTimeCPU(CPU *cpu)
{
    int i;
    for (i = 0; i < cpu->numCores; i++)
    {
        downTimeCore(&cpu->cores[i]);
    }
}

void downTime(MACHINE *machine)
{
    int i;
    for (i = 0; i < machine->numCpus; i++)
    {
        downTimeCPU(&machine->cpus[i]);
    }
}

/**************************************************************
 *********   CONTROL DE LOS PROCESOS EN LAS COLAS  ************
 *************************************************************/

void updateHilo(QueuesStruct *qs, Hilo *hilo)
{

    if (hilo->free == 0)
    {
        minusPrioridadPCB(hilo->pcb);
        addCola(qs, hilo->pcb);
        hilo->pcb = crearPCB(0, 0, 0);
        hilo->free = 1;
    }
}

void updateCore(QueuesStruct *qs, Core *core)
{
    int i;
    for (i = 0; i < core->numHilos; i++)
    {
        updateHilo(qs, &core->hilos[i]);
    }
}

void updateCPU(QueuesStruct *qs, CPU *cpu)
{
    int i;
    for (i = 0; i < cpu->numCores; i++)
    {
        updateCore(qs, &cpu->cores[i]);
    }
}

void update(QueuesStruct *qs, MACHINE *machine)
{
    int i;
    for (i = 0; i < machine->numCpus; i++)
    {
        updateCPU(qs, &machine->cpus[i]);
    }
}

/**************************************************************
 ******************   BORRAR LA MEMORIA  **********************
 *************************************************************/

void borrarHilo(Hilo *hilo)
{
    free(hilo);
}

void borrarCore(Core *core)
{
    int i;
    for (i = 0; i < core->numHilos; i++)
    {
        borrarHilo(&core->hilos[i]);
    }
    free(core);
}

void borrarCPU(CPU *cpu)
{
    int i;
    for (i = 0; i < cpu->numCores; i++)
    {
        borrarCore(&cpu->cores[i]);
    }
    free(cpu);
}

void borrarMachine(MACHINE *machine)
{
    int i;
    for (i = 0; i < machine->numCpus; i++)
    {
        borrarCPU(&machine->cpus[i]);
    }
    free(machine);
}