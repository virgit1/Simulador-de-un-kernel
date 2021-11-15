#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct PCB {

    int pid;
    int tiempoVida;
    int tiempototal;
    int numPrioridad;

} PCB;

/**************************************************************
 ********************   INICIALIZACION   **********************
 *************************************************************/

PCB *crearPCB(int id, int tiempoVida, int prioridad){
    PCB *pcb;
    pcb = (PCB*)malloc(sizeof(PCB));
    pcb->pid = id;
    pcb->tiempoVida = tiempoVida;
    pcb->tiempototal = tiempoVida;
    pcb->numPrioridad = prioridad;
    return pcb;
}

/**************************************************************
 ***********   CONTROL DE TIEMPO Y PRIORIDAD PCB  *************
 *************************************************************/

int downTimePCB(PCB *pcb){
    pcb->tiempoVida = pcb->tiempoVida - 1;
    if(pcb->tiempoVida == 0){
        return 1;
    }else{
        return 0;
    }
}

int minusPrioridadPCB(PCB *pcb){
    if(pcb->numPrioridad == 0 || pcb->numPrioridad == 1){
        return 1;
    }else{
        pcb->numPrioridad = pcb->numPrioridad - 1;
        return 0;
    }
}

/**************************************************************
 *******************   VER ESTADO DE PCB  *********************
 *************************************************************/

void verPCB(PCB *pcb){
    printf("PCB = %d    TIEMPO TOTAL = %d     Tiempo de Vida = %d   Prioridad = %d \n", pcb->pid, pcb->tiempototal, pcb->tiempoVida, pcb->numPrioridad);
    return;
}