#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/fcntl.h>

#define	OO_FB_DEV	"oofb"
#define	OO_FB_MAJOR	241

#define	FBDEV		"/dev/fb0"

#define	USE_DEBUG	0

extern struct file_operations *oo_fops;
struct inode *oo_inode;
struct file *oo_file;

static int oo_fb_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
#if USE_DEBUG
	printk("oo fb ioctl - inode #0x%x file #0x%x - oo_inode #0x%x oo_file #0x%x\n", inode, file, oo_inode, oo_file);
#endif

	if (oo_fops->ioctl)
		return oo_fops->ioctl(oo_inode, oo_file, cmd, arg);
	return 0;
}

static int oo_fb_mmap(struct file *file, struct vm_area_struct *vma)
{
#if USE_DEBUG
	printk("oo fb mmap - oo_inode #0x%x file #0x%x vma #0x%x\n", file->f_dentry->d_inode, file, vma);
#endif
	if (oo_fops->mmap)
		return oo_fops->mmap(oo_file, vma);

	return 0;
}

int oo_fb_open(struct inode *inode, struct file *file)
{
	oo_file = filp_open(FBDEV, O_RDWR, 0);
	oo_inode = oo_file->f_dentry->d_inode;
#if USE_DEBUG
	printk("oo fb open - inode #0x%x file #0x%x - oo_inode #0x%x\n", inode, file, oo_inode);
#endif

	return 0;
}

int oo_fb_release(struct inode *inode, struct file *file)
{
#if USE_DEBUG
	printk("oo fb close - inode #0x%x file #0x%x - oo_inode #0x%x\n", inode, file, oo_inode);
#endif
	if (oo_file)
		filp_close(oo_file, current->files);
	return 0;
}

struct file_operations oo_fb_fops =
{
	.owner = THIS_MODULE,
	.ioctl = oo_fb_ioctl,
	.mmap = oo_fb_mmap,
	.open = oo_fb_open,
	.release = oo_fb_release,
};

static int oo_fb_init(void)
{
	int ret;
	ret = register_chrdev(OO_FB_MAJOR, OO_FB_DEV, &oo_fb_fops);
	if (ret < 0) return ret;

#if USE_DEBUG
	printk("oo fb init\n");
#endif

	return 0;
}

static void oo_fb_exit(void)
{
	unregister_chrdev(OO_FB_MAJOR, OO_FB_DEV);
#if USE_DEBUG
	printk("oo fb exit\n");
#endif
}

module_init(oo_fb_init);
module_exit(oo_fb_exit);

MODULE_LICENSE("GPL");
