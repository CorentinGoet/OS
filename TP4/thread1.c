#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *thread_function(void *arg)
{
	while (1) {
		fprintf(stderr, "New thread\n");
		sleep(1);
	}
}


int main (void)
{
	pthread_t thr;

	if (pthread_create(&thr, NULL, thread_function, NULL) != 0) {
		fprintf(stderr, "Error during pthread_create()\n");
		exit(EXIT_FAILURE);
	}
	while (1) {
		fprintf(stderr, "Main thread\n");
		sleep(1);
	}
}