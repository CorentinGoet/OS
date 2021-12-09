#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
 
void *thread_function(void *arg) 
{
    int i = *(int *) arg; 
    // fprintf(stderr, "New thread n°%i\n", *(int *) arg);
    // Dans cette version les threads affichent uniquement l'indice du dernier passage dans la boucle.
    //fprintf(stderr, "New thread n°%i\n", i); 
    // les threads partagent la même variable i
    fprintf(stderr, "New thread n°%i\n", i);
    sleep(1); 
    // exit(0);
    // coupe l'execution de tout le processus
    int *res = (int*) malloc(sizeof(int));
    *res = i * i;
    pthread_exit((void *) res);// termine uniquement la thread
} 
 
 
int main (void) 
{ 
    pthread_t thr[5]; 
    int tab_i[5];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    for(int i = 0; i < 5; i++){
        
        tab_i[i] = i;
        if (pthread_create(&thr[i], NULL, thread_function, (void *) &tab_i[i]) != 0) { 
        fprintf(stderr, "Error during pthread_create()\n"); 
        exit(EXIT_FAILURE); 
        } 
    }
    sleep(5);
    
    fprintf(stderr, "Main thread\n");
    void *ret;
    for(int i = 0; i < 5; i ++){
        pthread_join(thr[i], &ret);
        int res = *(int*) ret;
        fprintf(stderr, "Retour thread %i : %i\n", i, res);
    }
    
    
}