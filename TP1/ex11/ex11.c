#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/wait.h>

int main()
{
    char commande[128]; // Nombre de caractères arbitraire
    int code_fils;
    char *argv[5];
    
    // Récupération de la commande à exécuter
    printf("Entrez une commande :\n");
    scanf("%s", commande);
    printf("test %s", commande);
    
    
    return 0;
}
