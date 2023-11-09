#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  
  int pipes[2];

  pipe(pipes);
  if (argc != 3) 
    return -1;

  char *srcFile = argv[1];
  char *dstFile = argv[2];

  if (fork()) {
    int src;
    if ((src = open(srcFile, O_RDONLY)) == -1) {
      printf("Error opening source file\n");
      return -1;
    }

    struct stat st;
    close(pipes[0]);
    char buf[1024];
    int readBytes;
    fstat(src, &st);
    write(pipes[1], &st, sizeof(st));
    while ((readBytes = read(src, buf, sizeof(buf))) != 0) {
      write(pipes[1], buf, readBytes);
    }

    close(src);
    close(pipes[1]);

  } else {
    int dst;
    if ((dst = open(dstFile, O_WRONLY | O_CREAT, S_IRWXU)) == -1) {
      printf("Error opening/creating destination file\n");
      return -1;
    }
    char buf[1024];

    int readBytes;
    struct stat st;
    read(pipes[0], &st, sizeof(struct stat));
    fchmod(dst, st.st_mode);
    while ((readBytes = read(pipes[0], buf, sizeof(buf))) != 0) {
      write(dst, buf, readBytes);
    }
    close(dst);
    close(pipes[0]);
  }
  return 0;
}
