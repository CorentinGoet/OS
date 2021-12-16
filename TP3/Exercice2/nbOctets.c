#include <stdio.h>             /* nbOctets.c */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "partage.h"

/* Decrire le handler de signal pour SIGUSR1 */
/* ========================================= */
/*
* Lorsque le programme du père recoit le signal SIGUSR1, il peut aller lire les données dans la mémoire partagée.
*/
void sig_handler(int sig){
  printf("Signal reçu !\n");
}


/*
* Fils_exec correspond au code exécuté par le fils chargé de la commande wc.
*/
void fils_exec(int tube[2]){
  // Redirection de la sortie standard vers le tube
  close(1);
  dup2(tube[1], STDOUT_FILENO);

  // Fermer les deux descripteurs du tube
  close(tube[0]);  // Le fils ne doit pas lire le tube
  close(tube[1]);  // Après redirection le descripteur d'écriture du tube est inutile. 

  // Exécuter la commande wc
  execlp("wc", "wc","-c", "toto", NULL);
  perror("execlp");

  exit(0);
}


/*
* Fils_read correspond au code exécuté par le fils chargé de la lecture.
*/
void fils_read(int tube[2], FILE *fIn, Zone z){
  int * ptDeb;
  // Fermeture du descripteur d'écriture dans le tube
  close(tube[1]);

  // Ouverture d'un flux vers la sortie du tube
  fIn = fdopen(tube[0],"r");
  if (fIn == NULL) {
        perror("open fIn");
        exit(-1);
      }
  // Lire dans le flux fIn
  int buf;
  fscanf(fIn, "%i", &buf);
  
  // Ecrire dans la mémoire partagée
  printf("Ecriture de %i sur la mémoire partagée\n", buf);
  ptDeb = (int *) z.debut;
  *ptDeb = buf;

  // Fermer le flux fIn et le descripteur de lecture du tube 
  close(fIn);
  close(tube[0]);

  // attendre 3 secondes pour que le père arrive jusqu'à la pause
  sleep(3);

  // envoyer le signal sigusr1 au père
  kill(getppid(), SIGUSR1);
  exit(0);
}


/*
* fin_pere correspond au code exécuté uniquement par le père.
*/
void fin_pere(int tube[2], Zone z, int pidWC, int pidRead, int status){
  /* Fermer les descripteurs de tube inutiles au pere */
  close(tube[0]);
  close(tube[1]);

  /* Attente d'un signal */
  pause();

  /* Recuperer le resultat dans la memoire partagee */
  
  printf("Le fichier contient %i octets\n", *(int *) z.debut);

  /* Attendre le 1er enfant  */
  waitpid(pidWC, &status, 0);

  /* Attendre le 2eme enfant */
  waitpid(pidRead, &status, 0);

  /* Supprimer la memoire partagee */
  supprimerZonePartagee(&z);
}



/* Le main */
/* ======= */

int main(int argc,char **argv)
{
 pid_t  pidWC;
 pid_t  pidREAD;
 int    status;   /* Pour les waitpid                        */

 int    tube[2];
 FILE  *fIn;      /* Pour faire un fdopen : int -> FILE *    */

 struct sigaction action;
 
 Zone     z;      // Déclaration de la mémoire partagée
 int *ptDeb;      /* Un pointeur (int*) sur la zone debut    */

 char  *fileName = NULL;

 if (argc!=2) { 
   // Si le nb d'arguments est incorrect, afficher la syntaxe.
   fprintf(stderr, "Usage: %s fileName\n", argv[0]); 
   return 1; 
   }

 fileName = argv[1];

/* A cause de warnings lorsque le code n'est pas encore la ...*/

 //(void)action; (void)fIn; (void)tube; (void)status; (void)pidREAD; (void)pidWC;

/* Gestion des signaux */

  /* Preparation de la structure action pour recevoir le signal SIGUSR1 */
  action.sa_handler = sig_handler;
  
  /* Appel a l'appel systeme sigaction */
  if(sigaction(SIGUSR1, &action, NULL) == -1){
      printf("Erreur dans la reception du signal.\n");
  }
  

/* Creation de la zone de memoire partagee */
/* ======================================= */

  if(creerZonePartagee(sizeof(int), &z) == -1){
    printf("Erreur dans la création de la mémoire partagée.\n");
  }

 ptDeb = (int*) z.debut;    /* *ptDeb <=> *((int*)z.debut) */

/* Creation du tube */
/* ================ */
  if(pipe(tube) != 0){
    perror("pipe"); 
    exit(EXIT_FAILURE);
  }

/* Creation du processus qui fera le exec ...   */
/* ============================================ */

  pidWC = fork();
  switch (pidWC)
  {
  case 0:
    fils_exec(tube);
    break;
  
  default:
    /* Creation du processus qui fera la lecture ...*/
    /* ============================================ */
    pidREAD = fork();
    switch (pidREAD)
    {
    case 0:
      fils_read(tube, fIn, z);
      break;
    
    default:
      fin_pere(tube, z, pidWC, pidREAD, status);
      break;
    }

    break;
  }


 return 0;
}
