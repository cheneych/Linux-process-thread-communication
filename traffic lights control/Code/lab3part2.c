/*
 * lab3part2.c
 *
 *  Created on: Sep 28, 2016
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
#include <asm/io.h>
#include "ece4220lab3.h"
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_sem.h>


MODULE_LICENSE("GPL");

SEM sem;
static RT_TASK mytask1,mytask2,mytask3;
RTIME period;

static void rt_process1(int t)
{
	unsigned long *PBDR,*PBDDR;
							unsigned long *ptr;
							ptr=(unsigned long*)__ioremap(0x80840000,4096,0);
							PBDR=ptr+1;
							PBDDR=ptr+5;
							*PBDDR |=0xE0;
							*PBDR &=0x00;

 while (1)
	{
     rt_sem_wait(&sem);

		*PBDR |=0x9F;
		rt_sleep(1000000);

		*PBDR &=0X00;
		rt_sem_signal(&sem);

	}
}

static void rt_process2(int t)
{
	unsigned long *PBDR,*PBDDR;
							unsigned long *ptr;
							ptr=(unsigned long*)__ioremap(0x80840000,4096,0);
							PBDR=ptr+1;
							PBDDR=ptr+5;
							*PBDDR |=0xE0;
							*PBDR &=0x00;

 while (1)
	{
     rt_sem_wait(&sem);
		*PBDR |=0x5F;
		rt_sleep(1000000);


		*PBDR &=0X00;
		rt_sem_signal(&sem);

    	}
}

static void rt_process3(int t)
{

	unsigned long *PBDR,*PBDDR;
							unsigned long *ptr;
							ptr=(unsigned long*)__ioremap(0x80840000,4096,0);
							PBDR=ptr+1;
							PBDDR=ptr+5;
							*PBDDR |=0xE0;
							*PBDR &=0x00;
 while (1)
	{
	  rt_sem_wait(&sem);
     if (check_button()==1)
     {


    	 *PBDR |=0x3F;
    	 rt_sleep(1000000);


    	clear_button();
    	*PBDR &=0X00;


     }
     rt_sem_signal(&sem);

	}
}

int init_module(void)
{


    int value;
    value=1;

	rt_set_periodic_mode();
	period=start_rt_timer(nano2count(1000000));

	rt_sem_init(&sem,value);
	rt_task_init(&mytask1,rt_process1,0,256,0,0,0);
	rt_task_init(&mytask2,rt_process2,0,256,1,0,0);
	rt_task_init(&mytask3,rt_process3,0,256,0,0,0);

	rt_task_resume(&mytask1);
	rt_task_resume(&mytask2);
	rt_task_resume(&mytask3);
	return 0;
}

void cleanup_module(void)
{
	rt_task_delete(&mytask1);
	rt_task_delete(&mytask2);
	rt_task_delete(&mytask3);
	rt_sem_delete(&sem);
	stop_rt_timer();
}





