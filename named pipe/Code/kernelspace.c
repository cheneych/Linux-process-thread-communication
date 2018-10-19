/*
 * lab42.c
 *
 *  Created on: Oct 19, 2016
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
#include <rtai_sem.h>

MODULE_LICENSE("GPL");

static RT_TASK mytask;
RTIME period;

static void rt_process(int t)
{
	unsigned long *PBDR,*PBDDR;
	struct timeval stamp;
	unsigned long time1;   //time-stamp
	//char buf;
	unsigned long *ptr;
	ptr=(unsigned long*)__ioremap(0x80840000,4096,0);
	PBDR=ptr+1;
	PBDDR=ptr+5;
	*PBDDR |=0xE0;		// 1110 0000
	//*PBDR &=0x00;     //initialize
	while (1)
	{
		if ((*PBDR &=0X01)==0)     //check if the button pressed 0001 1110
		{
			//printk("%ld\n",*PBDR);
			do_gettimeofday(&stamp);
			time1=(stamp.tv_sec)*1000000+(stamp.tv_usec); //get the time
			//buf=time1;  //transmit time to buffer


			rtf_put(1,&time1,sizeof(time1));  //put into the buffer
			//rt_sleep(100);
		}
		//*PBDR &=0X1F;   //back to original status
		rt_task_wait_period();  //wait for some time
	}
}

int init_module(void)
{



	rt_set_periodic_mode();
	period=start_rt_timer(nano2count(1000000));
	rt_task_init(&mytask,rt_process,0,256,0,0,0);    //initialize the tasks

	rt_task_make_periodic(&mytask,rt_get_time(),75*period);//set the period
	rtf_create(1,1*sizeof(unsigned long)); //create the fifo




	return 0;
}

void cleanup_module(void)
{
	rt_task_delete(&mytask);  //destroy the task
	stop_rt_timer();
	rtf_destroy(0);   //destroy the fifo
}

