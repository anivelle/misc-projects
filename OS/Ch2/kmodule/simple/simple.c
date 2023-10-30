#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <asm/param.h>

int simple_init(void) {
  printk(KERN_INFO "HZ: %d", HZ);
  printk(KERN_INFO "jiffies init: %lu", jiffies);
  return 0;
}

void simple_exit(void) {
  printk(KERN_INFO "jiffies exit: %lu", jiffies);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("Me");
