/*Create a proc named jiffies*/
#include <asm/param.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

const int hz = HZ;
unsigned long int volatile initial_jiffies;

ssize_t my_read(struct file *file, char __user *user_buf, size_t count,
                loff_t *pos);
/*Declare the owner and reader function*/
static struct proc_ops my_proc = {
    .proc_read = my_read,
};

int proc_init(void) {
  proc_create(PROC_NAME, 0666, NULL, &my_proc);
  initial_jiffies = jiffies;
  printk(KERN_INFO "The /proc/seconds loaded successfully!\n");
  return 0;
}

void proc_exit(void) {
  remove_proc_entry(PROC_NAME, NULL);
  printk(KERN_INFO "The /proc/seconds unloaded successfully!\n");
}

/*Implementation details of proc_read*/
ssize_t my_read(struct file *file, char __user *usr_buf, size_t count,
                loff_t *pos) {
  int rv = 0;
  char buffer[BUFFER_SIZE];
  static int completed = 0;
  if (completed) {
    completed = 0;
    return 0;
  }
  completed = 1;
  int sec = (jiffies - initial_jiffies) / hz;
  rv = sprintf(buffer, "Now it has passed %lu\n", sec);
  copy_to_user(usr_buf, buffer, rv);
  return rv;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Display the seconds");
MODULE_AUTHOR("Yevzwming");
