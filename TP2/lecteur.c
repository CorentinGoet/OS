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

/* Récupère le nombre de lecteurs */
int get_nb_lecteurs(){
    FILE *f = fopen("nombre", "r");
    int nb;
    if(fscanf(f, "%i", &nb) < 0){
        printf("Problème de lecture du nombre de lecteurs.\n");
        exit(-1);
    }
    fclose(f);

    return nb;
}

/* Attribue une valeur au nombre de lecteurs */
int set_nb_lecteurs(int nb){
    FILE *f = fopen("nombre", "w");
    if(fprintf(f, "%i", nb) < 0){
        printf("Problème d'écriture du nombre de lecteurs.\n");
        exit(-1);
    }
    fclose(f);

    return 0;
}

/* Incrémente le nombre de lecteurs */
int incr_nb_lecteur(){
    set_nb_lecteurs(get_nb_lecteurs() + 1);
    return 0;
}
/* Décrémente le nombre de lecteurs */
int decr_nb_lecteur(){
    //printf("decr : nb = %i\n", get_nb_lecteurs());
    set_nb_lecteurs(get_nb_lecteurs() - 1);
    //printf("decr : nb = %i\n", get_nb_lecteurs());
    return 0;
}

int lire(char *filename, char *buf){
    FILE *f = fopen(filename, "r");
    if(fscanf(f, "%s", buf) < 0){
        printf("Erreur de lecture.\n");
        exit(-1);
    }
    fclose(f);
    return 0;
}



int main(){

    key_t k;
    int semid;
    char mot[200];
    int num;
    // Récupérer la clé
    k = ftok("exo2_data.txt", PRJVAL);
    if(k == -1) perror("ftok");

    // Récupérer les sémaphores
    semid = semget(k, 0, 0);

    // On prend le sémaphore mutex_l
    printf("Lecteur: en attente du semaphore mutex_l.\n");
    if(P(semid, 1) == -1){
        printf("Problème fonction P.\n");
        exit(-1);
    }

    incr_nb_lecteur();

    num = get_nb_lecteurs();


    if(get_nb_lecteurs() == 1){
        // On prend le sémaphore de données
        printf("Premier lecteur en attente du sem de données.\n");

        if(P(semid, 0) == -1){
            printf("Problème fonction P.\n");
            exit(-1);
        }
    }

    // On libère le sémaphore mutex_l
    if(V(semid, 1) == -1){
        printf("Problème fonction V.\n");
        exit(-1);
    }

    // Lire le fichier
    lire("exo2_data.txt", mot);
    printf("Le mot %s a été lu.\n", mot);

    sleep(10);
    printf("Lecteur %i: lecture terminée.\n", num);

    // On prend le sémaphore mutex_l
    if(P(semid, 1) == -1){
        printf("Problème fonction P.\n");
        exit(-1);
    }

    decr_nb_lecteur();

    if(get_nb_lecteurs() == 0){
        // On libère le semaphore de données
        if(V(semid, 0) == -1){
            printf("Problème fonction V.\n");
            exit(-1);
        }
        printf("libération du sémaphore de données.\n");
    }

    // On libère le semaphore mutex_l
    if(V(semid, 1) == -1){
            printf("Problème fonction V.\n");
            exit(-1);
        }
    

    return 0;
}