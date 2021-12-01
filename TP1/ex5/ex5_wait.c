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
    
    file_code = open("toto.txt", O_RDWR);
    code_retour = fork (); //Crée un processus fils. Les deux processus vont exécuter la suite du programme.
    switch (code_retour) {
        case -1 :
            printf ("Pbm lors de la creation du processus\n");
            break;
        case 0 :
            // Le code retour du fils est 0. Il execute donc cette partie.

            write(file_code, "fils", 4);
            sleep(2);
            printf("lecture par le fils:");
            read(file_code, mot, 4);
            printf("%s\n", mot);
            close(file_code);
            exit(0);
            break;
        default:
            // Le père a un code retour positif il exécute cette partie.
            sleep(1);
            printf("Lecture par le père:");
            read(file_code, mot, 4);
            printf("%s\n", mot);
            write(file_code, "pere", 4);
            close(file_code);
            wait(&code_erreur_fils);
            printf("Fin du processus fils avec le code %i\n", code_erreur_fils);
    }
    // Cette partie est exécutée par les deux processus.

    return 0;
}
