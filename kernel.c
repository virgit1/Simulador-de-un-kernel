#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include "MACHINE.h"

#define NUMPROCESOS 100
#define PRIORIDADES 6
#define TIEMPO_EXPIRACION 3
#define TIEMPOMAXPROCESO 25

void start();
void *clock_();
void *timer();
void *load();
void *scheduler();
void borrarDatos();
void mensaje_error();
int crearNumPrioridad();
int crearTiempoVida();
int siguienteProg(char programa[11], char sigProg[11]);

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
Physical *physicalmemory;

int main(int argc, char *argv[])
{
    srand(time(NULL));
    system("chmod u+x print.sh");
    int cpus = 1; int cores = 1; int hilos = 1; int tam_memoria = 24;
    int option;
    //Parseamos los argumentos
    while ((option = getopt(argc, argv, "c::n::t::m::h")) != -1)
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
        case 'm':
            if (!optarg)
            {
                tam_memoria = 24;
            }
            else
            {
                tam_memoria = atoi(optarg);
                if (atoi(optarg)<24) {
                    mensaje_error("Tamaño minimo memoria 2^24");
                } else if (atoi(optarg)>30) {
                    mensaje_error("Tamaño maximo memoria 2^30");
                }
            }
            break;
        case 'h':
            fprintf(stdout, "Uso: %s [-cnth]\n", argv[0]);
            fprintf(stdout, "-c : Numero de cpu's\n");
            fprintf(stdout, "-n : Numero de cores\n");
            fprintf(stdout, "-t : Numero de hilos\n");
            fprintf(stdout, "-m : Tamaño de memoria\n");
            fprintf(stdout, "-h : Help\n");

            return 0;
        default:
            exit(EXIT_FAILURE);
        }
    }

    start(cpus,cores, hilos, tam_memoria);

    id_processGenerator idload;
    id_clock idClock;
    id_timer idTimer;
    id_scheduler idScheduler;
    idClock.c = 0;
    idTimer.t = 0;
    idload.pq = 0;
    idScheduler.s = 0;

    pthread_create(&idload.tid, NULL, load, &idload.pq);
    pthread_create(&idClock.tid, NULL, clock_, &idClock.c);
    pthread_create(&idTimer.tid, NULL, timer, &idTimer.t);
    pthread_create(&idScheduler.tid, NULL, scheduler, &idScheduler.s);
    sleep(100);
    borrarDatos("SIMULADOR DE KERNEL\n");

    return 0;
}

/**************************************************************
 *******************   INICIALIZACION *************************
 *************************************************************/

void start(int cpus, int cores, int hilos, int tam_memoria)
{
    memoria.proceso = 0;
    memoria.timer = 0;
    memoria.sec = 0;
    queuesstruct = crearQueuesStruct(PRIORIDADES, NUMPROCESOS);

    physicalmemory = createMemory(tam_memoria);

    machine = crearMachine(cpus, cores, hilos);
    pthread_mutex_init(&mutexProcesos, NULL);
    pthread_mutex_init(&mutexTimer, NULL);
    pthread_mutex_init(&mutexSec, NULL);
}

/**************************************************************
 ***************   CONTROL DE LA MAQUINA **********************
 *************************************************************/

void *clock_(void *c)
{
    memoria.sec = 0;
    char buf[100];
    while (1)
    {
        sleep(1);
        pthread_mutex_lock(&mutexSec);
        memoria.sec++;
        sprintf(buf, "bash print.sh %d %d", 1, memoria.sec);
        system(buf);

        ejecutarMACHINE(physicalmemory, machine);
        //downTime(machine);

        pthread_mutex_lock(&mutexProcesos);
        memoria.proceso = 1;
        pthread_mutex_unlock(&mutexProcesos);

        pthread_mutex_unlock(&mutexSec);
        //verEstado(machine);
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
            verEstado(machine);
            printMemoryDatos(physicalmemory,0,2);
            printTDPaginas(physicalmemory,10);
            pthread_mutex_unlock(&mutexTimer);
            contador = memoria.sec;
        }
        pthread_mutex_unlock(&mutexSec);
    }
}

