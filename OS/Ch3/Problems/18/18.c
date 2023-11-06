#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char *argv[]) {

  if (fork()) {
    while (1) {};
  } else {
    printf("Child exiting\n");
    exit(0);
  }

  return 0;
}
