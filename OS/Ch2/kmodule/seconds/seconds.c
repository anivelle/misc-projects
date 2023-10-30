#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <asm/param.h>

#define PROC_NAME "seconds"
#define BUF_SIZE 128

unsigned long start_time; 

ssize_t seconds_read(struct file *file, char __user *usr_buf, size_t count,
                  loff_t *pos);

struct proc_ops ops = {
  .proc_read = seconds_read,
};

int seconds_init(void) {
  proc_create(PROC_NAME, 0666, NULL, &ops);
  start_time = jiffies;
  return 0;
}

void seconds_exit(void) {
  remove_proc_entry(PROC_NAME, NULL);
}


ssize_t seconds_read(struct file *file, char __user *usr_buf, size_t count,
                  loff_t *pos){
  int rv = 0;
  char buf[BUF_SIZE];
  static int completed = 0;

  if (completed) {
    completed = 0;
    return 0;
  }

  completed = 1;
  rv = sprintf(buf, "%lu\n", (jiffies - start_time) / HZ);

  copy_to_user(usr_buf, buf, rv);
  return rv;
}

module_init(seconds_init);
module_exit(seconds_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("Me");
