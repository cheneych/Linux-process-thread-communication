/*
 * lab6.c
 *
 *  Created on: Nov 10, 2016
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
unsigned long *RAWINTSTSB;
unsigned long *ptr;

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
			printk("high\n");
			*PFDR=0xFF;
			flag=1;
		}
		else
		{
			printk("low\n");
			*PFDR=0xFD;
			flag=0;
		}
		rt_task_wait_period();  //wait for some time
	}

}

static void HW_ISR(unsigned irq_num,void *cookie)
{
	unsigned long temp;
	rt_disable_irq(59); //to avoid multi pressed buttons
	//change frequency
	temp=*RAWINTSTSB;
	//printk("%lu\n",temp);
	if ((temp &=0X01)==1)
	{
		rt_task_make_periodic(&mytask,rt_get_time(),2*period);
		printk("interrupt 1\n");
	}
	else
	{
		temp=*RAWINTSTSB;
		if ((temp &= 0X02)==2)
		{
			rt_task_make_periodic(&mytask,rt_get_time(),3*period);
			printk("interrupt 2\n");
		}
		else
		{
			temp=*RAWINTSTSB;
			if ((temp &= 0X04)==4)
			{
				rt_task_make_periodic(&mytask,rt_get_time(),4*period);
				printk("interrupt 3\n");
			}
			else
			{
				temp=*RAWINTSTSB;
				if ((temp &= 0X08)==8)
				{
					printk("interrupt 4\n");
					rt_task_make_periodic(&mytask,rt_get_time(),5*period);

				}
				else
				{
					temp=*RAWINTSTSB;
					if ((temp &= 0X10)==16)

					{
						rt_task_make_periodic(&mytask,rt_get_time(),1*period);
						printk("interrupt 5\n");
					}
				}
			}
		}
	}
	*PBEOI |= 0x1F;
	rt_enable_irq(59);
}

int init_module(void)
{
	printk("init module");
	//time slice
	rt_set_periodic_mode();
	period=start_rt_timer(nano2count(300000));
	//start the task
	rt_task_init(&mytask,rt_process,0,256,0,0,0);
	//Schedule the task
	rt_task_make_periodic(&mytask,rt_get_time(),6*period);
	rt_request_irq(59,HW_ISR,0,1);   	//bind the interrupt
	//configuration
	ptr=(unsigned long*)__ioremap(0x80840000,4096,0);
	PBINTTYPE1=ptr+43;
	PBINTTYPE2=ptr+44;
	PBEOI=ptr+45;
	PBINTEN=ptr+46;
	RAWINTSTSB=ptr+48;
	//initialize registers in portB
	*PBEOI |= 0x1F;
	*PBINTEN |= 0x1F;
	*PBINTTYPE1 |= 0X01;
	*PBINTTYPE2 &= 0X00;
	rt_enable_irq(59);  //enable the interrupt

	return 0;
}

void cleanup_module(void)
{
	rt_task_delete(&mytask);
	stop_rt_timer();
	rt_disable_irq(59);
	rt_release_irq(59);
}




