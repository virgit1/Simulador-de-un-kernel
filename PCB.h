#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "Memory.h"

typedef struct MM {
     int code;
     int data;
     int pgb;
 } MM;

typedef struct PCB {

    int pid;
    int tiempoVida;
    int numPrioridad;
    int tiempototal;
    MM *mm;
    int PTBR;
    int nPaginas;
    int PC;

} PCB;

/**************************************************************
 ********************   INICIALIZACION   **********************
 *************************************************************/

MM * createMM(int c,int d, int p) {
    MM *mm;
    mm = (MM *)malloc(sizeof(MM));
    mm->code = c;
    mm->data = d;
    mm->pgb = p;
    return mm;
 }

PCB *crearPCB(int id, int tiempoVida, int prioridad,  MM *mm, int PTBR, int nPaginas){
    PCB *pcb;
    pcb = (PCB*)malloc(sizeof(PCB));
    pcb->pid = id;
    pcb->tiempoVida = tiempoVida;
    pcb->tiempototal = tiempoVida;
    pcb->numPrioridad = prioridad;
    pcb->mm = mm;
    pcb->PTBR = PTBR;
    pcb->PC = 0;
    pcb->nPaginas = nPaginas;
    return pcb;
}

PCB * createEmptyPCB() {
    PCB *pcb;
    pcb = (PCB *)malloc(sizeof(PCB));
    pcb->pid = 0;
    pcb->tiempototal = 0;
    pcb->tiempoVida = 0;
    pcb->numPrioridad = 0;
    MM *mm = createMM(0,0,0);
    pcb->mm = mm;
    pcb->PTBR;
    pcb->PC = 0;
    pcb->nPaginas = 0;
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

/*----------------------------------------------------------------- 
 *   Aumentar PC del PCB en 4
 *----------------------------------------------------------------*/

void aumentarPC(PCB *pcb) {
    pcb->PC = pcb->PC + 4;
}

/*----------------------------------------------------------------- 
 *   Ejecutar instrucciones del PCB
 *----------------------------------------------------------------*/

int ejecutarInstruccionPCB(Physical *memoria, PCB *pcb) {
            //printf("ejecutando PCB");
    int i;
    for (i=0;i<pcb->nPaginas;i++) {
        int instruccion = leerMem(memoria,KERNEL_FI + 1 + pcb->PC + 256*pcb->PTBR + i*256);
        printf("El PCB %d esta ejecutando la instruccion %x\n", pcb->pid, instruccion);
        aumentarPC(pcb);
    }
    
    pcb->tiempoVida = pcb->tiempoVida - 1;
    if (pcb->tiempoVida == 0) {
        return 1;
    } else {
        return 0;
    }
}

void vaciarMemoriaPCB(Physical *memoria,PCB *pcb) {
      int i;
    for (i=0;i<pcb->nPaginas;i++) {
        escribirMem(memoria,0,KERNEL_FI + 1 + pcb->PC + 256*pcb->PTBR + i*256);
        printf("El PCB %d ha terminado y su memoria se ha vaciado\n", pcb->pid);
    }
    int j;
    for (j=0;j<pcb->nPaginas;j++) {
        escribirMem(memoria,0,pcb->PTBR+j);
    }
    
}

/**************************************************************
 *******************   VER ESTADO DE PCB  *********************
 *************************************************************/
void printMM (MM *mm) {
    printf("MM: Code = %d, Data = %d, pgb = %d \n", mm->code, mm->data, mm->pgb);
}

void verPCB(PCB *pcb){
    char buf[100];
    sprintf(buf, "bash print.sh %d %d %d %d %d", 6,  pcb->pid, pcb->tiempoVida, pcb->tiempototal,  pcb->numPrioridad);
    system(buf);
    printMM(pcb->mm);
    return;
}