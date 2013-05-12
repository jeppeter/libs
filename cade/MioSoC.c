/*
 *  Copyright (C) 2005-2007 Jiri Slaby <jirislaby@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  You need a userspace library to cooperate with this driver. It (and other
 *  info) may be obtained here:
 *  http://www.fi.muni.cz/~xslaby/phantom.html
 *  or alternatively, you might use OpenHaptics provided by Sensable.
 */
// 要看见这个GBK
/*要看见这个GBK*/
#include <linux/compat.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#include <linux/sched.h>
#include <linux/mutex.h>

#include "MioSoC.h"
#include "Motdata.h"

#include <linux/atomic.h>
#include <asm/io.h>
#include <linux/semaphore.h>


#define MioSoC_VERSION		"n1.0.0"

#define MioSoC_MAX_MINORS	8
#define MIO_IRQCTL		0x4c    /* irq control in caddr space */

//#define PLX_INTCSR	0x4c	/* Interrupt Control and Status Register */

//#define PLX_INT1_ENABLE	0x40	/* Interrupt 1 Enable bit */
//#define PLX_INT1_ACTIVE	0x04	/* Interrupt 1 Active bit */

//#define outw(iAddress, sData ) outw(sData,iAddress)
//#define inw inw

//short	inw( int iAddress );
//void	outw( int iAddress, short sData );

//DECLARE_MUTEX(MioSoC_mutex);

#define  MIOSOC_PAGE_REG                        15
#define  MIOSOC_PAGE_0                          0
#define  MIOSOC_PAGE_1                          1
#define  MIOSOC_PAGE_2                          2
#define  MIOSOC_PAGE_3                          3
#define  MIOSOC_PAGE_4                          4
#define  MIOSOC_PAGE_5                          5
#define  MIOSOC_PAGE_6                          6
#define  MIOSOC_PAGE_7                          7
#define  MIOSOC_PAGE_8                          8
#define  MIOSOC_PAGE_9                          9
#define  MIOSOC_PAGE_10                         10
#define  MIOSOC_PAGE_11                         11
#define  MIOSOC_PAGE_12                         12
#define  MIOSOC_PAGE_13                         13


#define  MIOSOC_PAGE0_RD_INTR_IDX_REG         0x0
#define  MIOSOC_PAGE0_WR_SW_RESET_REG         0x0
#define  MIOSOC_PAGE0_WR_ISA_BUS_WAIT_REG     0x1
#define  MIOSOC_PAGE0_WR_INTR_CHNL_REG        0x2
#define  MIOSOC_PAGE0_WR_INTR_MODE_REG        0x3
#define  MIOSOC_PAGE0_WR_INTR_CLR_REG         0x4


#define  MIOSOC_PAGE1_RD_INTR_CLR_REG         10

#define  MIOSOC_PAGE1_WR_DDA_INTR_REG         12
#define  MIOSOC_PAGE1_WR_DDA_CLK_REG          13

#define  MIOSOC_PAGE2_WR_INTR_ENBL_REG       12
#define  MIOSOC_PAGE2_WR_CLOCK_REG            13        

#define  MIOSOC_PAGE9_WR_ERR_REG              7

#define  MIOSOC_PAGE10_WR_SERIAL_CLK_REG     12

static struct semaphore MioSoC_mutex;

static struct class *MioSoC_class;



#define  MIOSOC_OFF_STATE       0
#define  MIOSOC_INIT_STATE      1

#define  LOCK_REGS(pmio,irqflag)  spin_lock_irqsave(&(pmio->regs_lock),irqflag)

#define  UNLOCK_REGS(pmio,irqflag)  spin_unlock_irqrestore(&(pmio->regs_lock),irqflag)

static int MioSoC_major;
struct MioSoC_device {
    unsigned int opened;
//    u32   caddr;
//    u32   ioaddr;  //不能用u32 __iomem *iaddr; 以为后面要用outw(),inw();否则对应不起来。2013-4-14
                   //iowrite16

    u32   caddr;
    u32   ioaddr;
	u32   state;   // indicate that the state of the device ,we just have 2 state 0 for OFF_STATE  1 INIT_STATE

    //atomic_t counter;
    wait_queue_head_t wait;
    struct cdev cdev;
    //struct mutex open_lock;
    spinlock_t regs_lock;
    struct fasync_struct *async_queue;

};
static int MOT_ISR(struct MioSoC_device* pmio);


int		siVcmdAxis = 0;
double	gdRealIntTime = 0.;
float		gfRealCycleTime = 0.;
unsigned short mode;
int iAxis;

struct MioSoC_device *pmio;

static unsigned char MioSoC_devices[MioSoC_MAX_MINORS];

static const struct file_operations MioSoC_file_ops;
static irqreturn_t MioSoC_isr(int irq, void *data);



