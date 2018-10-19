/*
 * lab6k.c
 *
 *  Created on: Nov 16, 2016
 *      Author: ccv5f
 */

#ifndef MODULE
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <asm/io.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
//#include <sys/mman.h>



MODULE_LICENSE("GPL");


static RT_TASK mytask;
RTIME period;
//reflect the physical address to memory space
unsigned long *PBDR,*PBDDR,*PFDR,*PFDDR;
unsigned long *PBEOI,*PBINTEN,*PBINTTYPE1,*PBINTTYPE2;
unsigned long *VIC2INTEN,*VIC2SINT,*VIC2SINTCLEAR;
unsigned long *RAWINTSTSB;
unsigned long *ptr,*ptr2;
char getfrom[40];

static void rt_process(int t)
{
	//according to their physical address
	PBDR=ptr+1;
	PBDDR=ptr+5;
	PFDR=ptr+12;
	PFDDR=ptr+13;
	//give input/output
	*PBDDR |= 0xE0;
	*PFDDR |= 0x02;
	int flag=0;
	while (1)
	{
		if (flag==0)
		{
			//printk("high\n");
			*PFDR=0xFF;
			flag=1;
		}
		else
		{
			//printk("low\n");
			*PFDR=0xFD;
			flag=0;
		}
		rt_task_wait_period();  //wait for some time
	}

}

static void SW_ISR(unsigned irq_num,void *cookie)
{
	rt_disable_irq(63); //to avoid multi interrupt signals
	//get the frequency from fifo
	printk("before fifo\n");
	rtf_get(1,&getfrom,sizeof(getfrom));  //get from the fifo
	//change frequency
	printk("after fifo\n");
	printk("%c\n",getfrom[1]);
	switch (getfrom[1])
	{
	case 65:
		rt_task_make_periodic(&mytask,rt_get_time(),2*period);
		break;
	case 66:
		rt_task_make_periodic(&mytask,rt_get_time(),3*period);
		break;
	case 67:
		rt_task_make_periodic(&mytask,rt_get_time(),4*period);
		break;
	case 68:
		rt_task_make_periodic(&mytask,rt_get_time(),5*period);
		break;
	case 69:
		rt_task_make_periodic(&mytask,rt_get_time(),1*period);
		break;
	default:
		break;
	}
	printk("why\n");
	*VIC2SINTCLEAR |= 0x80000000; //clear interrupt
	rt_enable_irq(63);
}

static void HW_ISR(unsigned irq_num,void *cookie)
{
	unsigned long temp;
	char cha[40];
	rt_disable_irq(59); //to avoid multi pressed buttons
	//change frequency
	temp=*RAWINTSTSB;
	if ((temp &=0X01)==1)
	{
		printk("interrupt 1\n");
		rt_task_make_periodic(&mytask,rt_get_time(),2*period);

	   sprintf(cha,"@A");
		rtf_put(2,&cha,sizeof(cha));
	}
	else
	{
		temp=*RAWINTSTSB;
		if ((temp &= 0X02)==2)
		{
			printk("interrupt 2\n");
			rt_task_make_periodic(&mytask,rt_get_time(),3*period);

			sprintf(cha,"@B");
			rtf_put(2,&cha,sizeof(cha));
		}
		else
		{
			temp=*RAWINTSTSB;
			if ((temp &= 0X04)==4)
			{
				printk("interrupt 3\n");
				rt_task_make_periodic(&mytask,rt_get_time(),4*period);

				sprintf(cha,"@C");

				rtf_put(2,&cha,sizeof(cha));
			}
			else
			{
				temp=*RAWINTSTSB;
				if ((temp &= 0X08)==8)
				{
					printk("interrupt 4\n");
					rt_task_make_periodic(&mytask,rt_get_time(),5*period);
					sprintf(cha,"@D");

					rtf_put(2,&cha,sizeof(cha));

				}
				else
				{
					temp=*RAWINTSTSB;
					if ((temp &= 0X10)==16)

					{
						printk("interrupt 5\n");
						rt_task_make_periodic(&mytask,rt_get_time(),1*period);

						sprintf(cha,"@E");

						rtf_put(2,&cha,sizeof(cha));
					}
				}
			}
		}
	}
	*PBEOI |= 0x1F;  //clear interrupt
	rt_enable_irq(59);
}

int init_module(void)
{
	//time slice
	rt_set_periodic_mode();
	period=start_rt_timer(nano2count(300000));
	//start the task
	rt_task_init(&mytask,rt_process,0,256,0,0,0);
	//Schedule the task
	rt_task_make_periodic(&mytask,rt_get_time(),6*period);
	rtf_create(1,1*sizeof(unsigned long)); //create the fifo
	rtf_create(2,1*sizeof(unsigned long)); //create another fifo
	rt_request_irq(59,HW_ISR,0,1);   	//bind the hardware interrupt
	rt_request_irq(63,SW_ISR,0,1);      //bind the software interrupt
	//configuration
	ptr=(unsigned long*)__ioremap(0x80840000,4096,0);
	ptr2=(unsigned long*)__ioremap(0x800C0000,4096,0);
	//registers related to interrupt 59
	PBINTTYPE1=ptr+43;
	PBINTTYPE2=ptr+44;
	PBEOI=ptr+45;
	PBINTEN=ptr+46;
	RAWINTSTSB=ptr+48;
	//registers related to interrupt 63
	VIC2INTEN=ptr2+4;
	VIC2SINT=ptr2+6;
	VIC2SINTCLEAR=ptr2+7;
	//initialize registers in portB
	*PBEOI |= 0x1F;
	*PBINTEN |= 0x1F;
	*PBINTTYPE1 |= 0X01;
	*PBINTTYPE2 &= 0X00;
	//initialize registers related to interrupt 63
	*VIC2INTEN |= 0x80000000;
	*VIC2SINT  &= 0x00000000;
	*VIC2SINTCLEAR |= 0x80000000;
	rt_enable_irq(59);  //enable the hardware interrupt
	rt_enable_irq(63);  //enable the software interrupt
	return 0;
}

void cleanup_module(void)
{
	rtf_destroy(1);   //destroy the fifo
	rtf_destroy(2);
	rt_task_delete(&mytask);
	stop_rt_timer();
	rt_disable_irq(59);
	rt_release_irq(59);
	rt_disable_irq(63);
	rt_release_irq(63);
}




