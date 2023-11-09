#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int to[2], from[2];

  pipe(to);
  pipe(from);

  if (argc != 2)
    return -1;

  char *message = argv[1];
  printf("%ld\n", strlen(message));
  char *retmessage = malloc(strlen(message) + 1);

  if (fork()) { 
    close(to[0]);
    close(from[1]);
    write(to[1], message, strlen(message) + 1);
    read(from[0], retmessage, strlen(message) + 1); 
    printf("%s\n", retmessage);
    close(to[1]);
    close(from[0]);

  } else {
    close(to[1]);
    close(from[0]);

    read(to[0], message, strlen(message) + 1);
    for (int i = 0; i < strlen(message); i++) {
      if (message[i] > 64 && message[i] < 91)
        message[i] += 32;
      else if (message[i] > 96 && message[i] < 123)
        message[i] -= 32;
    }
    write(from[1], message, strlen(message) + 1);
    close(to[0]);
    close(from[1]);
  }
  return 0;
}
