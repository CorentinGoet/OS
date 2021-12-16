#define _GNU_SOURCE
#include <sched.h>

#include <pthread.h>           /* produit.c */
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*********** Data Type ***********/

typedef enum
{
    STATE_WAIT,
    STATE_MULT,
    STATE_ADD,
    STATE_PRINT
} State;

typedef struct
{
    State state;
    int * pendingMult;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    size_t nbIterations;
    size_t size;
    double * v1;
    double * v2;
    double * v3;
    double result;
} Product;

/*********** Data structure ***********/

Product prod;

/*********** Function ***********/

void initPendingMult(Product * prod)
{
    size_t i;
    for(i=0;i<prod->size;i++)
    {
        prod->pendingMult[i]=1;
    }
}

int nbPendingMult(Product * prod)
{
    size_t i;
    int nb=0;
    for(i=0;i<prod->size;i++)
    {
        nb+=prod->pendingMult[i];
    }
    return(nb);
}

void wasteTime(unsigned long ms)
{
    unsigned long t,t0;
    struct timeval tv;
    gettimeofday(&tv,(struct timezone *)0);
    t0=tv.tv_sec*1000LU+tv.tv_usec/1000LU;
    do
    {
        gettimeofday(&tv,(struct timezone *)0);
        t=tv.tv_sec*1000LU+tv.tv_usec/1000LU;
    } while(t-t0<ms);
}

/*****************************************************************************/
void * mult(void * data)
{
size_t index;
size_t iter;
int cpu_id;



/* Recuperation de l'index, c'est a dire index = ... */
index = (int) data;

// Assigner à un cpu
    // choix du cpu
    cpu_id = index % sysconf(_SC_NPROCESSORS_ONLN);
    // modification
    cpu_set_t ensemble;
    CPU_ZERO(&ensemble); // On vide l'ensemble
    CPU_SET(cpu_id, &ensemble); // On ajoute le cpu voulu a l ensemble
    printf("CPU objectif : %i\n", cpu_id);

    // On change de cpu
    pthread_setaffinity_np(pthread_self(), sizeof(ensemble), &ensemble);
    printf("Thread de mult n°%i éxécutée par le cpu n°%i\n", index, sched_getcpu());

fprintf(stderr,"Begin mult(%d)\n",index);
                                           /* Tant que toutes les iterations */
for(iter=0;iter<prod.nbIterations;iter++)  /* n'ont pas eu lieu              */
 {
 /* Attendre l'autorisation de multiplication POUR UNE NOUVELLE ITERATION...*/
    // On récupère le mutex
    pthread_mutex_lock(&prod.mutex);

    // On attend
    while(prod.state != STATE_MULT){
        pthread_cond_wait(&prod.cond, &prod.mutex);
    }

    // On rend le mutex
    pthread_mutex_unlock(&prod.mutex);
    

 fprintf(stderr,"--> mult(%d)\n",index); /* La multiplication peut commencer */

 /* Effectuer la multiplication a l'index du thread courant... */
 prod.v3[index] = prod.v1[index] * prod.v2[index];

 wasteTime(200+(rand()%200)); /* Perte du temps avec wasteTime() */

 fprintf(stderr,"<-- mult(%d) : %.3g*%.3g=%.3g\n",           /* Affichage du */
         index,prod.v1[index],prod.v2[index],prod.v3[index]);/* calcul sur   */
                                                             /* l'index      */
 /* Marquer la fin de la multiplication en cours... */

    // On récupère le mutex
    pthread_mutex_lock(&prod.mutex);

    // On passe a l'état d'attente
    prod.state = STATE_WAIT;

    // On note la multiplication comme faite
    prod.pendingMult[index] = 0;

    // On rend le mutex
    pthread_mutex_unlock(&prod.mutex);

 /* Si c'est la derniere... */
    if (nbPendingMult(&prod) == 0)
   {
    /* Autoriser le demarrage de l'addition... */

    // On récupère le mutex
    pthread_mutex_lock(&prod.mutex);

    // On passe a l'état addition
    prod.state = STATE_ADD;

    // On signale aux autres threads que l'état à changé
    pthread_cond_broadcast(&prod.cond);

    // On rend le mutex
    pthread_mutex_unlock(&prod.mutex);

   }
 }
fprintf(stderr,"Quit mult(%d)\n",index);
return(data);
}

/*****************************************************************************/
void * add(void * data)
{
size_t iter;
fprintf(stderr,"Begin add()\n");
                                           /* Tant que toutes les iterations */
for(iter=0;iter<prod.nbIterations;iter++)  /* n'ont pas eu lieu              */
  {
  size_t index;
    initPendingMult(&prod);
  /* Attendre l'autorisation d'addition... */
    pthread_mutex_lock(&prod.mutex);
    while(prod.state != STATE_ADD){
        pthread_cond_wait(&prod.cond, &prod.mutex);
    }
    pthread_mutex_unlock(&prod.mutex);


  fprintf(stderr,"--> add\n"); /* L'addition peut commencer */

  /* Effectuer l'addition... */
  prod.result=0.0;
  for(index=0;index<prod.size;index++)
  {
      prod.result += prod.v3[index];
  }

  wasteTime(100+(rand()%100)); /* Perdre du temps avec wasteTime() */

  fprintf(stderr,"<-- add\n");

  /* Autoriser le demarrage de l'affichage... */
  pthread_mutex_lock(&prod.mutex);
  prod.state = STATE_PRINT;
  pthread_cond_broadcast(&prod.cond);
  pthread_mutex_unlock(&prod.mutex);


  }
fprintf(stderr,"Quit add()\n");
return(data);
}

