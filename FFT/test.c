#include <stdlib.h>
#include <unistd.h>
#include <complex.h>
#include <stdio.h>

int array_test(int *arr) { return arr[2]; }

int main(int argc, char *argv[]) {

    int arr[] = {0, 1, 2, 3, 4, 5, 6};

    printf("%d\n", array_test(&arr[3]));

    return 0;
}
