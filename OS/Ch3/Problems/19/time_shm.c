#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    struct timeval *startTime, *endTime;

    char *name = "/time";
    int memory;
    char **args = NULL;
    if (argc > 2)
        args = &argv[2];
    memory = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(memory, sizeof(struct timeval));
    startTime = (struct timeval *)mmap(0, sizeof(struct timeval),
                                       PROT_READ | PROT_WRITE, MAP_SHARED,
                                       memory, 0);
    endTime = malloc(sizeof(struct timeval));
    int child = fork();
    if (child) {
        wait(NULL);
        gettimeofday(endTime, NULL);
        // printf("End time: %ld\n", endTime->tv_usec);
        printf("Time taken: %.5f\n",
               (endTime->tv_usec - startTime->tv_usec) / 1000000.0);
    } else {
        gettimeofday(startTime, NULL);
        // printf("Child start time: %ld\n", startTime->tv_usec);
        printf("Args: ");
        for (int i = 0; i < argc - 2; i++){ 
          printf("%s ", args[i]);
        }
        printf("\n");
        execvp(argv[1], args);
    }
    return 0;
}
