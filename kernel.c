#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include "MACHINE.h"

#define NUMPROCESOS 100
#define PRIORIDADES 6
#define TIEMPO_EXPIRACION 3
#define TIEMPOMAXPROCESO 25

void start();
void *clock_();
void *timer();
void *processGenerator();
void *scheduler();
void borrarDatos();
int crearNumPrioridad();
int crearTiempoVida();

typedef struct memoriaCompartida
{
    int proceso;
    int timer;
    int sec;
} memoriaCompartida;

typedef struct 
{
    int pq;
    pthread_t tid;
} id_processGenerator;

typedef struct
{
    int c;
    pthread_t tid;
}id_clock;

typedef struct
{
    int t;
    pthread_t tid;
}id_timer;

typedef struct 
{
    int s;
    pthread_t tid;
}id_scheduler;

pthread_mutex_t mutexProcesos, mutexTimer, mutexSec;
memoriaCompartida memoria;

MACHINE *machine;
QueuesStruct *queuesstruct;

int main(int argc, char *argv[])
{
    srand(time(NULL));
    system("chmod u+x print.sh");
    int cpus = 1; int cores = 1; int hilos = 1;
    int option;
    //Parseamos los argumentos
    while ((option = getopt(argc, argv, "c::n::t::h")) != -1)
    {
        switch (option)
        {
        case 'c':
            if (!optarg)
            {
                cpus = 1;
            }
            else
            {
                cpus = atoi(optarg);
            }
            break;
        case 'n':
            if (!optarg)
            {
                cores = 1;
            }
            else
            {
                cores = atoi(optarg);
            }
            break;
        case 't':
            if (!optarg)
            {
                hilos = 1;
            }
            else
            {
                hilos = atoi(optarg);
            }
            break;
        case 'h':
            fprintf(stdout, "Uso: %s [-cnth]\n", argv[0]);
            fprintf(stdout, "-c : Numero de cpu's\n");
            fprintf(stdout, "-n : Numero de cores\n");
            fprintf(stdout, "-t : Numero de hilos\n");
            fprintf(stdout, "-h : Help\n");

            return 0;
        default:
            exit(EXIT_FAILURE);
        }
    }

    start(cpus, cores, hilos);

    id_processGenerator idProcessGenerator;
    id_clock idClock;
    id_timer idTimer;
    id_scheduler idScheduler;
    idClock.c = 0;
    idTimer.t = 0;
    idProcessGenerator.pq = 0;
    idScheduler.s = 0;

    pthread_create(&idProcessGenerator.tid, NULL, processGenerator, &idProcessGenerator.pq);
    pthread_create(&idClock.tid, NULL, clock_, &idClock.c);
    pthread_create(&idTimer.tid, NULL, timer, &idTimer.t);
    pthread_create(&idScheduler.tid, NULL, scheduler, &idScheduler.s);
    sleep(100);
    borrarDatos("SIMULADOR DE KERNEL\n");

    return 0;
}

void start(int cpus, int cores, int hilos)
{
    memoria.proceso = 0;
    memoria.timer = 0;
    memoria.sec = 0;
    queuesstruct = crearQueuesStruct(PRIORIDADES, NUMPROCESOS);

    machine = crearMachine(cpus, cores, hilos);
    pthread_mutex_init(&mutexProcesos, NULL);
    pthread_mutex_init(&mutexTimer, NULL);
    pthread_mutex_init(&mutexSec, NULL);
}

void *clock_(void *c)
{
    memoria.sec = 0;
    char buf[100];
    while (1)
    {
        sleep(1);
        pthread_mutex_lock(&mutexSec);
        memoria.sec++;
        sprintf(buf, "./print.sh %d %d", 1, memoria.sec);
        system(buf);

        downTime(machine);

        pthread_mutex_lock(&mutexProcesos);
        memoria.proceso = 1;
        pthread_mutex_unlock(&mutexProcesos);

        pthread_mutex_unlock(&mutexSec);
        verEstado(machine);
    }
}

void *timer(void *t)
{
    int contador = 0;
    memoria.timer = 0;
    while (1)
    {
        pthread_mutex_lock(&mutexSec);
        if (memoria.sec == contador + TIEMPO_EXPIRACION)
        {
            pthread_mutex_lock(&mutexTimer);
            memoria.timer = 1;
            pthread_mutex_unlock(&mutexTimer);
            contador = memoria.sec;
        }
        pthread_mutex_unlock(&mutexSec);
    }
}

void *processGenerator(void *pq)
{
    int i;
    for (i = 1; i <= NUMPROCESOS; i++)
    {

        PCB *pcb = crearPCB(i, crearTiempoVida(), crearNumPrioridad());
        pthread_mutex_lock(&mutexProcesos);
        addCola(queuesstruct, pcb);
        memoria.proceso = 1;
        pthread_mutex_unlock(&mutexProcesos);
        if (i % 10 == 0)
        {
            sleep(15);
        }
    }
}

void *scheduler(void *s)
{
    int i;
    while (1)
    {
        pthread_mutex_lock(&mutexProcesos);
        if (memoria.proceso == 1)
        {
            while (insertarPCB(machine, primeroEnCola(queuesstruct)) == 0)
            {
                quitarDeCola(queuesstruct);
            }
            memoria.proceso = 0;
        }
        pthread_mutex_unlock(&mutexProcesos);

        pthread_mutex_lock(&mutexTimer);
        if (memoria.timer == 1)
        {
            update(queuesstruct, machine);
            while (insertarPCB(machine, primeroEnCola(queuesstruct)) == 0)
            {

                quitarDeCola(queuesstruct);
            }
            memoria.timer = 0;
        }
        pthread_mutex_unlock(&mutexTimer);
    }
}

int crearTiempoVida()
{

    return ((rand() % TIEMPOMAXPROCESO) + 1);
}

int crearNumPrioridad()
{

    return ((rand() % PRIORIDADES) + 1);
}

void borrarDatos()
{
    borrarColas(queuesstruct);
    borrarMachine(machine);
}