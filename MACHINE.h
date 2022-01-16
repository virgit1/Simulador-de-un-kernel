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
                Hilo *hilo = crearHilo(k);
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
        char buf[100];
        sprintf(buf, "bash print.sh %d %d", 5, core->hilos[i].id);
        system(buf);;
        verHilo(&core->hilos[i]);
    }
}

void verCPU(CPU *cpu)
{
    int i;
    char buf[100];
    for (i = 0; i < cpu->numCores; i++)
    {
        sprintf(buf, "bash print.sh %d %d", 4, cpu->cores[i].id);
        system(buf);
        verCore(&cpu->cores[i]);
    }
}

void verEstado(MACHINE *machine)
{
    int i;
    char buf[100];
    sprintf(buf, "bash print.sh %d", 2);
    system(buf);

    for (i = 0; i < machine->numCpus; i++)
    {
        sprintf(buf, "bash print.sh %d %d", 3, machine->cpus[i].id);
        system(buf);
        verCPU(&machine->cpus[i]);
    }
}

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

void ejecutarHilo(Physical *physicalmemory, Hilo *hilo)
{
    if (hilo->free == 0)
    {
        if (ejecutarInstruccionPCB(physicalmemory,hilo->pcb) == 1)
        {
            vaciarMemoriaPCB(physicalmemory,hilo->pcb);
            hilo->pcb = createEmptyPCB();
            hilo->free = 1;
        }
    }
}

void ejecutarCore(Physical *physicalmemory, Core *core)
{
    int i;
    for (i = 0; i < core->numHilos; i++)
    {
        ejecutarHilo(physicalmemory,&core->hilos[i]);
    }
}

void ejecutarCPU(Physical *physicalmemory, CPU *cpu)
{
    int i;
    for (i = 0; i < cpu->numCores; i++)
    {
        ejecutarCore(physicalmemory,&cpu->cores[i]);
    }
}

void ejecutarMACHINE(Physical *physicalmemory, MACHINE *machine)
{
    int i;
    for (i = 0; i < machine->numCpus; i++)
    {
        ejecutarCPU(physicalmemory, &machine->cpus[i]);
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
        hilo->pcb = createEmptyPCB();
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