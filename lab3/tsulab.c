#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/timekeeping.h>
#include <linux/slab.h>

#define procfs_name "tsulab"
#define GAGARIN_START_TIME_UTC 387401220

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TSU Lab");
MODULE_DESCRIPTION("TSU Lab /proc file module with custom messages.");

static struct proc_dir_entry *our_proc_file;
static ssize_t procfile_read(struct file *file, char __user *buffer,
                             size_t len, loff_t *offset)
{
    if (*offset > 0)
        return 0;

    s64 current_time_sec = ktime_get_real_seconds();
    
    s64 elapsed_sec = current_time_sec - GAGARIN_START_TIME_UTC;
    s64 elapsed_min = elapsed_sec / 60;

    char msg[128];
    int msg_len;
    
    msg_len = snprintf(msg, sizeof(msg),
        "Прошло минут со старта Гагарина (12.04.1961 06:07 UTC): %lld\n",
        (long long)elapsed_min);

    if (msg_len < 0 || msg_len >= sizeof(msg)) {
        pr_err("tsulab: snprintf failed or buffer overflow\n");
        return -EINVAL;
    }

    if (copy_to_user(buffer, msg, msg_len))
        return -EFAULT;

    *offset = msg_len;
    return msg_len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

static int __init tsulab_init(void)
{
    pr_info("Welcome to the Tomsk State University\n"); 
    
    our_proc_file = proc_create(procfs_name, 0444, NULL, &proc_file_fops);
    if (!our_proc_file) {
        pr_err("tsulab: Failed to create /proc/%s\n", procfs_name);
        pr_info("tsulab: Module load failed.\n");
        return -ENOMEM;
    }
    pr_info("tsulab: /proc/%s file created successfully.\n", procfs_name);
    
    return 0;
}

static void __exit tsulab_exit(void)
{
    proc_remove(our_proc_file);

    pr_info("Tomsk State University forever!\n"); 
}

module_init(tsulab_init);
module_exit(tsulab_exit);