int __RegisterDeviceNoLock(struct pci_dev* pdev,int major)
{
	unsigned int minor;
	int retval;
	int i;

	u32 pio_flags1,pio_len1;
	u32 pio_flags3,pio_len3;
	//unsigned long mmio_start, mmio_end, mmio_flags, mmio_len;

	MIO_DEBUG("probe start.\n");//added by guo, to test the probe, 2013-3-22

	retval = pci_enable_device(pdev);
	if (retval) {
		dev_err(&pdev->dev, "pci_enable_device failed!\n");
		goto err;
	}

	minor = MioSoC_MAX_MINORS;
	for(i=0;i<MioSoC_MAX_MINORS;i++)
	{
		if (MioSoC_devices[i]==0)
		{
			minor = i;
			break;
		}
	}

	if (minor == MioSoC_MAX_MINORS) {
		dev_err(&pdev->dev, "too many devices found!\n");
		retval = -EIO;
		goto err_dis;
	}


	retval = pci_request_regions(pdev, "MioSoC");
	if (retval) {
		dev_err(&pdev->dev, "pci_request_regions failed!\n");
		goto err_dis;
	}

	MIO_DEBUG( "pci_request_regions ok.\n");//added by guo, to test the probe, 2013-3-24

	retval = -ENOMEM;
	pmio = kzalloc(sizeof(*pmio), GFP_KERNEL);
	if (pmio == NULL) {
		dev_err(&pdev->dev, "unable to allocate device\n");
		goto err_reg;
	}

	pmio->caddr = pci_resource_start(pdev, 1);
 	pio_flags1 = pci_resource_flags (pdev, 1);
	pio_len1 = pci_resource_len(pdev,1);
 

	/* set this immediately, we need to know before
	 * we talk to the chip directly */
	MIO_DEBUG("PIO region 1 size == 0x%02X\n", pio_len1);

	/* make sure PCI base addr 1 is PIO */
	if (!(pio_flags1 & IORESOURCE_IO)) {
		printk (KERN_INFO  "region #1 not a PIO resource, aborting\n");
		retval = -ENODEV;
		goto err_fr;
	}


	MIO_DEBUG("pmio->caddr: %X\n", pmio->caddr);//added by guo, to test the probe, 2013-3-26


	pmio->ioaddr = pci_resource_start(pdev, 3);

 	pio_flags3 = pci_resource_flags (pdev, 3);
	pio_len3 = pci_resource_len (pdev, 3);


	/* set this immediately, we need to know before
	 * we talk to the chip directly */
	MIO_DEBUG("PIO region 3 size == 0x%02X\n", pio_len3);

	/* make sure PCI base addr 3 is PIO */
	if (!(pio_flags3 & IORESOURCE_IO)) {
		printk (KERN_INFO  "region #3 not a PIO resource, aborting\n");
		retval = -ENODEV;
		goto err_fr;
	}

	MIO_DEBUG("pmio->ioaddr: %x\n", pmio->ioaddr);//added by guo, to test the probe, 2013-3-26


	spin_lock_init(&pmio->regs_lock);
	init_waitqueue_head(&pmio->wait);
	cdev_init(&pmio->cdev, &MioSoC_file_ops);
	pmio->cdev.owner = THIS_MODULE;



	retval = request_irq(pdev->irq, MioSoC_isr,IRQF_SHARED , "MioSoC", pmio);
	if (retval) {
		dev_err(&pdev->dev, "can't establish ISR\n");
		MIO_DEBUG("can not get request irq\n");
		goto err_unmio;
	}

	MIO_DEBUG("pdev->irq: %X\n", pdev->irq);  //added by guo, to test the probe, 2013-3-30


	retval = cdev_add(&pmio->cdev, MKDEV(MioSoC_major, minor), 1);
	if (retval) {
		dev_err(&pdev->dev, "chardev registration failed\n");
		goto err_irq;
	}

	if (IS_ERR(device_create(MioSoC_class, &pdev->dev,
				 MKDEV(MioSoC_major, minor), NULL,
				 "MioSoC%u", minor))){
		dev_err(&pdev->dev, "can't create device\n");
		retval = -ENODEV;
		goto err_cdev_del;
	}

	pci_set_drvdata(pdev, pmio);
	MioSoC_devices[minor] = 1;

	retval = minor;

	MIO_DEBUG("probe ok.\n");//added by guo, to test the probe, 2013-3-22

	return retval;
err_cdev_del:
	cdev_del(&(pmio->cdev));
err_irq:
	free_irq(pdev->irq, pmio);
err_unmio:
	pci_release_regions(pdev);
err_fr:
	kfree(pmio);
err_reg:
	pci_release_regions(pdev);
err_dis:
	pci_disable_device(pdev);
err:
	return retval;
}

int RegisterDevice(struct pci_dev* pdev,int major)
{
	int minor = -1;
	int ret;

	/*we maybe error on down wait*/
	ret = down_interruptible(&MioSoC_mutex);
	if (ret < 0)
	{
		return ret;
	}

	minor = __RegisterDeviceNoLock(pdev,major);
	up(&MioSoC_mutex);
	return minor;
}

