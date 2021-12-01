#include <stdio.h>
#include <unistd.h>

int main ()  {
    int code_retour;
    printf ("Debut du test fork ()\n");
    code_retour = fork (); //Crée un processus fils. Les deux processus vont exécuter la suite du programme.
    switch (code_retour) {
        case -1 :
            printf ("Pbm lors de la creation du processus\n");
            break;
        case 0 :
            // Le code retour du fils est 0. Il execute donc cette partie.
            printf ("Je suis le processus fils \n");
            break;
        default:
            // Le père a un code retour positif il exécute cette partie.
            printf ("Je suis le processus père\n");
            printf ("Je viens de créer le processus fils dont le pid est %d \n",code_retour);
    }
    // Cette partie est exécutée par les deux processus.
    printf ("code_retour %d\n", code_retour);
    printf ("Fin du testfork ()\n");
    return 0;
}