#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  
  struct sockaddr_in address = {AF_INET, htons(5000), htonl(INADDR_LOOPBACK)};
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if (bind(sock, (struct sockaddr *)&address, sizeof(address)) == -1) {
    printf("Error binding\n");
    return -1;
  }
  listen(sock, 20);

  while (1) {
    struct sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(peer_addr);

    int connection = accept(sock, (struct sockaddr *)&peer_addr, &addr_len);
    char buf[64];

    ssize_t readchars;
    while ((readchars = read(connection, buf, sizeof(buf))) != 0) {
      write(connection, buf, readchars);
    }
    // shutdown(connection, SHUT_RD);
    // close(connection);
  }
  return 0;
}
