#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define PROC_NAME "procrw"
#define BUF_SIZE 128

static int len,temp;
static char *msg;
 
static ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *offp ) 
{
    if (count > temp) 
    {
        count = temp;
    }

    temp = temp - count;
    if (copy_to_user(buf, msg, count) != 0)
    {
        return -EFAULT;
    }

    if (count == 0)
        temp = len;
    
    return count;
}
 
static ssize_t write_proc(struct file *filp, const char *buf, size_t count, loff_t *offp) 
{
    if (count > BUF_SIZE - 1)
    {
        count = BUF_SIZE - 1;
    }

    if (copy_from_user(msg, buf, count) != 0)
    {
        return -EFAULT;
    }

    msg[count] = '\0';
    len = count;
    temp = len;

    return count;
}
 
static const struct proc_ops proc_fops = 
{
    proc_read: read_proc,
    proc_write: write_proc,
};
 
static void create_new_proc_entry(void) 
{ 
    msg = kmalloc(BUF_SIZE * sizeof(char), GFP_KERNEL);
    proc_create(PROC_NAME, 0666, NULL, &proc_fops);
}
 
static int proc_init (void) 
{
    create_new_proc_entry();
    printk(KERN_INFO "Proc created successfully!\n");
    return 0;
}
 
static void proc_cleanup(void) 
{
    remove_proc_entry(PROC_NAME, NULL);
    kfree(msg);
    printk(KERN_INFO "Proc removed successfully!\n");
}

MODULE_LICENSE("BDL");
MODULE_AUTHOR("Badin Daniil");
MODULE_DESCRIPTION("Kernel module for read/write from/to proc");
module_init(proc_init);
module_exit(proc_cleanup);