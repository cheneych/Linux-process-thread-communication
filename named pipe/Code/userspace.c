/*
 * lab4.c
 *
 *  Created on: Oct 5, 2016
 *      Author: ccv5f
 */

#include <stdio.h>
#include "serial_ece4220.h"
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <fcntl.h>


pthread_t thrd,thrd2;
sem_t sem;
int fd_fifo2;
struct BUFFER
{
	unsigned long t;  //time
	unsigned int x;  //data from serial port
}buf;

struct SEND
{
	unsigned int x0;
	unsigned long t0;
	unsigned long te;
	int num;
}send[4];




//thread for print
void *print(void *ptr)
{
	
	struct BUFFER_PRINT_RECEIVE
	{
		unsigned int x0_p;
		unsigned int x1_p;
		float x_event_p;
		unsigned long t0_p;
		unsigned long t1_p;
		unsigned long t_event_p;
		int num;
	}receive;


	int print_pipe_in;

	if((print_pipe_in = open("/dev/rtf/2",O_RDONLY))<0)  //open a named pipe 1 to read kernel
	{
		printf("Named pipe UtoP for print read open error\n");
		exit(-1);
	}
	while(1)
	{
		if((read(print_pipe_in, &receive, sizeof(receive)))<0) //read from pipe 2 bufferprint
		{
			printf("Named pipe UtoP read error\n");
			exit(-1);
		}
		printf("thrd number%d\n",receive.num);
        printf("before data %lu  %u\n",receive.t0_p,receive.x0_p);
        printf("button data %lu  %.2f\n",receive.t_event_p,receive.x_event_p);
        printf("after data %lu  %u\n",receive.t1_p,receive.x1_p);
        printf("\n");


		sem_post(&sem);
		//printf("release sema success\n");
		//sleep(1);

	}
	//pthread_exit(NULL);


}

void *grandchild(void *ptr)
{
	struct B_PRINT
	{
		unsigned int x0;
		unsigned int x1;
		float xe;
		unsigned long t0;
		unsigned long t1;
		unsigned long te;
		int num;
	}bufprint;


	//sem_init(&semID,0,1);
	struct SEND *received_data = (struct SEND *)ptr;
	unsigned int gx0, gx1, temp;
	float button_x;
	unsigned long gt0, gt1, button_t;
	temp = received_data->x0;
    int p=0;;
	while(temp == buf.x)
	{


	}

	sem_wait(&sem);

	gx0 = received_data->x0;
	gt0 = received_data->t0;
	button_t = received_data->te;



	gx1 = buf.x;
	gt1 = buf.t;

	button_x = ((float)gx1-(float)gx0)*((float)button_t-(float)gt0)/((float)gt1-(float)gt0)+(float)gx0;
	//sem_wait(&semID);

	bufprint.x0 = gx0;
	bufprint.t0 = gt0;
	bufprint.xe = button_x;
	bufprint.te = button_t;
	bufprint.x1 = gx1;
	bufprint.t1 = gt1;
	bufprint.num=received_data->num;



	//send fifo+buffer
	if ((fd_fifo2=open("/dev/rtf/2",O_WRONLY))<0)
	{
		printf("pipe fd_fifo_out error\n");
		exit(-1);
	}

	if (write(fd_fifo2,&bufprint,sizeof(bufprint))!=sizeof(bufprint))
	{
		printf("write error!\n");
		exit(-1);
	}

	//printf("write grand success\n");
	pthread_exit(NULL);

}

void *first(void *ptr)   //first thread
{
	unsigned long timestamp;  //temporary variable
	int i=0;     //use for loop
	pthread_t arrthrd[4];

	//open a fifo
    int fd_fifo1;
	if ((fd_fifo1=open("/dev/rtf/1",O_RDONLY))<0)
	{
		printf("pipe fd_fifo_in error\n");
		exit(-1);
	}



	
	while (1)
	{

		//reads the time-stamp from the FIFO(kernel)
		if (read(fd_fifo1,&timestamp,sizeof(timestamp))<0)
		{
			printf("read error\n");
			//exit(-1);
		}

		//printf("read first success\n");

		send[i].x0=buf.x;
		send[i].t0=buf.t;
		send[i].te=timestamp;
		send[i].num=i;

		//collects the GPS data and time-stamps from buffer
		//printf("%ld   %ld  %ld\n",data.ts,data.l1,data.t1);

		//create 4 threads for buttons pressed

		pthread_create(&arrthrd[i],NULL,(void*)&grandchild,(void *)&send[i]);

		i++;
		if (i>3)
			i=0;


	}
	//pthread_exit(NULL);
}


int main()
{

	struct timeval currentt;   //acquire the current time
	int port_id=serial_open(0,5,5);
	unsigned long time1;   //time1
	char x;      //data from the serial port

	sem_init(&sem,0,1);  //initialize the semaphore

	pthread_create(&thrd,NULL,(void*)&first,NULL); //create the first thread
	pthread_create(&thrd2,NULL,(void*)&print,NULL);


int j=0;
	while (1)
	{
		// read from serial port

		serial_read(port_id,&x,sizeof(x));
     j++;

		//time1
		gettimeofday(&currentt,NULL);
		time1=(currentt.tv_sec)*1000000+(currentt.tv_usec);

		//update the buffer
		buf.x=x;
		buf.t=time1;

//		printf("main %lu  %u  %d\n",buf.t,buf.x,j);
		//	printf("%u\n",ftime.num);
		//printf("%ld   %ld\n",time1,ftime.num);
		fflush(stdout);


	}

	//wait for the end of threads
	pthread_join(thrd,NULL);
	//	for (i=1;i<=4;i++)
	//		pthread_join(arrthrd[i],NULL);
	//delete semaphore
	sem_destroy(&sem);
	//close serial port
	serial_close(port_id);


	return 0;
}




