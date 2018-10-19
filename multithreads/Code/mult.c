/*
 * mult.c
 *
 *  Created on: Sep 21, 2016
 *      Author: ccv5f
 */



#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

int a[50][50],b[50][50],c[50][50],p[1000];  //matrix a,b,c
int n1,m1,n2,m2,num;  //row,column
struct timeval start1,end1;  //calculate time

// one thread for each row
void *mult(void *ptr)
{
  int rnum=*(int*)ptr;
  int j,k;

  //matrix multiplication
for (j=0;j<m2;j++)

  for (k=0;k<m1;k++)
    c[rnum][j]+=a[rnum][k]*b[k][j];
  pthread_exit(0);
}

// one thread for each element
void *mult3(void *ptr)
{
  int rnum=*(int*)ptr;
  int i,j,k;

  i=rnum/m2;

  j=rnum%m2;
  //matrix multiplication
  for (k=0;k<m1;k++)
    c[i][j]+=a[i][k]*b[k][j];
  pthread_exit(0);
}

// one thread for entire matrix
void *mult2(void *ptr)
{

  int i,j,k;
  //matrix multiplication
  for (i=0;i<n1;i++)
	  for (j=0;j<m2;j++)
		  for (k=0;k<m1;k++)
    c[i][j]+=a[i][k]*b[k][j];
  pthread_exit(0);
}

int main()
{
  pthread_t thread[100],thrd;   // define 100 threads
  int i,j;
  FILE *fp;

  //read from the file
  if ((fp=fopen("/home/ccv5f/workspace/lab2b/Release/multiplication.txt","r"))<0)
  {
    printf("open this file is error!\n");
    exit(0);
  }

  fscanf(fp,"%d%d",&n1,&m1);
  for (i=0;i<n1;i++)
    for (j=0;j<m1;j++)
      fscanf(fp,"%d",&a[i][j]);   //input the first matrix

  fscanf(fp,"%d%d",&n2,&m2);
    for (i=0;i<n2;i++)
    for (j=0;j<m2;j++)
      fscanf(fp,"%d",&b[i][j]);   //input the second matrix

    fclose(fp);

    while (1)
    {
        printf("Choose number of threads:\n");
        printf("Input 1 : one thread for each row of matrix\n");
        printf("Input 2 : one thread to compute entire matrix\n");
        printf("Input 3 : one thread for each element of matrix\n");
        scanf("%d",&num);

        switch (num)
        {
        case 1:
          {
        	  gettimeofday(&start1,NULL);

        	   for (i=0;i<n1;i++)
        	   {
        	   p[i]=i;   //save i,otherwise there will be a conflict
        	   pthread_create(&thread[i],NULL,(void*)&mult, (void *)&p[i]);//create n threads
        	   }


        	   for (i=0;i<n1;i++)
        	   pthread_join(thread[i],NULL);

        	   gettimeofday(&end1,NULL);
        	   long time1=(end1.tv_sec-start1.tv_sec)*1000000+((end1.tv_usec-start1.tv_usec));
        	   printf("time of calculation by %d thread is:%ld microsecond.\n",n1,time1);


        	   for (i=0;i<n1;i++)
        	   {
        	       for (j=0;j<m1;j++)
        	     printf("%d ",a[i][j]);
        	     printf("\n");
        	   }

        	   for (i=0;i<n2;i++)
        	    {
        	        for (j=0;j<m2;j++)
        	      printf("%d ",b[i][j]);
        	      printf("\n");
        	    }


        	   for (i=0;i<n1;i++)
        	 {
        	     for (j=0;j<m2;j++)
        	   printf("%d ",c[i][j]);
        	   printf("\n");
        	 }

          break;
          }

        case  2:
         {

        	    gettimeofday(&start1,NULL);

        	  pthread_create(&thrd,NULL,(void*)&mult2,NULL); //create a thread



        	  pthread_join(thrd,NULL);  //wait for the end of a thread

        	  gettimeofday(&end1,NULL);
        	    long time1=(end1.tv_sec-start1.tv_sec)*1000000+((end1.tv_usec-start1.tv_usec));
        	    printf("time of calculation by 1 thread is:%ld microsecond.\n",time1);


        	  for (i=0;i<n1;i++)
        	  {
        	      for (j=0;j<m1;j++)
        	    printf("%d ",a[i][j]);
        	    printf("\n");
        	  }

        	  for (i=0;i<n2;i++)
        	   {
        	       for (j=0;j<m2;j++)
        	     printf("%d ",b[i][j]);
        	     printf("\n");
        	   }


        	  for (i=0;i<n1;i++)
        	{
        	    for (j=0;j<m2;j++)
        	  printf("%d ",c[i][j]);
        	  printf("\n");
        	}
        	  break;
         }




       case 3:
         {

        	 gettimeofday(&start1,NULL);
        	   for (i=0;i<n1*m2;i++)
        	   {
        	   p[i]=i; //save i,otherwise there will be a conflict
        	   pthread_create(&thread[i],NULL,(void*)&mult3, (void *)&p[i]);//create n1*m2 threads
        	   }


        	   for (i=0;i<n1*m2;i++)
        	   pthread_join(thread[i],NULL); //wait for the end of n1*m2 threads

        	   gettimeofday(&end1,NULL);
        	       long time1=(end1.tv_sec-start1.tv_sec)*1000000+((end1.tv_usec-start1.tv_usec));
        	       printf("time of calculation by %d thread is:%ld microsecond.\n",n1*m2,time1);

        	   for (i=0;i<n1;i++)
        	   {
        	       for (j=0;j<m1;j++)
        	     printf("%d ",a[i][j]);
        	     printf("\n");
        	   }

        	   for (i=0;i<n2;i++)
        	    {
        	        for (j=0;j<m2;j++)
        	      printf("%d ",b[i][j]);
        	      printf("\n");
        	    }


        	   for (i=0;i<n1;i++)
        	 {
        	     for (j=0;j<m2;j++)
        	   printf("%d ",c[i][j]);
        	   printf("\n");
        	 }

         break;
         }

        default:break;

         }
        if (num<1 || num>3) break;
    }




  return 0;
}



