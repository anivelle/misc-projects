#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>

#define PROC_NAME "jiffies"
#define BUF_SIZE 128

ssize_t jiffies_read(struct file *file, char __user *usr_buf, size_t count,
                  loff_t *pos);

struct proc_ops ops = {
  .proc_read = jiffies_read,
};

int jiffies_init(void) {
  proc_create(PROC_NAME, 0666, NULL, &ops);
  return 0;
}

void jiffies_exit(void) {
  remove_proc_entry(PROC_NAME, NULL);
}


ssize_t jiffies_read(struct file *file, char __user *usr_buf, size_t count,
                  loff_t *pos){
  int rv = 0;
  char buf[BUF_SIZE];
  static int completed = 0;

  if (completed) {
    completed = 0;
    return 0;
  }

  completed = 1;
  rv = sprintf(buf, "%lu\n", jiffies);

  copy_to_user(usr_buf, buf, rv);
  return rv;
}

module_init(jiffies_init);
module_exit(jiffies_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Jiffies Module");
MODULE_AUTHOR("Me");
