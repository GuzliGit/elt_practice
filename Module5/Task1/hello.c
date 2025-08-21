#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h> 

MODULE_LICENSE("BDL");
MODULE_AUTHOR("Badin Daniil");
MODULE_DESCRIPTION("First task kernel module");

static int __init hello_init(void)
{
    printk(KERN_INFO "This is Hello World module!\n");
    return 0;
}

static void __exit hello_cleanup(void)
{
    printk(KERN_INFO "Module cleanup...\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
