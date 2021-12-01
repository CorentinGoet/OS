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
            printf("Le premier fils a été créé.\n");
            printf("Le premier fils s'endort.\n");
            sleep(5);
            printf("Le premier fils s'arrête. \n");
            exit(1);
            break;
        default:
            // Le père a un code retour positif il exécute cette partie.
            code_retour = fork();   // crée le 2e fils
            switch (code_retour)
            {
            case 0:
                // Code exécuté par le 2e fils.
                printf("Le second fils a été créé. \n");
                printf("Le second fils s'endort. \n");
                sleep(10);
                printf("Le second fils s'arrête.\n");
                exit(2);
                break;
            
            default:
                wait(&code_erreur_fils);
                wait(&code_erreur_fils);
                if (WIFEXITED(code_erreur_fils) != 0){
                    printf("le fils s'est terminé avec le statut : %d \n", WEXITSTATUS(code_erreur_fils));
                }
                if (WIFSIGNALED(code_erreur_fils) != 0){
                    printf("Le signal ayant terminé le fils est : %d \n", WTERMSIG(code_erreur_fils));
                }
                break;
            }            
	    }	
    // Cette partie est exécutée par les deux processus.

    return 0;
}