int __UnRegisterDeviceNoLock(struct pci_dev* pdev,int major,int minor)
{
    struct MioSoC_device *pmio = pci_get_drvdata(pdev);

	if (MioSoC_devices[minor]==0){
		MIO_ERROR("at[%d] device is not set for it\n",minor);
		return -ENODEV;
	}


	cdev_del(&(pmio->cdev));
	/*because ,we have set the irq not callable ,so we do this ok*/
	free_irq(pdev->irq, pmio);
	pci_release_regions(pdev);
	kfree(pmio);
	pci_release_regions(pdev);
	pci_disable_device(pdev);
    MioSoC_devices[minor] = 0;
	return 0;
}


int UnRegisterDevice(struct pci_dev* pdev,int major,int minor)
{
	int ret;

	/*in unregister we must try best*/
	down(&MioSoC_mutex);
	ret = __UnRegisterDeviceNoLock(pdev,major,minor);
	up(&MioSoC_mutex);
	return ret;
}

struct Para_EnableMioChannel *pPara_EnableMioChannel;
struct Para_WriteLioStatus *pPara_WriteLioStatus;
struct Para_WriteLio2Status *pPara_WriteLio2Status;
struct Para_DisableLdiInt *pPara_DisableLdiInt;
struct Para_StartDda *pPara_StartDda;

static int ISR(struct MioSoC_device* pmio);


unsigned short __ReadMioSocWord(struct MioSoC_device *pmio,int Page,int reg)
{
	unsigned short word;
	/*first to set the page*/
	outw_p(Page,pmio->ioaddr+MIOSOC_PAGE_REG*2);

	/*now to read the register*/
	word = inw_p(pmio->ioaddr+reg*2);
	return word;
}

int __GetMioSocPage(struct MioSoC_device *pmio)
{
	unsigned short val;

	val = inw(pmio->ioaddr + MIOSOC_PAGE_REG*2);
	return (int)val;
}

int __SetMioSocPage(struct MioSoC_device *pmio,int Page)
{
	/*first to set the page*/
	outw_p(Page,pmio->ioaddr+MIOSOC_PAGE_REG*2);
	return 0;
}

int __WriteMioSocWord(struct MioSoC_device *pmio,int Page,int reg,unsigned short word)
{

	__SetMioSocPage(pmio,Page);
	/*now to read the register*/
	outw_p(word,pmio->ioaddr+reg*2);
	return 0;
}


unsigned short __GetMioSocIntrReg(struct MioSoC_device *pmio)
{
	unsigned short intr;

	intr = inw(pmio->caddr + MIO_IRQCTL);
	return intr;
}

unsigned short __EnableMioSocIntr(struct MioSoC_device *pmio)
{
	unsigned short intr;

	intr = inw(pmio->caddr + MIO_IRQCTL);
	intr |= 0x43;

	outw_p(intr,pmio->caddr + MIO_IRQCTL);
	return intr;
}

unsigned short __DisableMioSocIntr(struct MioSoC_device *pmio)
{
	unsigned short intr;

	intr = inw(pmio->caddr + MIO_IRQCTL);
	intr &= (unsigned short)(~0x43);
	outw_p(intr,pmio->caddr + MIO_IRQCTL);
	return intr;
}


int SetMioSocOffState(struct MioSoC_device *pmio)
{
	unsigned long flags;
	int ret=0;
	LOCK_REGS(pmio,flags);

	if (pmio->state != MIOSOC_OFF_STATE)
	{
		/*now to set for the disable*/
		__DisableMioSocIntr(pmio);
		
	}

	pmio->state = MIOSOC_OFF_STATE;

	UNLOCK_REGS(pmio,flags);

	return ret;
}


