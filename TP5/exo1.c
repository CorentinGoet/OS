#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char const *argv[])

{
    while(1){
        printf("Coeur n° %i\n", sched_getcpu());
        sleep(0.1);
        break;
    }
    printf("cpus : %i\n", sysconf(_SC_NPROCESSORS_ONLN));
    return 0;
}
