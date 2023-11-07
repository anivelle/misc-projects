#include <stdio.h>
#include <stdlib.h>

#define MIN_PID 300
#define MAX_PID 5000
#define DIVISOR sizeof(int)

int *pid_map;
int cur_pid;

/* Using the suggestion in the book and making a bitmap. Don't think C would
 *  allow a single datatype this long so instead I'm gonna use some bit shifting
 *  and modulo.  
 */

int allocate_map(void) {
    // Allocates enough bytes to have one bit for every possible PID
    pid_map = malloc(MAX_PID / 8 + 1);
    if (pid_map == NULL)
        return -1;
    cur_pid = MIN_PID;
    return 1;
}

int allocate_pid(void) {
    if (cur_pid > MAX_PID)
        return -1;
    int *pid_space;
    pid_space = &pid_map[cur_pid / DIVISOR];
    // Loop to find the first available PID
    // This should only happen if a PID is released, idk if there's a better
    // way. Maybe keep track of the "top" of the PID stack?
    while (*pid_space & (1 << (cur_pid % DIVISOR))) {
      cur_pid++;
      if (cur_pid == MAX_PID)
        return -1;
      // Only get a new integer from the map if we wrap through our original
      if (cur_pid % DIVISOR == 0)
        pid_space = &pid_map[cur_pid / DIVISOR];
    }
    *pid_space ^= 1 << (cur_pid % DIVISOR);
    int return_pid = cur_pid;
    cur_pid++;
    return return_pid;
}

void release_pid(int pid) {
    if (pid > MAX_PID)
        return;
    if (pid < MIN_PID)
        return;
    int temp_pid = pid;
    int *pid_space = &pid_map[temp_pid / DIVISOR];
    printf("PID int: %02X\n", *pid_space);
    // Only release if the PID is actually in use?
    if (*pid_space & (1 << (temp_pid % DIVISOR))) {
        *pid_space ^= 1 << (temp_pid % DIVISOR);
        if (pid < cur_pid)
          cur_pid = pid;
    printf("Current PID: %d\n", cur_pid);
  }
}

int main(int argc, char *argv[]) {
  // Should print 1 to indicate success
  printf("Mapping structure: %d\n", allocate_map());

  printf("Allocating PIDs");
  // This should basically count from 300 - 5000 and then return a bunch of -1s
  for (int i = MIN_PID; i < MAX_PID + 20; i++) {
    printf("%d\n", allocate_pid());
  }
  
  // These two loops should result in counting from 300 - 500 in multiples of 2
  for (int i = MIN_PID; i < 500; i += 2) {
    release_pid(i);
  }

  for (int i = MIN_PID; i < 500; i += 2) {
    printf("%d\n", allocate_pid());
  }
}
