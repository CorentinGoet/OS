#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/wait.h> 




int main ()  {
    int code_retour;
    int file_code;
    char mot[128];
    int code_erreur_fils;
    int pid;
    
    code_retour = fork (); //Crée un processus fils. Les deux processus vont exécuter la suite du programme.
    switch (code_retour) {
        case -1 :
            printf ("Pbm lors de la creation du processus\n");
            break;
        case 0 :
            // Le code retour du fils est 0. Il execute donc cette partie.
            sleep(5);
	    printf("Je suis le premier fils\n");
            printf("Le pid du premier fils est: %d\n", getpid());
            exit(1);
            break;
        default:
            // Le père a un code retour positif il exécute cette partie.
            
            
            wait(&code_erreur_fils);
            if (WIFEXITED(code_erreur_fils) != 0){
            	printf("le fils s'est terminé avec le statut : %d \n", WEXITSTATUS(code_erreur_fils));
            }
            if (WIFSIGNALED(code_erreur_fils) != 0){
            	printf("Le signal ayant terminé le fils est : %d \n", WTERMSIG(code_erreur_fils));
	    }	
    }
    // Cette partie est exécutée par les deux processus.

    return 0;
}