int __MioSocInitNoLock(struct MioSoC_device *pmio,int minisec)
{
	unsigned int i,divide,bitno;
	unsigned short val,cardtype;

	/*reset all the modules*/
	__WriteMioSocWord(pmio,0,0,0);

	MIO_DEBUG("\n");
	// Set PCI Bridge Interrupt ..........从windows驱动下复制过来，再做修改； 2013-3-26，郭


	/*to enable soc intr*/
	__EnableMioSocIntr(pmio);

	__WriteMioSocWord(pmio,0,0x1f,0);
	cardtype = __ReadMioSocWord(pmio,0,(0x1e));

	/*reset module again */
	__WriteMioSocWord(pmio,0,0,0x300);

	
	for (i=0;i<100;i++)
	{
		__WriteMioSocWord(pmio,0,0,0x00);
	}


	/*to reset the interrupt channel*/
	__WriteMioSocWord(pmio,0,2,(1<<9) | 0xff);


	__WriteMioSocWord(pmio,0,3,0x2);


	/*SetAxisCounter count is 192 */
	val = 3<<6;
	for (i=1;i<10;i++)
	{
		int regcount=0,pagecount=MIOSOC_PAGE_2;
		if (i<4)
		{
			pagecount = MIOSOC_PAGE_2;
			regcount = 4*(i-1);
		}
		else if (i<7)
		{
			pagecount = MIOSOC_PAGE_3;
			regcount = 4*(i-4);
		}
		else if (i < 10)
		{
			pagecount = MIOSOC_PAGE_4;
			regcount = 4*(i-7);
		}
		/*to write lower reg the count and set higher reg 0*/
		__WriteMioSocWord(pmio,pagecount,regcount,val);
		__WriteMioSocWord(pmio,pagecount,regcount+1,0);
	}


	/*start filter sample*/
	__WriteMioSocWord(pmio,MIOSOC_PAGE_2,MIOSOC_PAGE2_WR_CLOCK_REG,0x8000);

	outw((u16)1,pmio->ioaddr + 0x20 + 0x1e );	   //====== InterruptLineInit: //从原来EpcioInit()里移过来。 2013-3-28.郭=============
	outw((u16)0,pmio->ioaddr + 0x20 );


	/*dda unlatch*/
	val = __ReadMioSocWord(pmio,MIOSOC_PAGE_1,10) & 0xff;
	for (i=0;i<7;i++)
	{
		if (val & (1 << i))
		{
			return i + 1;
		}
	}


	/*set int clear*/
	__WriteMioSocWord(pmio,MIOSOC_PAGE_0,4,MIOSOC_PAGE0_WR_SW_RESET_REG);

	/*enable error counter 0x8000 means start plc 0x3f means start channel 0-5*/
	__WriteMioSocWord(pmio,MIOSOC_PAGE_9,MIOSOC_PAGE9_WR_ERR_REG,0x803f);

	/*enable dda serial clock ,0x8000 means star 0x10 is the serial clock divider */
	__WriteMioSocWord(pmio,MIOSOC_PAGE_10,MIOSOC_PAGE10_WR_SERIAL_CLK_REG,0x80);


	for (i=0;i<(MAX_AXIS+3);i++)
	{
		/*for axis enable*/
		val = (1<<i);
		__WriteMioSocWord(pmio,MIOSOC_PAGE_2,MIOSOC_PAGE2_WR_INTR_ENBL_REG,val);

		for (val=0;val<2000;val++)
		{
			/*for delay*/
			__asm__("movl %esi,%esi");
		}

		__WriteMioSocWord(pmio,MIOSOC_PAGE_2,MIOSOC_PAGE2_WR_INTR_ENBL_REG,0);
	}



	for (i=6;i<12;i++)
	{
		/*plus clock is 28 and out format is A/B*/
		__WriteMioSocWord(pmio,MIOSOC_PAGE_1,i,(28|0x1000));
	}


	if (minisec == 0)
	{
		minisec = 10;
	}


	divide = (unsigned int)((40*1000*minisec/32768));
	/*15 bits dda*/
	bitno = 0x5000;


	val = (divide | bitno);
	__WriteMioSocWord(pmio,MIOSOC_PAGE_1,MIOSOC_PAGE1_WR_DDA_CLK_REG,val);


	/*to enable dda cycle ,and not minimum stock count and emergency stop DDA machin and dda[0:5] start*/
	__WriteMioSocWord(pmio,MIOSOC_PAGE_1,MIOSOC_PAGE1_WR_DDA_INTR_REG,0x40bf);

	MIO_DEBUG("\n");
	return 0;

}

