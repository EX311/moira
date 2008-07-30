#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/delay.h>
//#include <linux/devfs_fs_kernel.h>
#include <asm/arch/map.h>

#define	KEY_DEV_NAME	"ookey"
#define	KEY_DEV_MAJOR	240
#define	READ_DELAY		15

#define	rGPBCON		(*(volatile unsigned *)(S3C24XX_VA_GPIO+0x10))
#define	rGPBDAT		(*(volatile unsigned *)(S3C24XX_VA_GPIO+0x14))
#define	rGPBUP		(*(volatile unsigned *)(S3C24XX_VA_GPIO+0x18))

#define	rGPFCON		(*(volatile unsigned *)(S3C24XX_VA_GPIO+0x50))
#define	rGPFDAT		(*(volatile unsigned *)(S3C24XX_VA_GPIO+0x54))
#define	rGPFUP		(*(volatile unsigned *)(S3C24XX_VA_GPIO+0x58))

#define	USE_DEBUG	0

DECLARE_WAIT_QUEUE_HEAD(WaitQueue_Read);
int anti_chat;
int keyid;

irqreturn_t oo_key_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
#if USE_DEBUG
	printk("interrupt %d\n", irq);
#endif
	
	if (anti_chat == READ_DELAY) {
		rGPFCON &= ~((1<<7)|(1<<6));

		rGPBDAT = ((rGPBDAT & (~(0x1f))) | ~(1<<4));
		if (!(rGPFDAT & (1<<3))) {
			keyid = 0x1;
#if USE_DEBUG
			printk("ENTER\n");
#endif
			goto OUT_INT3;
		}

		rGPBDAT = ((rGPBDAT & (~(0x1f))) | ~(1<<3));
		if (!(rGPFDAT & (1<<3))) {
			keyid = 0x2;
#if USE_DEBUG
			printk("UP\n");
#endif
			goto OUT_INT3;
		}

		rGPBDAT = ((rGPBDAT & (~(0x1f))) | ~(1<<2));
		if (!(rGPFDAT & (1<<3))) {
			keyid = 0x4;
#if USE_DEBUG
			printk("LEFT\n");
#endif
			goto OUT_INT3;
		}

		rGPBDAT = ((rGPBDAT & (~(0x1f))) | ~(1<<1));
		if (!(rGPFDAT & (1<<3))) {
			keyid = 0x8;
#if USE_DEBUG
			printk("DOWN\n");
#endif
			goto OUT_INT3;
		}

		rGPBDAT = ((rGPBDAT & (~(0x1f))) | ~(1<<0));
		if (!(rGPFDAT & (1<<3))) {
			keyid = 0x10;
#if USE_DEBUG
			printk("RIGHT\n");
#endif
			goto OUT_INT3;
		}
OUT_INT3:
		rGPFCON = (rGPFCON & (~(0xffc0))) | 0xaa80;
		rGPFUP	= rGPFUP & (~(0xf8));
		rGPBDAT	= (rGPBDAT & (~(0x1f)));

		anti_chat = 0;
		wake_up_interruptible(&WaitQueue_Read);
		mdelay(50);
	}
	anti_chat++;

	return IRQ_HANDLED;
}

ssize_t oo_key_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	keyid = -1;
#if USE_DEBUG
	printk("oo read\n");
#endif
	if (!(filp->f_flags & O_NONBLOCK))
		interruptible_sleep_on(&WaitQueue_Read);
	else
		return -EAGAIN;

/*	rGPBDAT |= ((1<<4)|(1<<3)|(1<<2)|(1<<1)|1);
	if (!anti_chat) {
		rGPBDAT = (8+4+2+1);
		udelay(400);
		rGPBDAT = (16+4+2+1);
		udelay(400);
		rGPBDAT = (16+8+2+1);
		udelay(400);
		rGPBDAT = (16+8+4+1);
		udelay(400);
		rGPBDAT = (16+8+4+2);
		udelay(400);
	}
*/
	udelay(400);
	return keyid;
}

int oo_key_open(struct inode *inode, struct file *filp)
{
#if USE_DEBUG
	printk("oo open\n");
#endif
	return 0;
}

int oo_key_release(struct inode *inode, struct file *filp)
{
#if USE_DEBUG
	printk("oo close\n");
#endif
	return 0;
}

struct file_operations oo_key_fops =
{
	.owner = THIS_MODULE,
	.read = oo_key_read,
	.open = oo_key_open,
	.release = oo_key_release,
};

static int oo_key_init(void)
{
	int ret;
	ret = register_chrdev(KEY_DEV_MAJOR, KEY_DEV_NAME, &oo_key_fops);
	if (ret < 0) return ret;
#if USE_DEBUG
	printk("oo init\n");
#endif

//	devfs_mk_cdev(MKDEV(KEY_DEV_MAJOR,0), S_IFCHR|S_IRUGO, KEY_DEV_NAME);
	
	rGPFCON = (rGPFCON & (~(0xffc0))) | 0xaa80;
	rGPFUP = rGPFUP & (~(0xf8)) ;
//	rEXTINT0 = (rEXTINT0 & (~(0xfffff000))) | 0x44444000;
	rGPBCON = (rGPBCON & (~(0x3ff))) | 0x155;
    rGPBDAT = (rGPBDAT & (~(0x1f)));
		
	ret = request_irq(IRQ_EINT3, oo_key_interrupt, SA_INTERRUPT, KEY_DEV_NAME, NULL);
//	if (ret < 0) return ret;
//	ret = request_irq(IRQ_EINT4, oo_key_interrupt, SA_INTERRUPT, KEY_DEV_NAME, NULL);
//	if (ret < 0) return ret;
//	ret = request_irq(IRQ_EINT5, oo_key_interrupt, SA_INTERRUPT, KEY_DEV_NAME, NULL);
//	if (ret < 0) return ret;
//	ret = request_irq(IRQ_EINT6, oo_key_interrupt, SA_INTERRUPT, KEY_DEV_NAME, NULL);
//	if (ret < 0) return ret;
//	ret = request_irq(IRQ_EINT7, oo_key_interrupt, SA_INTERRUPT, KEY_DEV_NAME, NULL);
//	if (ret < 0) return ret;

	
	return 0;
}

static void oo_key_exit(void)
{
//	devfs_remove(KEY_DEV_NAME);

	unregister_chrdev(KEY_DEV_MAJOR, KEY_DEV_NAME);
	free_irq(IRQ_EINT3, NULL);
//	free_irq(IRQ_EINT4, NULL);
//	free_irq(IRQ_EINT5, NULL);
//	free_irq(IRQ_EINT6, NULL);
//	free_irq(IRQ_EINT7, NULL);
#if USE_DEBUG
	printk("oo_exit\n");
#endif
}

module_init(oo_key_init);
module_exit(oo_key_exit);

MODULE_LICENSE("GPL");
