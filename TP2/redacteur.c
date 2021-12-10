#include <sys/types.h>     /* key_t  */
#include <sys/ipc.h>       /* ftok   */
#include <sys/sem.h>       /* semget, semctl, semop */
#include <sys/stat.h>      /* stat, struct stat  */
#include <fcntl.h>         /* file */
#include <stdlib.h>        /* malloc */
#include <stdio.h>         /* perror, printf */
#include <errno.h>         /* errno */
#include <unistd.h> 	   /*sleep*/

#define PRJVAL 1


int P(int semid, int noSem)
{
	struct sembuf Ops[1];
	int ok;

	Ops[0].sem_num = noSem;
    Ops[0].sem_op = -1;
    Ops[0].sem_flg = 0;

    ok = semop(semid, Ops, 1);
	if(ok == -1) perror("semop");
	return ok;
}

/* retourne -1 en cas d'erreur           */
int V(int semid, int noSem)
{
	struct sembuf Ops[1];
	int ok;
	
	Ops[0].sem_num = noSem;
    Ops[0].sem_op = 1;
    Ops[0].sem_flg = 0;
    
    ok = semop(semid, Ops, 1);
	if(ok == -1) perror("semop");
	return ok;
}

/* Ecrit dans le fichier demandé*/
int ecrire(char *nom){

    FILE *f = fopen(nom, "r+");
    if(fprintf(f, "Je suis le rédacteur\n") < 0) return -1;
    fclose(f);

    return 0;
}

int main(){

    key_t k;
    int semid;
    char buf[200];

    // Récupérer la clé
    k = ftok("exo2_data.txt", PRJVAL);
    if(k == -1) perror("ftok");

    // Récupérer les sémaphores
    semid = semget(k, 0, 0);
    printf("Rédacteur: sémaphores récupérés.\n");

    // On prend le sémaphore de données
    printf("Rédacteur en attente du sémaphore de données.\n");
    if(P(semid, 0) == -1){
        printf("Problème fonction P.\n");
        exit(-1);
    }

    // Ecriture dans le fichier
    if(ecrire("exo2_data.txt") == -1){
        printf("Erreur fonction ecrire\n");
        exit(-1);
    }

    printf("Rédacteur : écriture terminée, début de sommeil\n");

    sleep(5);

    // Libérer le semaphore de données
    if(V(semid, 0) == -1){
        printf("Erreur fonction V.\n");
        exit(-1);
    }

    printf("Rédacteur: sémaphore libéré\n");

    return 0;
}
