/*
 * lab2part2.c
 *
 *  Created on: Sep 20, 2016
 *      Author: ccv5f
 */


#include <rtai.h>
#include <rtai_lxrt.h>
#include <stdio.h>
#include<pthread.h>
#include <string.h>
#include <stdbool.h>

pthread_t thread1,thread2,thread3;   // define 3 threads for reading information and taking information
char c[30][1000];   //final string
char a[1000];  //buffer
int sum=0;     //total lines
FILE *fp1,*fp2;
// signs to stop taking strings from buffer
bool b2;
bool b1;

RTIME BaseP;


void *r1(void *ptr)
{

    // read from a file
	if ((fp1=fopen("/home/ccv5f/workspace/lab2part2/Release/first.txt","r"))<0)
		{
			printf("open this file is error!\n");
			exit(0);
		}

  //Initialization
	RT_TASK* rttask1=rt_task_init(nam2num("thr1"),0,512,256);
	//Schedule the task
	rt_task_make_periodic(rttask1,rt_get_time()+10*BaseP,20*BaseP);

	b1=0;
	while (!feof(fp1))    // until the end of a file
	{

		fgets(a,sizeof(a),fp1); // read a line
		rt_task_wait_period();  //wait for some time
	}
	 fclose(fp1);
	b1=1;     //finish reading

	pthread_exit(NULL);
}

void *r2(void *ptr)
{

  //read from a file
	if ((fp2=fopen("/home/ccv5f/workspace/lab2part2/Release/second.txt","r"))<0)
		{
			printf("open this file is error!\n");
			exit(0);
		}
	//Initialization
	RT_TASK* rttask2=rt_task_init(nam2num("thr2"),0,512,256);
	//Schedule the task
	rt_task_make_periodic(rttask2,rt_get_time()+20*BaseP,20*BaseP);

 b2=0;

	while (!feof(fp2))    // until the end of a file
	{
		fgets(a,sizeof(a),fp2);  //read a line
		rt_task_wait_period();    //wait for some time
	}
	 fclose(fp2);
	b2=1;       //finish reading

	pthread_exit(NULL);
}

void *c1(void *ptr)
{
	int i;
	//Initialization
	RT_TASK* rttask3=rt_task_init(nam2num("thr3"),0,512,256);
	//Schedule the task
	rt_task_make_periodic(rttask3,rt_get_time()+15*BaseP,10*BaseP);


	while ((!b1)|| (!b2))  //the condition of reading from a buffer
	{
		sum++;
		strcpy(c[sum],a);  //read from the buffer
		rt_task_wait_period(); //wait for some time
	}

pthread_exit(NULL);
}


int main()
{
	int i;


	BaseP=start_rt_timer(nano2count(1000000));
	// create three threads
	pthread_create(&thread1,NULL,(void*)&r1, NULL);
	pthread_create(&thread2,NULL,(void*)&r2, NULL);
	pthread_create(&thread3,NULL,(void*)&c1, NULL);
    //wait for threads to end
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	pthread_join(thread3,NULL);

	// print out the combined string
	for (i=1;i<=sum;i++)
		puts(c[i]);

	return 0;
}
