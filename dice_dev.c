/*
 ============================================================================
 Name        : dice_dev.c
 Author		 : ERL67 
 dice.rules	 : KERNEL=="dice", SYMLINK+="dice_dev", MODE="0444"
 Compile     : make ARCH=i386
 Move  		 : scp erl67@thoth.cs.pitt.edu:~/private/449/proj5/ . -r
 Load	     : insmod dice_dev.ko; mknod /dev/dice c 10 63
 Test	     : od -t x1 /dev/dice
 Remove	     : rm /dev/dice; rmmod dice_dev.ko
 ============================================================================
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/random.h>

static ssize_t dice_read(struct file * file, unsigned char * buf, size_t count, loff_t *ppos) {

	int i;
	unsigned char* mybuf = kmalloc(count, GFP_KERNEL);	//store rolls on heap
	get_random_bytes(mybuf, count);

	for(i = 0; i < count; i++) {
		*(mybuf + i) %= 6;
	}

	if (copy_to_user(buf, mybuf, count)) {
		kfree(mybuf);
		printk(KERN_ERR "You lost the dice.\n");
		return -EINVAL;
	} else {
		kfree(mybuf);
		*ppos = *ppos + count;
	}
	return count;
}

static const struct file_operations dice_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_read,
};

static struct miscdevice dice_dev = {
	MISC_DYNAMIC_MINOR,
	"dice",
	&dice_fops
};

static int __init
dice_init(void) {
	int ret;
	ret = misc_register(&dice_dev);
	if (ret)
		printk(KERN_ERR "Unable to register \"Dice dev\" misc device\n");
	return ret;
}

module_init(dice_init);

static void __exit
dice_exit(void) {
	misc_deregister(&dice_dev);
}

module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eric Laslo <erl67@pitt.edu>");
MODULE_DESCRIPTION("\"Dice\" minimal module");
MODULE_VERSION("dev");
