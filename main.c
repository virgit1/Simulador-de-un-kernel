#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define pcb * process_queue;

int cpus ,cores, hilos;

int main(int argc, char *argv[]){

int option;
//Parseamos los argumentos
    while ((option = getopt(argc, argv, "c:n:t:h")) != -1)
    {
        switch (option)
        {
            case 'c':
            if(!optarg){
                printf("Hola");
                cpus = 1;
            }else{
                cpus= optarg;
            }
                break;
            case 'n':
             if(!optarg){
                cores = 1;
            }else{
                cores= optarg;
            }
                break;
            case 't':
             if(!optarg){
                hilos = 1;
            }else{
                hilos= optarg;
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

return 0;

}