/*****************************************************************************/
int main(int argc,char ** argv)
{
size_t i, iter;
pthread_t *multTh;
size_t    *multData;
pthread_t  addTh;
void      *threadReturnValue;

/* A cause de warnings lorsque le code n'est pas encore la...*/
 (void)addTh; (void)threadReturnValue;

/* Lire le nombre d'iterations et la taille des vecteurs */

if((argc<=2)||
   (sscanf(argv[1],"%u",&prod.nbIterations)!=1)||
   (sscanf(argv[2],"%u",&prod.size)!=1)||
   ((int)prod.nbIterations<=0)||((int)prod.size<=0))
{
    fprintf(stderr,"usage: %s nbIterations vectorSize\n",argv[0]);
    return(EXIT_FAILURE);
}

/* Initialisations (Product, tableaux, generateur aleatoire,etc) */

prod.state=STATE_WAIT;

prod.pendingMult=(int *)malloc(prod.size*sizeof(int));

/*initialiser prod.mutex ... */
pthread_mutex_init(&(prod.mutex), NULL);

/* initialiser prod.cond ...  */
pthread_cond_init(&(prod.cond), NULL);

/* Allocation dynamique des 3 vecteurs v1, v2, v3 */

prod.v1=(double *)malloc(prod.size*sizeof(double));
prod.v2=(double *)malloc(prod.size*sizeof(double));
prod.v3=(double *)malloc(prod.size*sizeof(double));

/* Allocation dynamique du tableau pour les threads multiplieurs */

multTh=(pthread_t *)malloc(prod.size*sizeof(pthread_t));

/* Allocation dynamique du tableau des MulData */

multData=(size_t *)malloc(prod.size*sizeof(size_t));

/* Initialisation du tableau des MulData */

for(i=0;i<prod.size;i++)
{
 multData[i]=i;
}

/* Creer les threads de multiplication... */
for ( i = 0; i < prod.size; i++){
    if (pthread_create(&multTh[i], NULL, mult, (void *) i) != 0) {
		fprintf(stderr, "Error during pthread_create()\n");
		exit(EXIT_FAILURE);
    }
	
}

printf("Threads de multiplications créés.\n");

/*=>Creer le thread d'addition...          */
i = 0;
if(pthread_create(&addTh, NULL, add, (void *) i) != 0){
    fprintf(stderr, "Error during pthread_create()\n");
    exit(EXIT_FAILURE);
}

printf("Thread d'addition créé.\n");

srand(time((time_t *)0));   /* Init du generateur de nombres aleatoires */

/* Pour chacune des iterations a realiser, c'est a dire :                   */
for(iter=0;iter<prod.nbIterations;iter++) /* tant que toutes les iterations */
  {                                       /* n'ont pas eu lieu              */
  size_t j;
 
  /* Initialiser aleatoirement les deux vecteurs */

  for(j=0;j<prod.size;j++)
    {
    prod.v1[j]=10.0*(0.5-((double)rand())/((double)RAND_MAX));
    prod.v2[j]=10.0*(0.5-((double)rand())/((double)RAND_MAX));
    }

    // Affichage des vecteurs
    printf("Vecteur 1: ");
    for(j = 0; j<prod.size; j++){
        printf("%f ", prod.v1[j]);
    }
    printf("\nVecteur 2: ");
    for(j = 0; j<prod.size; j++){
        printf("%f ", prod.v2[j]);
    }
    printf("\n");

  /* Autoriser le demarrage des multiplications pour une nouvelle iteration..*/

    // On récupère le mutex pour accéder à l'état
    pthread_mutex_lock(&prod.mutex);

    // On modifie l'état
    prod.state = STATE_MULT;

    // Je signale aux threads que j'ai modifié l'état
    pthread_cond_broadcast(&prod.cond);

    // Je rend le mutex
    pthread_mutex_unlock(&prod.mutex);


  /* Attendre l'autorisation d'affichage...*/
    // On reprend le mutex
    pthread_mutex_lock(&prod.mutex);

    while(prod.state != STATE_PRINT){ // Tant qu'on ne doit pas afficher
        // On attend
        pthread_cond_wait(&prod.cond, &prod.mutex);
    }

    // On rend le mutex
    pthread_mutex_unlock(&prod.mutex);


  /* Afficher le resultat de l'iteration courante...*/
    printf("Iteration n° %i ~ resultat : %f\n", iter, prod.result);
  }

printf("Calculs terminés.\n");

/* Attendre la fin des threads de multiplication...*/
for (int i = 0; i < 5; i++){
    pthread_join(multTh[i], NULL);
    
}

printf("Threads de multiplications terminés.\n");

/* Attendre la fin du thread d'addition...*/
pthread_join(addTh, NULL);
printf("Thread d'addition terminé.\n");

/* detruire prod.cond ... */
pthread_cond_destroy(&prod.cond);

/* detruire prod.mutex ... */
pthread_mutex_destroy(&prod.mutex);

/* Detruire avec free ce qui a ete initialise avec malloc */

free(prod.pendingMult);
free(prod.v1);
free(prod.v2);
free(prod.v3);
free(multTh);
free(multData);
return(EXIT_SUCCESS);
}
