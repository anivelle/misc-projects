#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

#define BUF_SIZE 128
#define PROC_NAME "proc"

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count,
                  loff_t *pos);

// Higher versions of the Linux kernel use struct proc_ops rather than
// struct file_operations
static struct proc_ops ops = {.proc_read = proc_read};

int proc_init(void) {
    
    proc_create(PROC_NAME, 0666, NULL, &ops);
    return 0;
}

void proc_exit(void) { remove_proc_entry(PROC_NAME, NULL); }

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count,
                  loff_t *pos) {
  int rv = 0;
  char buffer[BUF_SIZE];
  static int completed = 0;

  if (completed) {
    completed = 0;
    return 0;
  }

  completed = 1;
  rv = sprintf(buffer, "Hello World\n");
  // This function returns the number of bytes not copied, but it looks like you
  // can't return the value on the first go if you want the buffer to be copied. 
  copy_to_user(usr_buf, buffer, rv);
  return rv;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Proc FS Module");
MODULE_AUTHOR("Me");