int MioSocInit(struct MioSoC_device *pmio,int minisec)
{
	int ret;
	unsigned long flags;

	LOCK_REGS(pmio,flags);
	ret = __MioSocInitNoLock( pmio,minisec);
	UNLOCK_REGS(pmio,flags);
	
	return ret;
}
static long MioSoC_ioctl(struct file *file, unsigned int cmd,
        unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    struct MioSoC_device *pmio = file->private_data;
    int iMiniSec;
	int ret;
//struct Para_MioSoCinit *pPara_MioSoCinit;
	ret = 0;
    switch (cmd)
    {

        case MioSoCinit:

			if (copy_from_user(&iMiniSec, argp, sizeof(iMiniSec)))
			{
				return -EFAULT;
			}

			ret = MioSocInit(pmio,iMiniSec);

            break;
        case ResetAllModules://

             //==========================================================================
             // Function   : void ResetAllModules
             // Parameters : ( int iBaseAddress )
             // Purpose    : 重置EPCIO的动作
             // Return     : 无
             //==========================================================================
             {
             int iDelay;
             SetPage(0);
             outw( (u16)0x300,pmio->ioaddr );

             for(iDelay=0;iDelay<200;iDelay++);
             outw((u16) 0,pmio->ioaddr  );
             }
         break;
        case GetCardAxisNum:       //int GetCardAxisNum(int iFpgaBaseAddress)
                //==========================================================================
                // Function   : int GetCardAxisNum(iFpgaBaseAddress)
                // Parameters : (iFpgaBaseAddress)
                // Purpose    : 由FPGA判别轴卡的轴数
                // Return     : 请参照电子硬件地址分配
                //				3 => 三轴版
                //				其它 => 六轴版
                //==========================================================================

                outw(0,pmio->ioaddr + 0x20+0x1e );
                return inw(pmio->ioaddr + 0x20+0x1a);

            break;
        case DisableIndexInt:  //      int DisableIndexInt(int iAxis)
                //==========================================================================
                // Function   : DisableIndexInt
                // Parameters : (int iAxis)
                // Purpose    : Disable Encoder Comparator Interrupt
                // Return     : 0  => 成功
                //              -1 => Axis error (0~8)
                //==========================================================================

                {
                unsigned int val=0;
                unsigned int iState=0;

                if( iAxis<0 || iAxis>=MAX_ENCODER )
                    return -1;

                giEnableTriggerCounterLatch[iAxis] = giEnableTriggerCounterLatch[iAxis] & (~(1<<iAxis));

                val = 1<<(iAxis);

                if( (giEnableIndexAxes & val) == val )
                    giEnableIndexAxes = giEnableIndexAxes^( 1<< (iAxis) ) ;

                if(iAxis < 3)
                {
                    /* for axis 0,1,2 */
                    iState = (giEnableEncoderComparatorAxes & 0x7) | ((giEnableIndexAxes & 0x7) << 3);

                    SetPage(2);
                    outw(iState,pmio->ioaddr+12*2);
                }
                else if(iAxis >= 3 && iAxis < 6)
                {
                    /*for axis 3,4,5 */
                    iState = ((giEnableEncoderComparatorAxes & 0x38) >> 3) | (giEnableIndexAxes & 0x38);

                    SetPage(3);
                    outw(iState,pmio->ioaddr+12*2);
                }
                else if(iAxis<9)
                {
                    /*for axis 6,7,8 */
                    iState = ((giEnableEncoderComparatorAxes & 0x1C0) >> 6) | ((giEnableIndexAxes & 0x1C0) >> 3);
                    SetPage(4);
                    outw(iState,pmio->ioaddr+12*2);
                }

                return ON;
                }
            break;
        case DisableLdiInt:   //             int DisableLdiInt(int iNum, int iAxis)
                //Motion Code Ver: 92.12.09
                //==========================================================================
                // Function   : DisableLdiInt
                // Parameters : (int iNum) => LDI number (0~7)
                //		(int iAxis) => Axis Number (0~8)
                // Purpose    : Disable Local Input Interrupt
                // Return     :  0 => 成功
                //              -1 => iNum error (0~7)
                //              -2 => iAxis error (0~8)
                //==========================================================================
                {
                int iCase=0x00;

                if( pPara_DisableLdiInt->iNum<0 || pPara_DisableLdiInt->iNum>=MAX_LDI_INT )
                    return -1;

                if( pPara_DisableLdiInt->iAxis<0 || pPara_DisableLdiInt->iAxis>=MAX_ENCODER )
                    return -2;

                if(pPara_DisableLdiInt->iNum==0 || pPara_DisableLdiInt->iNum==1)
                    giEnableTriggerCounterLatch[pPara_DisableLdiInt->iAxis] = giEnableTriggerCounterLatch[pPara_DisableLdiInt->iAxis] & (~(1<<(pPara_DisableLdiInt->iNum+9)));

                giEnableLdiIntAxes = giEnableLdiIntAxes & (~(0x03<<(pPara_DisableLdiInt->iNum*2)));
                giEnableLdiIntAxes = giEnableLdiIntAxes | iCase<<(pPara_DisableLdiInt->iNum*2);

                SetPage(8);
                outw(giEnableLdiIntAxes, pmio->ioaddr+2*3);

                return 0;
                }
            break;
        case WriteLioStatus://               int WriteLioStatus(int iNum, int iValue)
                //==========================================================================
                // Function   : int WriteLioStatus(int iNum, int iValue)
                // Parameters : (int iNum, int iValue)(0~~27)
                // Purpose    : 写入LIO(0~27)  0 : OFF	 1 : ON
                // Return     : 0 => 成功
                //				   -1 => iNum参数值域错误(0~27)
                //				   -2 => iValue参数值域错误(0~1)
                //==========================================================================

                //Motion Code Ver: 93.03.25(1)
                //unsigned short sState1 = 0xFFFF;
                //unsigned short sState2 = 0x0FFF;
                //

                if( pPara_WriteLioStatus->iValue == 1 )   //因为硬件为反向作动,故在此颠倒输入值
                    pPara_WriteLioStatus->iValue = 0;
                else
                    pPara_WriteLioStatus->iValue = 1;

                if(pPara_WriteLioStatus->iNum < 0 || pPara_WriteLioStatus->iNum > 27)
                    return -1;

                if(pPara_WriteLioStatus->iValue != 0 && pPara_WriteLioStatus->iValue != 1)
                    return -2;

                SetPage(8);

                if(pPara_WriteLioStatus->iNum >= 0 && pPara_WriteLioStatus->iNum < 16)
                {
                    if( pPara_WriteLioStatus->iValue == 1)
                        sWriteLioState1 = (unsigned short)(sWriteLioState1 | (pPara_WriteLioStatus->iValue << pPara_WriteLioStatus->iNum));
                    else
                        sWriteLioState1 = (unsigned short)(sWriteLioState1 ^ (1 << pPara_WriteLioStatus->iNum));

                    outw(sWriteLioState1,pmio->ioaddr);
                }
                else
                {
                    if( pPara_WriteLioStatus->iValue == 1)
                        sWriteLioState2 = (unsigned short)(sWriteLioState2 | (pPara_WriteLioStatus->iValue << (pPara_WriteLioStatus->iNum - 16)));
                    else
                    {
                        if((sWriteLioState2 & (1 << (pPara_WriteLioStatus->iNum -16))) != 0 )
                        { sWriteLioState2 = (unsigned short)(sWriteLioState2 ^ (1 << (pPara_WriteLioStatus->iNum -16))); }
                    }

                    outw(sWriteLioState2,pmio->ioaddr + 2*1);
                }

                    return 0;

            break;
        case WriteLio2Status://               int WriteLio2Status(int iNum, int iValue)
                //==========================================================================
                // Function   : int WriteLio2Status
                // Parameters : (int iNum) => Outout number(0~31)
                //				(int iValue) => 0:OFF
                //								1:ON
                // Purpose    : CPLD local ouput(0~31)
                // Return     : 0 => OK
                //				-1 => iNum error(0~31)
                //				-2 => iValue error(0~1)
                //==========================================================================

                //Motion Code Ver: 93.03.25(1)
                //unsigned short sState1 = 0x0000;
                //unsigned short sState2 = 0x0000;
                //

                //因为CPLD硬件为不会反向作动,故在此取消颠倒
                //if( iValue == 1 )
                //   iValue = 0;
                //else
                //    iValue = 1;

                if(pPara_WriteLio2Status->iNum < 0 || pPara_WriteLio2Status->iNum >= MAX_FPGA_LDO_NUM)
                    return -1;

                if(pPara_WriteLio2Status->iValue != 0 && pPara_WriteLio2Status->iValue != 1)
                    return -2;

                SetFpgaPage(1);

                if(pPara_WriteLio2Status->iNum >= 0 && pPara_WriteLio2Status->iNum < 16)
                {
                    if( pPara_WriteLio2Status->iValue == 1)
                        sWriteLio2State1 = (unsigned short)(sWriteLio2State1 | (pPara_WriteLio2Status->iValue << pPara_WriteLio2Status->iNum));
                    else
                        sWriteLio2State1 = (unsigned short)(sWriteLio2State1 & ~(1 << pPara_WriteLio2Status->iNum));

                    outw(sWriteLio2State1,pmio->ioaddr + 0x20+0x04);
                }
                else
                {
                    if( pPara_WriteLio2Status->iValue == 1)
                        sWriteLio2State2 = (unsigned short)(sWriteLio2State2 | (pPara_WriteLio2Status->iValue << (pPara_WriteLio2Status->iNum - 16)));
                    else
                        sWriteLio2State2 = (unsigned short)(sWriteLio2State2 & ~(1 << (pPara_WriteLio2Status->iNum -16)));

                    outw(sWriteLio2State2,pmio->ioaddr + 0x20+0x06);
                }

                    return 0;

            break;
        case ResetIRQNo: //               int ResetIRQNo()
                //==========================================================================
                // Function   : int ResetIRQNo()
                // Parameters : 无
                // Purpose    : 重置IRQ
                // Return     : 无(PCI界面才用到的函式)
                //==========================================================================

                SetPage(0);
                outw(0x00ff,pmio->ioaddr + 2*2);
                return 1;

            break;
    default:
        return -ENOTTY;

    }
    return ret;
}


