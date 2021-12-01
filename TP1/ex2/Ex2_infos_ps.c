#include <stdio.h>
#include <unistd.h>

int main(){
    // Création du processus fils
    printf("Création du fils.");
    int code_retour = fork();

    // Récupération des infos
    int pid = getpid();
    int ppid = getppid();
    char tab[128];
    getcwd(tab, 128);
    int prop_reel = getuid();
    int groupe_reel = getgid();
    int prop_eff = geteuid();
    int group_eff = getegid();

    // Traitement séparé pour le père et le fils.
    switch (code_retour) {
        case -1:
            printf("Erreur dans la création du fils.");
            break;
        case 0:
            //exécution par le fils
            printf("Je suis le fils \n");
            break;
        default:
            // execution par le pere
            printf("Je suis le pere. \n");
    }
    printf("identifiant : %d \n", pid);
    printf("identifiant du pere : %d \n", ppid);
    printf("répertoire de travail : %s \n", tab);
    printf("propriétaire réel : %d \n", prop_reel);
    printf("propriétaire effectif : %d \n", prop_eff);
    printf("groupe du prop. reel : %d \n", groupe_reel);
    printf("groupe du prop effectif : %d \n", group_eff);


    return 0;
}