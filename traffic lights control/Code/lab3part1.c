/*
 * lab3part1.c
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
static RT_TASK mytask;
RTIME period;

static void rt_process(int t)
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

		*PBDR |=0x9F;   //green light on
		rt_sleep(1000000);  //time length

		*PBDR &=0X00;   //back to original status


		*PBDR |=0x5F; //yellow light on
		rt_sleep(1000000);  //time length


		*PBDR &=0X00; //back to original status

     if (check_button()==1)     //check if the button pressed
     {

    	 *PBDR |=0x3F;   //red light on
    	 rt_sleep(1000000); //time length


    	clear_button();  //pop the button  
    	*PBDR &=0X00;   //back to original status

     }

	}
}

int init_module(void)
{


	rt_set_periodic_mode();
	period=start_rt_timer(nano2count(1000000)); //time slice

	rt_task_init(&mytask,rt_process,0,256,0,0,0);
	rt_task_resume(&mytask);  //start the task
	return 0;
}

void cleanup_module(void)
{
	rt_task_delete(&mytask);
	stop_rt_timer();
}




