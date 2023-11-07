#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2)
        return 1;
    int memory = shm_open("/collatz", O_RDWR | O_CREAT, 0666);
    ftruncate(memory, sizeof(int *));
    int *ptr = (int *)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,
                           memory, 0);

    if (fork()) {
        wait(NULL);
        int size = ptr[0];
        for (int i = 1; i < size - 1; i++) {
            printf("%d, ", ptr[i]);
        }
        printf("%d\n", ptr[size - 1]);
    } else {
        int val = atoi(argv[1]);
        // Allocate one more integer to let the parent know how long the array
        // is
        int size = 2;
        // Idk if this is slow, but I wanted to use as little memory as
        // possible, especially since longer sequences of the Collatz conjecture
        // could take up a lot more memory than originally allocated, but also
        // shorter ones will be wasting a large allocation
        int *sequence = malloc(sizeof(int) * size);  
        while (val != 1) {
            sequence[size - 1] = val;
            size++;
            // Reallocate memory to add a new value on the next loop
            sequence = realloc(sequence, sizeof(int) * size);
            if (val % 2 == 0)
                val /= 2;
            else
                val = 3 * val + 1;
        }
        sequence[size - 1] = val;
        sequence[0] = size;
        // Remap the shared memory object to contain the size of the array and
        // copy the array into it
        ptr = (int *)mmap(0, size * sizeof(int), PROT_READ | PROT_WRITE,
                          MAP_SHARED, memory, 0);
        memcpy(ptr, sequence, size * sizeof(int));
    }

    return 0;
}
