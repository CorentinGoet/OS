#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    char buf[80];
    printf("[%d]-- Début du test exec \n", getpid());
    sprintf (buf,"-- pid=[%d]", getpid());
    execl ("/bin/echo","echo","Execution","d'un test exec",buf, NULL);
    // Les lignes après celle-ci ne sont exécutées que si l'appel à execl entraine une erreur.
    printf ("[%d]-- Echec de la fonction execl () \n",getpid());
    printf ("[%d]-- Fin du test exec () \n",getpid());
    exit (0);
}
