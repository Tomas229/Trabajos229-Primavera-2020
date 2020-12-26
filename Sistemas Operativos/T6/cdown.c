/* Necessary includes for device drivers */
#include <linux/init.h>
/* #include <linux/config.h> */
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h>   /* kmalloc() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>   /* O_ACCMODE */
#include <linux/uaccess.h> /* copy_from/to_user */

MODULE_LICENSE("Dual BSD/GPL");

/* Declaration of cdown.c functions */
static int cdown_open(struct inode *inode, struct file *filp);
static int cdown_release(struct inode *inode, struct file *filp);
static ssize_t cdown_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t cdown_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void cdown_exit(void);
int cdown_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations cdown_fops = {
    read : cdown_read,
    write : cdown_write,
    open : cdown_open,
    release : cdown_release
};

/* Declaration of the init and exit functions */
module_init(cdown_init);
module_exit(cdown_exit);

/* Global variables of the driver */
/* Major number */
static int cdown_major = 61;
/* Buffer to store data */
#define KBUF_SIZE 20
static char kbuf[KBUF_SIZE];
static struct semaphore mutex;
static int c;

int cdown_init(void)
{
    int result;
    /* Registering device */
    result = register_chrdev(cdown_major, "cdown", &cdown_fops);
    if (result < 0)
    {
        printk(
            "<1>cdown: cannot obtain major number %d\n", cdown_major);
        return result;
    }

    sema_init(&mutex, 1);
    return 0;
}

void cdown_exit(void)
{
    /* Freeing the major number */
    unregister_chrdev(cdown_major, "memory");
}

static int cdown_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int cdown_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t cdown_read(struct file *filp, char *buf,
                          size_t count, loff_t *f_pos)
{
    down(&mutex);
    int rc;
    if (c == 0 || *f_pos != 0)
    {
        rc = 0;
        goto epilog;
    }

    int len = snprintf(kbuf, KBUF_SIZE, "%d\n", c);
    if (len < 0)
    {
        rc = len;
        goto epilog;
    }

    if (count > len)
    {
        count = len;
    }

    rc = copy_to_user(buf, kbuf, count); //count bytes desde kbuf a buf
    if (rc != 0)
    {
        rc = -EFAULT;
        goto epilog;
    }

    c--;
    *f_pos += count;
    rc = count;
epilog:
    up(&mutex);
    return rc;
}

static ssize_t cdown_write(struct file *filp, const char *buf,
                           size_t count, loff_t *f_pos)
{
    down(&mutex);
    int rc = kstrtoint_from_user(buf, count, 10, &c);
    if (rc < 0)
    {
        printk("<1> error de escritura: Argumento invalido");
        up(&mutex);
        return rc;
    }
    up(&mutex);
    return count;
}
