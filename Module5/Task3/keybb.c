#include <linux/module.h>
#include <linux/configfs.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/printk.h> 
#include <linux/kobject.h> 
#include <linux/sysfs.h>
#include <linux/fs.h> 
#include <linux/string.h>

#define INPUT_BASE 10
#define BLINK_DELAY   HZ/5
#define ALL_LEDS_ON   0x07
#define RESTORE_LEDS 0xFF

static struct kobject *keyb_kobject;
static int kbled_val;

struct timer_list my_timer;
struct tty_driver *my_driver;
static int kbled_active = 0;

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", kbled_val);
}
 
static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    int temp;
    
    if (kstrtoint(buf, INPUT_BASE, &temp) == 0 && temp >= 0 && temp <= 7)
    {
        kbled_val = temp;
    }

    return count;
}

static struct kobj_attribute foo_attribute =__ATTR(kbled_val, 0660, foo_show, foo_store);

static void my_timer_func(struct timer_list *ptr)
{
    int next_status;

    if (kbled_active)
    {
        next_status = RESTORE_LEDS;
    }
    else
    {
        next_status = kbled_val;
    }

    kbled_active = !kbled_active;
    
    (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, next_status);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}

static int __init keybb_init (void)
{
    int error = 0;

    printk(KERN_INFO "Kernel module for keyboard initialized successfully!\n");

    keyb_kobject = kobject_create_and_add("syskbled_val", kernel_kobj);

    if (!keyb_kobject)
    {
        return -ENOMEM;
    }

    error = sysfs_create_file(keyb_kobject, &foo_attribute.attr);
    if (error) 
    {
        printk(KERN_INFO "[ERROR]: failed to create the foo file in /sys/kernel/syskbled_val\n");
    }

    my_driver = vc_cons[fg_console].d->port.tty->driver;
    timer_setup(&my_timer, my_timer_func, 0);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);

    return error;
}

static void __exit keybb_exit (void)
{
    printk(KERN_INFO "Kernel module for keyboard un initialized successfully!\n");
    kobject_put(keyb_kobject);

    timer_delete(&my_timer);
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Badin Daniil");
MODULE_DESCRIPTION("Kernel module for highlighting light bulbs on the keyboard via sysfs");
module_init(keybb_init);
module_exit(keybb_exit);
