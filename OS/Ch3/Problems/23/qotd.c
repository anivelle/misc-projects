#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  
  // Important: use htons() and htonl() on the port and address values
  const struct sockaddr_in address = {AF_INET, htons(6017), htonl(INADDR_LOOPBACK)};
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1) {
    printf("Error binding\n");
    return -1;
  }
  
  listen(sock, 20);
  while (1) {
    struct sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(peer_addr);
    int new_sock = accept(sock, (struct sockaddr *)&peer_addr, &addr_len);
    if (new_sock == -1) {
      printf("Could not accept connection\n");
      return -1;
    }
    char *message = "Hello";
    write(new_sock, message, sizeof(char) * 6);
    close(new_sock);
  }
  return 0;
}
