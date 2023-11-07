#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
  if (argc != 2) 
    return 1;
  
  if (fork()) {
    wait(NULL);
  } else {
    int val = atoi(argv[1]);

    while (val != 1) {
      printf("%d, ", val);
      if (val % 2 == 0)
        val /= 2; 
      else 
        val = 3 * val + 1;
    }

    printf("1\n");
  }

  return 0;
}
