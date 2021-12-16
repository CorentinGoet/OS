#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char const *argv[])

{
    int cpu_id;

    // Récupération des arguments
    if((argc != 2) || sscanf(argv[1], "%i", &cpu_id) != 1){
        printf("Usage : %s <(int) CPU id> \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    printf("Je suis executé par le cpu : %i\n", sched_getcpu());
    printf("Je dois m'executer sur le cpu : %i\n", cpu_id);
    
    cpu_set_t ensemble;
    CPU_ZERO(&ensemble); // On vide l'ensemble
    CPU_SET(cpu_id, &ensemble); // On ajoute le cpu voulu a l ensemble

    // On change de cpu
    sched_setaffinity(getpid(), sizeof(ensemble), &ensemble);

    printf("Je suis executé par le cpu : %i\n", sched_getcpu());
    
    return 0;
}
