/*
 * addition.c
 *
 *  Created on: Sep 21, 2016
 *      Author: ccv5f
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include<stdlib.h>
#include <unistd.h>

int a[50][50],b[50][50],c[50][50],p[1000]; // matrix a,b,c
int n,m,num;              // row and column
struct timeval start1,end1;  //calculate time

// one thread for each element
void *add3(void *ptr)
{
    int rnum=*((int *)ptr);
    int i,j;
    i=rnum/m;
    j=rnum%m;
    //matrix addition
    c[i][j]=a[i][j]+b[i][j];

  pthread_exit(NULL);
}

//one thread for entire output
void *add2(void *ptr)
{
	int i,j;
	 //matrix addition
	for (i=0;i<n;i++){
		for (j=0;j<m;j++){
			c[i][j]=a[i][j]+b[i][j];
		}
	}

	pthread_exit(0);
}

//one thread for each row
void *add(void *ptr)
{
  int rnum=*(int*)ptr;
  int j;
  //matrix addition
  for (j=0;j<n;j++)
    c[rnum][j]=a[rnum][j]+b[rnum][j];

  pthread_exit(NULL);
}

int main()
{
  pthread_t thread[1000];  // define 1000 threads
  pthread_t thrd;
  int i,j;
  FILE *fp;

  //read from the file
  if ((fp=fopen("/home/ccv5f/workspace/lab2a/Release/addition.txt","r"))<0)
  {
    printf("open this file is error!\n");
    exit(0);
  }

  fscanf(fp,"%d%d",&n,&m);
  for (i=0;i<n;i++)
    for (j=0;j<n;j++)
      fscanf(fp,"%d",&a[i][j]);


  fscanf(fp,"%d%d",&n,&m);
    for (i=0;i<n;i++)
    for (j=0;j<m;j++)
      fscanf(fp,"%d",&b[i][j]);

    fclose(fp);
while (1)
{
    printf("Choose number of threads:\n");
    printf("Input 1 : one thread for each row of matrix\n");
    printf("Input 2 : one thread to compute entire matrix\n");
    printf("Input 3 : one thread for each element of matrix\n");
    scanf("%d",&num);
    gettimeofday(&start1,NULL);
    switch (num)
    {
    case 1:
      {
    	  for (i=0;i<n;i++)
      {
    	   p[i]=i;  //save i,otherwise there will be a conflict
    	  pthread_create(&thread[i],NULL,(void*)&add, (void *)&p[i]); //create n threads
      }
      for (i=0;i<n;i++)
        pthread_join(thread[i],NULL);   //wait for the end of n threads
      gettimeofday(&end1,NULL);
            	   long time1=(end1.tv_sec-start1.tv_sec)*1000000+((end1.tv_usec-start1.tv_usec));
            	   printf("time of calculation by %d thread is:%ld microsecond.\n",n,time1);
            	   break;
      }

    case  2:
     {
    	 pthread_create(&thrd,NULL,(void*)&add2,NULL); //create one thread
    	 pthread_join(thrd,NULL);  //wait for the end of a thread
    	 gettimeofday(&end1,NULL);
    	            	   long time1=(end1.tv_sec-start1.tv_sec)*1000000+((end1.tv_usec-start1.tv_usec));
    	            	   printf("time of calculation by %d thread is:%ld microsecond.\n",1,time1);
     break;
     }

    case 3:
     {
    	 printf("fgfgfg");
    	 for (i=0;i<n*m;i++)
    	  {
    		  p[i]=i;
    		  pthread_create(&thread[i],NULL,(void*)&add3, (void *)&p[i]); //create n*m threads

    	  }
     	  for (i=0;i<n*m;i++)
    			  pthread_join(thread[i],NULL);  //wait for the end of n*m threads
     	 gettimeofday(&end1,NULL);
     	            	   long time1=(end1.tv_sec-start1.tv_sec)*1000000+((end1.tv_usec-start1.tv_usec));
     	            	   printf("time of calculation by %d thread is:%ld microsecond.\n",n*m,time1);
     break;
     }


      default:break;
     }




if (num>=1 && num<=3)

  for (i=0;i<n;i++)
{
    for (j=0;j<m;j++)
  printf("%d ",c[i][j]);
  printf("\n");
}
else break;

}

  return 0;
}

