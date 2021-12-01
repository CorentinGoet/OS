#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/wait.h>

int main()
{
    int code_fils;
    int code_retour;
    char *argv[5];
    code_fils = fork(); // création du fils

    switch (code_fils)
    {
    case 0:
        // Partie exécutée par le fils.
        
        // exécution de la commande ps
        //execl("/bin/ps", "ps", "-l", NULL);     // execl -> arguments donnés en paramètres les uns après les autres.
        argv[0] = "ps";
        argv[1] = "-l";
        argv[2] = NULL;
        execv("/bin/ps", argv);
        printf("Erreur dans l'exécution de la commande ps.\n");
        break;
    
    default:
        // Partie exécutée par le père.
        wait(&code_retour);
        printf("Le programme s'est bien terminé.\n");
        break;
    }
    return 0;
}