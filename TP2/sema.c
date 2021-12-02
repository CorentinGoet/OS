#include <sys/types.h>     /* key_t  */
#include <sys/ipc.h>       /* ftok   */
#include <sys/sem.h>       /* semget, semctl, semop */
#include <sys/stat.h>      /* stat, struct stat  */
#include <stdlib.h>        /* malloc */
#include <stdio.h>         /* perror, printf */
#include <errno.h>         /* errno */

#define PRJVAL  1

int main(int argc,char ** argv)
{
	 int nbSem, val;
	 int semid;
	 key_t k;
	 int s;
	 unsigned short *values;
	 struct stat buf;
	 
	 // le programme prend 3 arguments: le nom du fichier pour cr�er la cl�, le nombre de s�maphore ainsi que la valeur initiale
	 if (argc!=4) { fprintf(stderr,"Usage: %s nomFichier nbSem val\n",argv[0]);
					return 1;
	 }

	 if (stat(argv[1],&buf)==-1) {//le nom de fichier
	   fprintf(stderr,"%s n'est pas une reference!\n",argv[1]);
	   return 1;
	 }

	 if (sscanf(argv[2],"%d",&nbSem)!=1) {// le nombre de semaphore cr�es
	   fprintf(stderr,"%s n'est pas un entier!\n",argv[2]);
	   return 1;
	 }
	 if (sscanf(argv[3],"%d",&val)!=1) {// la valeur initiale des s�maphores
	   fprintf(stderr,"%s n'est pas un entier!\n",argv[3]);
	   return 1;
	 }
	 
	 
	 // Q: rajouter ici l'appel � la fonction ftok(), la variable k contiendra la cl� extraite (retour de la fonction ftok) 
	 // on pourra utiliser PRJVAL comme second param�tre de la fonction ftok
	 // --
	 
	 if (k==-1) perror("ftok");

	 printf("cle %d (dec) %x (hex) inode %x\n", k,k,(int)buf.st_ino);
	 
	 // Q: appel � la fonction semget() et mise de la valeur de retour dans la variable semid
	 //--
	 
	 if (semid==-1)
	 {
	  fprintf(stderr,"Erreur lors de la creation\n");
	  return 1;
	 }
	 printf("semid obtenu:%d\n",semid);
	 
	 // initialisation des semaphores
	 values=(unsigned short*)malloc(sizeof(unsigned short)*nbSem); //autant d'�l�ments dans le tableau que le nombre de semaphores
	 if (values==NULL)
	 {
		exit(-1);
	 }
	
	// le tableau values contient l'ensemble des valeur avec lesquels on veut initialiser les s�maphores	
	for (s=0; s<nbSem; s++) values[s]=val;
	
	
	// Q: faites un appel � la fonction semclt() pour initialiser les s�maphore avec les valeurs se trouvant dans le tableau values
	// utilisez l'op�ration SETALL
	// --
	free(values);
		 

	return 0;
}