static int MioSoC_open(struct inode *inode, struct file *file)
{
    struct MioSoC_device *dev = container_of(inode->i_cdev,
            struct MioSoC_device, cdev);


    if (dev->opened) {
        return -EINVAL;
    }

    nonseekable_open(inode, file);

    file->private_data = dev;
	SetMioSocOffState(dev);
    dev->opened++;
    return 0;
}

static int MioSoC_release(struct inode *inode, struct file *file)
{
    struct MioSoC_device *dev = file->private_data;

    //mutex_lock(&dev->open_lock);

    dev->opened = 0;
	SetMioSocOffState(dev);
    //MioSoC_status(dev, dev->status & ~PHB_RUNNING);
   // dev->status &= ~PHB_NOT_OH;

    //mutex_unlock(&dev->open_lock);

    return 0;
}


static int MioSoC_fasync(int fd, struct file *filp, int mode)
{
    struct MioSoC_device     *dev = filp->private_data;

    printk("MioSoC_fasync()++\n");

    fasync_helper(fd, filp, mode, &dev->async_queue);

    printk("MioSoC_fasync()--\n");

    return 0;
}


static const struct file_operations MioSoC_file_ops = {
    .open = MioSoC_open,
    .release = MioSoC_release,
    .unlocked_ioctl = MioSoC_ioctl,
    .fasync = MioSoC_fasync,
    //.compat_ioctl = MioSoC_compat_ioctl,  marked by guo
    //.poll = MioSoC_poll,
    //.llseek = no_llseek,
};


