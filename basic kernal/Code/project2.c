/*
 * project2.c
 *
 *  Created on: Sep 2, 2016
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
#include<rtai.h>
#include<rtai_lxrt.h>

MODULE_LICENSE("GPL");

int init_module(void)
{
	unsigned long *PBDR,*PBDDR;
	unsigned long *ptr;
	ptr=(unsigned long*)__ioremap(0x80840000,4096,0);
	PBDR=ptr+1;
	PBDDR=ptr+5;
	*PBDDR |=0xE0;
	*PBDR |=0x60;
	printk("MODULE INSTALLED\n");
	return 0;
}

void cleanup_module(void)
{
	unsigned long *PBDR,*PBDDR;
	unsigned long *ptr;

	ptr=(unsigned long*)__ioremap(0x80840000,4096,0);
		PBDR=ptr+1;
		PBDDR=ptr+5;
		*PBDDR |=0xE0;
		*PBDR &=0x00;
		printk("MODULE REMOVED\n");
}






