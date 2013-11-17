 #include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <mach/am_regs.h>
#include <mach/pinmux.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "val_remote.h" 

//type_printk input_dbg;

static DEFINE_MUTEX(kp_enable_mutex);
static DEFINE_MUTEX(kp_file_mutex);

 

static struct kp   *gp_kp=NULL;
//char *remote_log_buf;
typedef  struct {
	char		     *platform_name;
	unsigned int  pin_mux;
	unsigned int  bit;
}pin_config_t;
 


static int remote_printk(const char *fmt, ...)
{
	va_list args;
	int r;

	if (gp_kp->debug_enable==0)  return 0;
	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);
	return r;
}

static int kp_mouse_event(struct input_dev *dev, unsigned int scancode, unsigned int type)
{
	 
	return 0;
}

 
static void  disable_remote_irq(void)
{
	 

}
static void  enable_remote_irq(void)
{
	 
}
 
static irqreturn_t kp_interrupt(int irq, void *dev_id)
{
	/* disable keyboard interrupt and schedule for handling */
	//  input_dbg("===trigger one  kpads interupt \r\n");
	 

	return IRQ_HANDLED;
}
 
 
static ssize_t kp_virtualEvent_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	int ret =0;
	//ret=sprintf(buf, "%s\n", remote_log_buf);
	//printk(remote_log_buf);
 
	return ret ;
}

static ssize_t kp_enable_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n",1);
}

//control var by sysfs .
static ssize_t kp_enable_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t count)
{
	int state;

	if (sscanf(buf, "%u", &state) != 1)
		return -EINVAL;

	if ((state != 1) && (state != 0))
		return -EINVAL;

	mutex_lock(&kp_enable_mutex);
	 
	mutex_unlock(&kp_enable_mutex);

	return strnlen(buf, count);
}

//static DEVICE_ATTR(enable, S_IRUGO | S_IWUSR, kp_enable_show, kp_enable_store);
static DEVICE_ATTR(virtualEvent, S_IRUGO | S_IWUSR, kp_virtualEvent_show, NULL);


static int  val_remote_config_open(struct inode *inode, struct file *file)
{    
	unsigned int   val, i;
     
   	return 0;
}
	static int
val_remote_config_ioctl(struct inode *inode, struct file *filp,
		unsigned int cmd, unsigned long args)
{
	struct kp   *kp=(struct kp*)filp->private_data;
	void  __user* argp =(void __user*)args;
	unsigned int   val, i;

	 
	return  0;
}

	static int
val_remote_config_release(struct inode *inode, struct file *file)
{
	file->private_data=NULL;
	return 0;
}

static const struct file_operations remote_fops = {
	.owner      = THIS_MODULE,
	.open       =val_remote_config_open,
	.ioctl      = val_remote_config_ioctl,
	.release        = val_remote_config_release,
};

static int  register_remote_dev(struct kp  *kp)
{
	int ret=0;
	strcpy(kp->config_name,"valremote");
	ret=register_chrdev(0,kp->config_name,&remote_fops);
	if(ret <=0)
	{
		printk("register char dev tv error\r\n");
		return  ret ;
	}
	kp->config_major=ret;
	printk("valremote config major:%d\r\n",ret);
	kp->config_class=class_create(THIS_MODULE,kp->config_name);
	kp->config_dev=device_create(kp->config_class,NULL,MKDEV(kp->config_major,0),NULL,kp->config_name);
	return ret;
}