void *load(void *pq)
{
    int i;
    int IDpcb = 1;
    char programa[11] = "prog000.elf";
    char sigProg[11];
   
    FILE *f;
    int count_lines = 0;
    printMemory(physicalmemory);
    while (f = fopen(programa,"r")) {
        printf("programa: %s\n", programa);

        int linesText = 0;                  /*Leer programa del archivo*/
        int linesData = 0;
        char text[100];
        char data[100];
        int iniText;
        char iniData[6];
        int iniData2;
        char cadena[100];
        int textArray[LINESMAX];
        int dataArray[LINESMAX];

        int PC = 0;
        fgets(text, 100, f);
        iniText = 0;

        fgets(data,100,f);
        strncpy(iniData, data+6,6);
        iniData2 = (int)strtol(iniData, NULL, 16);

        i = 0;
        int cadenaInt;
        while (PC != iniData2)
        {
            fgets(cadena, 100, f);
            cadenaInt = (int)strtol(cadena, NULL, 16);
            textArray[i] = cadenaInt;
            i++;
            PC +=4;
            linesText++;
        }

        i = 0;
        while (fgets(cadena, 100, f) != NULL)
        {
            cadenaInt = (int)strtol(cadena, NULL, 16);
            dataArray[i] = cadenaInt;
            i++;
            linesData++;
        }

        int numPaginas;
        if ((linesData + linesText)%64 != 0) {
            numPaginas = (linesData + linesText)/64 + 1;
        } else {
            numPaginas = (linesData + linesText)/64;
        }

        /*Escribir en Memoria fisica informacion del programa*/
        int PTBR = escribirDatosMemoria(physicalmemory, textArray, linesText, dataArray, linesData, numPaginas);                       

        fclose(f);
        strcpy(sigProg, "prog");
        if (siguienteProg(programa,sigProg) == 1) break;
        strcpy(programa, sigProg);

        MM *mm = createMM(linesText,linesData,PTBR);
        int prioridad = crearNumPrioridad();
        PCB *pcb = crearPCB(IDpcb,linesText,prioridad,mm,PTBR,numPaginas);

        int sleepSeg = ((rand()%TIEMPO_EXPIRACION) +1);
        pthread_mutex_lock(&mutexProcesos);
        addCola(queuesstruct, pcb);
        memoria.proceso = 1;
        pthread_mutex_unlock(&mutexProcesos);
        IDpcb++;
        sleep(sleepSeg);
        
    }
    printf("Ya no quedan programas para ejecutar\n");
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

/**************************************************************
 ***************** FUNCIONES AUXILIARES ***********************
 *************************************************************/

int crearTiempoVida()
{

    return ((rand() % TIEMPOMAXPROCESO) + 1);
}

int crearNumPrioridad()
{

    return ((rand() % PRIORIDADES) + 1);
}

void mensaje_error(char *s) {
    printf("***error*** %s\n",s);
} // mensaje_error

int siguienteProg(char programa[11], char sigProg[11]) {
    char* num;
    int i;
    num = malloc(sizeof(char)*11);
    for (i=0;i<3;i++) {
        num[i] = programa[i+4];
    }
    int n = atoi(num);
    if (n == 999) return 1;
    n++;
    sprintf(num, "%d", n);
    if (strlen(num) == 1) {
        strcat(sigProg, "00");
    } else if (strlen(num) == 2) {
        strcat(sigProg, "0");
    } 
    strcat(sigProg, num);
    strcat(sigProg, ".elf");
    return 0;
}

void getFileSizes(FILE *f);

/**************************************************************
 ******************** LIBERAR MEMORIA *************************
 *************************************************************/
void borrarDatos()
{
    borrarColas(queuesstruct);
    borrarMachine(machine);
}