#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define BUF_SIZE 512

int main(int argc, char *argv[]) {
  int orig, copy;
  size_t bread, bwrite;
  char buf[BUF_SIZE];

  if (argc < 3) {
    printf("Please provide filenames.\n");
    return 1;
  }
  
  if ((orig = open(argv[1], O_RDONLY)) < 0) {
    printf("Error opening file %s: File does not exist.\n", argv[1]);
    return 1;
  }
  copy = creat(argv[2], O_WRONLY | S_IRWXU | S_IRGRP | S_IROTH);
  

  while (1) {
    bread = read(orig, buf, BUF_SIZE);
    bwrite = write(copy, buf, bread);
    if (bread < BUF_SIZE)
      break;
  }
  printf("Done copying\n");
  return 0;
}
