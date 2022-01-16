#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>    // srand, rand,...
#include <time.h>      // time 
#include <math.h>

#define LINESMAX 1000
#define KERNEL_INI  0x000000
#define KERNEL_FI 0x3FFFFF
#define TDP_INI 0x000000

typedef struct Physical {
    int size;
    int kernelIni;
    int kernelFi;
    int TDPini;
    int TDPfi;
    int *elements;
} Physical;



/*----------------------------------------------------------------- 
 *   Crear memoria Phisical
 *----------------------------------------------------------------*/

Physical * createMemory(int size_bits) {
    Physical *P;
    P = (Physical *)malloc(sizeof(Physical));
    int size = 1 << size_bits;
    P->elements = (int *)malloc(sizeof(int)*size);
    P->size = size;
    P->kernelIni = KERNEL_INI;
    P->kernelFi = KERNEL_FI;
    P->TDPini = TDP_INI;
    P->TDPfi = size/256-1;
    return P;
}

int leerMem(Physical *memoria, int posicion) {
    return memoria->elements[posicion];
}

int estaOcupado(Physical *memoria, int posicion) {
    return memoria->elements[posicion];
}

int ocupar(Physical *memoria, int posicion) {
    memoria->elements[posicion] = 1;
}

int getSize(Physical *memoria) {
    return memoria->size;
}

int getkernelFi(Physical *memoria) {
    return memoria->kernelFi;
}

int getkernelIni(Physical *memoria) {
    return memoria->kernelIni;
}

int getTDPini(Physical *memoria) {
    return memoria->TDPini;
}

int getTDPfi(Physical *memoria) {
    return memoria->TDPfi;
}

void escribirMem(Physical *memoria, int dato, int posicion) {

    memoria->elements[posicion] = dato;
    memoria->elements[posicion+1] = dato;
    memoria->elements[posicion+2] = dato;
    memoria->elements[posicion+3] = dato;

}

int escribirDatosMemoria(Physical *memoria, int textArray[LINESMAX], int linesText, int dataArray[LINESMAX], int linesData, int nPaginas) {
    int i = 0;
    int pagina = 0;
    int paginas = nPaginas;
    int encontrado = 0;
    while (encontrado == 0 && i<memoria->TDPfi) {
        if (estaOcupado(memoria, TDP_INI+i) == 1) {
            i++;
            pagina = i;
        } else {
            paginas--;
            if (paginas == 0) {
                encontrado = 1;
            } else {
                i++;
            }
        }
    }
    if (encontrado = 0) {
        printf("No queda sitio en memoria para el programa");
    } else {

        int j;
        for (j=0;j<nPaginas;j++) {
            ocupar(memoria, pagina+j);
        }

        int k;
        for (k=0;k<linesText;k++) {
            escribirMem(memoria,textArray[k],KERNEL_FI + 1 + (k*4) + (256*pagina));
        }

        int l;
        for (l=0;l<linesData;l++) {
            escribirMem(memoria,dataArray[l],KERNEL_FI + 1 + (l*4) +(256*pagina) + linesText*4);
        }
        return pagina;

    }

}


void printTDPaginas(Physical *memoria, int paginas) {
        int i;
    for (i=memoria->TDPini; i<memoria->TDPini + (paginas); i++) {
        printf("Pagina: %x, Ocupado: %x\n", i, leerMem(memoria, i));
    }
}

void printMemoryDatos(Physical *memoria, int primeraPagina, int nPaginas) {
        int i;
    for (i=primeraPagina; i<nPaginas*256; i+=4) {
        printf("Posicion: %x, Dato: %x\n", memoria->kernelFi + 1 + i , leerMem(memoria, memoria->kernelFi + 1 + i ));
    }
}

void printMemory(Physical *memoria) {
    printf("Tamaño: %x, Inicio Kernel: %x, Final Kernel: %x, Inicio TDP: %x, Final TDP: %x \n", getSize(memoria), getkernelIni(memoria), getkernelFi(memoria), getTDPini(memoria), getTDPfi(memoria));
}