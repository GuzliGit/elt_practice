#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/inet.h>
#include <linux/kobject.h>
#include <linux/string.h>

#define MAX_IPS_COUNT 10
#define IP_STR_LEN 16

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Badin Daniil");
MODULE_DESCRIPTION("Kernel module for filtering outgoing requests by ip");

static char block_list[MAX_IPS_COUNT][IP_STR_LEN];
static int blocked_count = 0;
static struct kobject* kobj;
static struct nf_hook_ops nfout;

static ssize_t show_list(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t add_to_list(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
static ssize_t remove_from_list(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

static struct kobj_attribute list_attr = __ATTR(list, 0444, show_list, NULL);
static struct kobj_attribute add_attr = __ATTR(add, 0200, NULL, add_to_list);
static struct kobj_attribute remove_attr = __ATTR(remove, 0200, NULL, remove_from_list);

static struct attribute *attrs[] = 
{
    &list_attr.attr,
    &add_attr.attr,
    &remove_attr.attr,
    NULL,
};

static struct attribute_group attr_group = 
{
    .attrs = attrs,
};

static ssize_t show_list(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    ssize_t count = 0;
    int i;

    if (blocked_count == 0) 
    {
        return sprintf(buf, "No blocked IPs\n");
    }

    for (i = 0; i < blocked_count; i++) 
    {
        count += sprintf(buf + count, "%s\n", block_list[i]);
    }

    return count;
}

static ssize_t add_to_list(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    __be32 ip_bin;
    char ip_str[IP_STR_LEN];
    int i;

    strncpy(ip_str, buf, IP_STR_LEN);
    if (ip_str[count - 1] == '\n') 
    {
        ip_str[count - 1] = '\0';
    } 
    else if (count < IP_STR_LEN) 
    {
        ip_str[count] = '\0';
    }

    if (in4_pton(ip_str, -1, (u8 *)&ip_bin, -1, NULL) == 0) 
    {
        pr_alert("Invalid IP: %s\n", ip_str);
        return -EINVAL;
    }

    for (i = 0; i < blocked_count; i++) 
    {
        if (strcmp(ip_str, block_list[i]) == 0) 
        {
            pr_info("IP %s is already blocked\n", ip_str);
            return count;
        }
    }

    if (blocked_count >= MAX_IPS_COUNT) 
    {
        pr_alert("List is full (max %d IPs)\n", MAX_IPS_COUNT);
        return -ENOSPC;
    }

    strcpy(block_list[blocked_count], ip_str);
    blocked_count++;
    
    pr_info("%s added to block list\n", ip_str);
    return count;
}

static ssize_t remove_from_list(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    char ip_str[IP_STR_LEN];
    int i, j;

    strncpy(ip_str, buf, IP_STR_LEN);
    if (ip_str[count - 1] == '\n') 
    {
        ip_str[count - 1] = '\0';
    } 
    else if (count < IP_STR_LEN) 
    {
        ip_str[count] = '\0';
    }

    for (i = 0; i < blocked_count; i++) 
    {
        if (strcmp(ip_str, block_list[i]) == 0)
        {
            for (j = i; j < blocked_count - 1; j++) 
            {
                strcpy(block_list[j], block_list[j + 1]);
            }
            blocked_count--;
            
            pr_info("%s removed from block list\n", ip_str);
            return count;
        }
    }

    pr_info("%s not found in block list\n", ip_str);
    return count;
}

static unsigned int hook_func_out(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct iphdr *ip_hdr;
    __be32 dst_ip;
    char dst_str[IP_STR_LEN];
    int i;

    ip_hdr = (struct iphdr *)skb_network_header(skb);
    if (!ip_hdr) 
    {
        return NF_ACCEPT;
    }

    dst_ip = ip_hdr->daddr;
    snprintf(dst_str, IP_STR_LEN, "%pI4", &dst_ip);

    for (i = 0; i < blocked_count; i++) 
    {
        if (strcmp(dst_str, block_list[i]) == 0) 
        {
            pr_info("Packet to %s was blocked\n", dst_str);
            return NF_DROP;
        }
    }

    return NF_ACCEPT;
}

static int __init netfilter_out_init(void)
{
    pr_info("Netfilter module was loaded\n");

    kobj = kobject_create_and_add("netfilter_mod", kernel_kobj);
    if (!kobj) 
    {
        return -ENOMEM;
    }

    int err;
    err = sysfs_create_group(kobj, &attr_group);
    if (err)
    {
        kobject_put(kobj);
        return err;
    }

    nfout.hook = hook_func_out;
    nfout.hooknum = NF_INET_LOCAL_OUT;
    nfout.pf = PF_INET;
    nfout.priority = NF_IP_PRI_FIRST;

    err = nf_register_net_hook(&init_net, &nfout);
    if (err) 
    {
        sysfs_remove_group(kobj, &attr_group);
        kobject_put(kobj);
        return err;
    }

    return 0;
}

static void __exit netfilter_out_cleanup(void)
{
    pr_info("Netfilter module was unloaded\n");

    nf_unregister_net_hook(&init_net, &nfout);
    sysfs_remove_group(kobj, &attr_group);
    kobject_put(kobj);
}

module_init(netfilter_out_init);
module_exit(netfilter_out_cleanup);
