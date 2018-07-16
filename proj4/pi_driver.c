/*******************************
 * Author: Michael Adams
 * Last Edit: 4/22/18
 * Pitt CS 449
 ******************************/

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

#include <asm/uaccess.h>

#include "pi.h"

/*
 * pi_read is the function called when a process calls read() on /dev/pi.
 */
static ssize_t pi_read(struct file * file, char * buf, size_t count, loff_t *ppos)
{
	// declare relevant vars
	char *pi_buf; //buffer to pass to pi()
	unsigned int space; //amount of space we need for buffer

	// calculate amount of space pi_buf needs
	space = (4 - (*ppos + count) % 4) + *ppos + count;

	// create buffer using pi.h macro
	pi_buf = (char *)MALLOC(space);

	// call pi
	pi(pi_buf, space);

	/*
	 * Besides copying the string to the user provided buffer,
	 * this function also checks that the user has permission to
	 * write to the buffer, that it is mapped, etc.
	 */
	if (copy_to_user(buf, pi_buf + *ppos, count)) {
		FREE(pi_buf);
		return -EINVAL;
	}
	
	/*
	 * Tell the user how much data we wrote.
	 */
	FREE(pi_buf);
	*ppos += count;

	return count;
}

/*
 * The only file operation we care about is read.
 */
static const struct file_operations pi_fops = {
	.owner		= THIS_MODULE,
	.read		= pi_read,
};

static struct miscdevice pi_driver = {
	/*
	 * We don't care what minor number we end up with, so tell the
	 * kernel to just pick one.
	 */
	MISC_DYNAMIC_MINOR,
	/*
	 * Name ourselves /dev/pi.
	 */
	"pi_driver",
	/*
	 * What functions to call when a program performs file
	 * operations on the device.
	 */
	&pi_fops
};

static int __init 
pi_init(void)
{
	int ret;

	/*
	 * Create the "pi_driver" device in the /sys/class/misc directory.
	 * Udev will automatically create the /dev/hello device using
	 * the default rules.
	 */
	ret = misc_register(&pi_driver);
	if (ret)
		printk(KERN_ERR "Unable to register pi misc device\n");
	return ret;
}

module_init(pi_init);

static void __exit 
pi_exit(void)
{
	misc_deregister(&pi_driver);
}

module_exit(pi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Adams");
MODULE_DESCRIPTION("Pi tings");
MODULE_VERSION("dev");
