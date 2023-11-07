#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    struct timeval *startTime, *endTime;
    startTime = malloc(sizeof(struct timeval));
    endTime = malloc(sizeof(struct timeval));
    int pipes[2];

    char **args = NULL;
    pipe(pipes);
    
    if (argc > 2)
        args = &argv[2];
    int child = fork();
    if (child) {
        close(pipes[1]);
        read(pipes[0], startTime, sizeof(struct timeval));
        close(pipes[0]);
        // printf("Start time: %ld\n", startTime->tv_usec);
        wait(NULL);
        gettimeofday(endTime, NULL);
        // printf("End time: %ld\n", endTime->tv_usec);
        printf("Time taken: %.5f\n",
               (endTime->tv_usec - startTime->tv_usec) / 1000000.0);
    } else {
        close(pipes[0]);
        gettimeofday(startTime, NULL);
        write(pipes[1], startTime, sizeof(struct timeval));
        close(pipes[1]);
        // printf("Child start time: %ld\n", startTime->tv_usec);
        // printf("Args: ");
        // for (int i = 0; i < argc - 2; i++){ 
        //   printf("%s ", args[i]);
        // }
        // printf("\n");
        execvp(argv[1], args);
    }
    return 0;
}
