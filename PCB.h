#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct PCB {

    int pid;
    int tiempoVida;
    int numPrioridad;
    int tiempototal;

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
    char buf[100];
    sprintf(buf, "./print.sh %d %d %d %d %d", 6,  pcb->pid, pcb->tiempoVida, pcb->tiempototal,  pcb->numPrioridad);
    system(buf);
    //printf("PCB = %2d      Tiempo de Vida = %2d de %2d      Prioridad = %2d \n", pcb->pid, pcb->tiempoVida, pcb->tiempototal,  pcb->numPrioridad);
    return;
}