static int __init kp_probe(struct platform_device *pdev)
{
	struct kp *kp;
	struct input_dev *input_dev;

	int i,ret;
printk("set ----------------------------- completed\r\n");  
	 
	kp = kzalloc(sizeof(struct kp), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!kp || !input_dev) {
		kfree(kp);
		input_free_device(input_dev);
		return -ENOMEM;
	}
	gp_kp=kp;
	kp->debug_enable=0;

	//input_dbg=remote_printk;
	platform_set_drvdata(pdev, kp);
	kp->work_mode=REMOTE_NEC_HW;
	kp->input = input_dev;
	//kp->release_delay=KEY_RELEASE_DELAY;
	 
 

	/* get the irq and init timer*/
	printk("set drvdata completed\r\n");
	//tasklet_enable(&tasklet);
	//tasklet.data = (unsigned long) kp;
	//setup_timer(&kp->timer, kp_timer_sr, 0) ;


	//ret = device_create_file(&pdev->dev, &dev_attr_enable);
	//if (ret < 0)
	//	goto err1;
	ret=device_create_file(&pdev->dev, &dev_attr_virtualEvent);
	if(ret<0)
	{
		device_remove_file(&pdev->dev, &dev_attr_virtualEvent);
		goto err1;
	}

	printk("device_create_file completed \r\n");
	input_dev->evbit[0] = BIT_MASK(EV_KEY)  | BIT_MASK(EV_REL);
	input_dev->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) |BIT_MASK(BTN_RIGHT)|BIT_MASK(BTN_MIDDLE);
	input_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y)| BIT_MASK(REL_WHEEL);
	input_dev->keybit[BIT_WORD(BTN_MOUSE)] |=BIT_MASK(BTN_SIDE)|BIT_MASK(BTN_EXTRA);	
	for (i = 0; i<KEY_MAX; i++){
		set_bit( i, input_dev->keybit);
	}

	//clear_bit(0,input_dev->keybit);
	input_dev->name = "val_keypad";
	input_dev->phys = "keypad/input0";
	//input_dev->cdev.dev = &pdev->dev;
	//input_dev->private = kp;
	input_dev->dev.parent = &pdev->dev;

	input_dev->id.bustype = BUS_ISA;
	input_dev->id.vendor = 0x0001;
	input_dev->id.product = 0x0001;
	input_dev->id.version = 0x0100;
	kp->repeat_enable=0;


	input_dev->rep[REP_DELAY]=0xffffffff;
	input_dev->rep[REP_PERIOD]=0xffffffff;


	input_dev->keycodesize = sizeof(unsigned short);
	input_dev->keycodemax = 0x1ff;

	ret = input_register_device(kp->input);
	if (ret < 0) {
		printk(KERN_ERR "Unable to register keypad input device\n");
		goto err2;
	}
	printk("input_register_device completed \r\n");
	 

	register_remote_dev(gp_kp);
	//remote_log_buf = (char*)__get_free_pages(GFP_KERNEL,REMOTE_LOG_BUF_ORDER);
	//remote_log_buf[0]='\0';
	//printk("physical address:0x%x\n",(unsigned int )virt_to_phys(remote_log_buf));
	return 0;
err3:
	//     free_irq(NEC_REMOTE_IRQ_NO,kp_interrupt);
	input_unregister_device(kp->input);
	input_dev = NULL;
err2:
//	device_remove_file(&pdev->dev, &dev_attr_enable);
	device_remove_file(&pdev->dev, &dev_attr_virtualEvent);
err1:

	kfree(kp);
	input_free_device(input_dev);

	return -EINVAL;
}

static int kp_remove(struct platform_device *pdev)
{
	struct kp *kp = platform_get_drvdata(pdev);

	/* disable keypad interrupt handling */
	printk(" virtualInputDevice remove kpads \r\n");
	//tasklet_disable(&tasklet);
	//tasklet_kill(&tasklet);

	/* unregister everything */
	input_unregister_device(kp->input);
	//free_pages((unsigned long)remote_log_buf,REMOTE_LOG_BUF_ORDER);
//	device_remove_file(&pdev->dev, &dev_attr_enable);
	device_remove_file(&pdev->dev, &dev_attr_virtualEvent);
	 
	input_free_device(kp->input);


	unregister_chrdev(kp->config_major,kp->config_name);
	if(kp->config_class)
	{
		if(kp->config_dev)
			device_destroy(kp->config_class,MKDEV(kp->config_major,0));
		class_destroy(kp->config_class);
	}

	kfree(kp);
	gp_kp=NULL ;
	return 0;
}

static struct platform_driver kp_driver = {
	.probe      = kp_probe,
	.remove     = kp_remove,
	.suspend    = NULL,
	.resume     = NULL,
	.driver     = {
		.name   = "virtualInputDevice1",
	},
};

static int __devinit kp_init(void)
{
	printk( " virtualInputDevice Keypad Driver\n");

	return platform_driver_register(&kp_driver);
}

static void __exit kp_exit(void)
{
	printk(  " virtualInputDevice Keypad exit \n");
	platform_driver_unregister(&kp_driver);
}

module_init(kp_init);
module_exit(kp_exit);

MODULE_AUTHOR("jianfeng_wang");
MODULE_DESCRIPTION("Keypad Driver");
MODULE_LICENSE("GPL");

