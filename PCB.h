#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct PCB {

    int pid;
    int tiempoVida;
    int numPrioridad;
    
} PCB;