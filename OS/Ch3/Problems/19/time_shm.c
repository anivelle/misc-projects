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
    struct timeval *ptr;
    memory = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(memory, sizeof(struct timeval));
    int child = fork();
    printf("%d\n", child);
        wait(NULL);
    if (child) {
        ptr = (struct timeval *)mmap(0, sizeof(struct timeval), PROT_READ | PROT_WRITE,
                           MAP_SHARED, memory, 0);
        startTime = ptr;
        gettimeofday(endTime, NULL);
        printf ("Start time: %ld\n", startTime->tv_usec);
        printf ("End time: %ld\n", endTime->tv_usec);

    } else {
        // ptr = (struct timeval *)mmap(0, sizeof(struct timeval), PROT_READ | PROT_WRITE,
        //                    MAP_SHARED, memory, 0);
        gettimeofday(startTime, NULL);
        // Why is this not printing
        printf("Child start time: %ld\n", startTime->tv_usec);
        *ptr = *startTime;
        //execv(argv[1], args);
    }
  return 0;
}