int iIsrCount1;

static irqreturn_t MioSoC_isr(int irq, void *data)
{
    //struct fpga_key_dev *dev = &fpga_key_dev;
    struct MioSoC_device *pmio = data;
	unsigned long flags;
	irqreturn_t ret=IRQ_NONE;
    //unsigned int i;
    //u32 ctl;
    unsigned int dwIntStatus;
	LOCK_REGS(pmio,flags);

	/*read intr status*/
    dwIntStatus = __GetMioSocIntrReg(pmio);

        if ((unsigned int)dwIntStatus & 0x4)              // 检查是否为本卡的中断
        {

			__DisableMioSocIntr(pmio);

            MOT_ISR(pmio);
//            iIsrCount1++;
//            printk(KERN_INFO "iIsrCount1: %d\n", iIsrCount1);//added by guo, to test the probe, 2013-4-10

            if (pmio->async_queue)
               kill_fasync(&pmio->async_queue, SIGIO, POLL_IN);
			__EnableMioSocIntr(pmio);
			ret = IRQ_HANDLED;

        }

	UNLOCK_REGS(pmio,flags);

    //inw(dev->ioaddr); /* PCI posting */

    //atomic_inc(&dev->counter);
    if (ret == IRQ_HANDLED)
    {
    	wake_up_interruptible(&pmio->wait);
    }

    return ret;
}
//==========================================================================
// Function   : void MOT_ISR(void)
// Parameters : 无
// Purpose    : Motion中断服务程序
// Return     : 无
//==========================================================================
static int MOT_ISR(struct MioSoC_device* pmio)
{
    int iIntSource = 0;
    int iIsrLoop = 0;
    int iOldPage = 0;

    // 用以纪录是哪一个index触发了中断
    //long lIndexSrc = 0;
    //Motion Code Ver: 92.12.09
    //int iLdiDfiIntSrc = 0;
    //int iRemoteSet0IntSrc = 0;
    //int iRemoteSet1IntSrc = 0;
    //
    //Motion Code Ver: 93.12.29(2)

    //



    iOldPage=__GetMioSocPage(pmio);

    //for Vcmd_G31, marked by maverick 2004/6/18 05:39PM
    //iIntSource=ReadIntSource();
    //Motion Code Ver: 92.12.09
    //iInterruptSource = iIntSource;
    //
    //

    while((iIntSource=__GetMioSocIntrReg(pmio))!=0 && iIsrLoop< 3)
    {
        //for Vcmd_G31, added by maverick 2004/6/18 05:39PM
//        iInterruptSource = iIntSource;
        //
        if((iIntSource & 0x1) == 0x1)
        {
        	DdaUnlatchFunc(pmio);
		}

//        if(iIntSource & 0xE)
//            { lIndexSrc = ENCODERUnlatch(iIntSource); }

//        //Motion Code Ver: 93.12.29(2)
//        if((iIntSource & 0x100) == 0x100) // error counter overflow
//            iErrCntChannel = ErrorCounterUnlatch();
//        //

//        //Motion Code Ver: 92.12.09
//        if(iIntSource & 0x80)
//            { iLdiDfiIntSrc = LdiDfiIntUnlatch(); }

//        if(iIntSource & 0x10)
//            { iRemoteSet0IntSrc = RemoteInputIntUnlatch(0); }

//        if(iIntSource & 0x20)
//            { iRemoteSet1IntSrc = RemoteInputIntUnlatch(1); }
        //

    /* Execute the corresponding procedures thereafter   */
        if((iIntSource & 0x1) == 0x1)
        {

            ISR(pmio);

        }

//        //Motion Code Ver: 93.12.29(2)
//        if((iIntSource & 0x100) == 0x100) // error counter overflow
//            ErrCntOverflowISR(iErrCntChannel);
//        //

//        if(iIntSource & 0xE)
//            { IndexISR(lIndexSrc); }

//        //Motion Code Ver: 92.12.09
//        if(iIntSource & 0x80)
//            { LdiDfiIntISR(iLdiDfiIntSrc); }

//        if(iIntSource & 0x10)
//            { RemoteInputIntISR(0,iRemoteSet0IntSrc); }

//        if(iIntSource & 0x20)
//            { RemoteInputIntISR(1,iRemoteSet1IntSrc); }
        //

    iIsrLoop++;
    }

    iIsrLoop=0;

    __SetMioSocPage(pmio, iOldPage );

    return 0;

}

static int ISR(struct MioSoC_device* pmio)
{

//    iIsrCount1++;
//    printk(KERN_INFO "iIsrCount1: %d\n", iIsrCount1);//added by guo, to test the probe, 2013-4-10



    return 0;
}
//==========================================================================
// Function   : int DdaUnlatch(void)
// Parameters : 无
// Purpose    : dda中断清除动作
// Return     : 目前无意义
//==========================================================================
int DdaUnlatchFunc(struct MioSoC_device* pmio)
{
     unsigned int uiSource;

	 uiSource = __ReadMioSocWord(pmio,MIOSOC_PAGE_1,MIOSOC_PAGE1_RD_INTR_CLR_REG) & 0xff;

      if(uiSource & 1)
     {
            return 1 ; /* FIFO 0 with minimum stock*/
     }
     else if(uiSource & (1<<1) )
     {
          return 2; /*  FIFO 1 with minimum stock*/
     }
     else if(uiSource & (1<<2) )
     {
            return 3; /*  FIFO 2 with minimum stock*/
     }
     else if(uiSource & (1<<3) )
     {
          return 4; /*  FIFO 3 with minimum stock */
     }
     else if(uiSource & (1<<4) )
     {
          return 5; /*  FIFO 4 with minimum stock*/
     }
     else if(uiSource & (1<<5) )
     {
          return 6;  /*  FIFO 5 with minimum stock*/
     }
     else if(uiSource & (1<<6) )
     {
          return 7; /* DDA Cycle*/
     }
     return 0;
}

//==========================================================================
// Function   : int ReadIntSource(void)
// Parameters : 无
// Purpose    : 读取中断来源位置
// Return     : epcio 中断状态值
//==========================================================================
int ReadIntSourceFunc(void)
{
     unsigned int uiSource;

     SetPage(0);
     uiSource=inw( pmio->ioaddr)& 0x01ff;

    return uiSource;

}


static int __devinit MioSoC_probe(struct pci_dev *pdev, const struct pci_device_id *pci_id)
{
	int retval;

	/*return value is minor*/
	retval = RegisterDevice(pdev,MioSoC_major);
	if(retval < 0)
	{
		return retval;
	}
	return 0;
}


static void __devexit MioSoC_remove(struct pci_dev *pdev)
{
    struct MioSoC_device *pmio = pci_get_drvdata(pdev);
    unsigned int minor = MINOR(pmio->cdev.dev);

	UnRegisterDevice(pdev,MioSoC_major,minor);
	return;
}


/*  参考了8139too.c的写法， probe 开始生效；*/
static DEFINE_PCI_DEVICE_TABLE(MioSoC_pci_tbl) = {
    {0x10b5, 0x9050, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },

    {0,}
};

MODULE_DEVICE_TABLE(pci, MioSoC_pci_tbl);


static struct pci_driver MioSoC_pci_driver = {
    .name = "MioSoC",
    .id_table = MioSoC_pci_tbl,
    .probe = MioSoC_probe,
    .remove = __devexit_p(MioSoC_remove),
   // .suspend = MioSoC_suspend   marked by guo,2013,3-17
   // .resume = MioSoC_resume    marked by guo,2013,3-17
};



static CLASS_ATTR_STRING(version, 0444, MioSoC_VERSION);

static int __init MioSoC_init(void)
{
    int retval;
    dev_t dev;


	sema_init(&MioSoC_mutex,1);
    MioSoC_class = class_create(THIS_MODULE, "MioSoC");
    if (IS_ERR(MioSoC_class)) {
        retval = PTR_ERR(MioSoC_class);
        MIO_ERROR("MioSoC: can't register MioSoC class\n");
        goto err;
    }
    retval = class_create_file(MioSoC_class, &class_attr_version.attr);
    if (retval) {
        MIO_ERROR( "MioSoC: can't create sysfs version file\n");
        goto err_class;
    }

    retval = alloc_chrdev_region(&dev, 0, MioSoC_MAX_MINORS, "MioSoC");
    if (retval) {
        MIO_ERROR("MioSoC: can't register character device\n");
        goto err_attr;
    }
    MioSoC_major = MAJOR(dev);

    retval = pci_register_driver(&MioSoC_pci_driver);
    if (retval) {
        MIO_ERROR("MioSoC: can't register pci driver\n");
        goto err_unchr;
    }

    printk(KERN_INFO "MioSoC Linux Driver, version " MioSoC_VERSION ", "
            "init OK\n");

    return 0;
err_unchr:
    unregister_chrdev_region(dev, MioSoC_MAX_MINORS);
err_attr:
    class_remove_file(MioSoC_class, &class_attr_version.attr);
err_class:
    class_destroy(MioSoC_class);
err:	
    return retval;
}

static void __exit MioSoC_exit(void)
{
    pci_unregister_driver(&MioSoC_pci_driver);

    unregister_chrdev_region(MKDEV(MioSoC_major, 0), MioSoC_MAX_MINORS);

    class_remove_file(MioSoC_class, &class_attr_version.attr);
    class_destroy(MioSoC_class);

    pr_debug("MioSoC: module successfully removed\n");
	
}

module_init(MioSoC_init);
module_exit(MioSoC_exit);

MODULE_AUTHOR("Guo Yunfei <guoyunfe@foxmail.com>");
MODULE_DESCRIPTION("Motion and IO system on Chip driver (PCI devices)");
MODULE_LICENSE("GPL");
MODULE_VERSION(MioSoC_VERSION);
