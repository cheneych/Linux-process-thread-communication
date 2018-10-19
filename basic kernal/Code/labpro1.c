#include<stdio.h>
#include<sys/mman.h>
#include<pthread.h>
#include<stdlib.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
	int fp,n;
	unsigned long *PFDR,*PFDDR;
	unsigned long *PBDR,*PBDDR;   //create pointer to port B/F
	unsigned long *ptr;   //create pointer to __mmap
	unsigned long state;     //create button's state
	int button;

	fp=open("/dev/mem",O_RDWR);
	if (fp==-1){
		printf("\n error\n");
		return(-1);  //failed open
	}
	ptr=(unsigned long*)
			mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fp,0x80840000);
	if (ptr==MAP_FAILED){
		printf("\n Unable to map memory space \n");
		return(-2);
	}
	PBDR=ptr+1;
	PBDDR=ptr+5;
	PFDR=ptr+12;
	PFDDR=ptr+13;  //according to their physical address
	*PBDDR |=0xE0;
	*PFDDR |=0x02; //give input/output
    state=0XFF;
          printf("scan an integer from 0 to 4:");
          scanf("%d",&n);


      //For 5 buttons;press corresponding button to skip out of while loop and play sound
          switch (n)
         {
         case 0:while (state!=0x1E){
        printf("please wait\n");
		state=*PBDR;
	    usleep(100000);
	}
        break;

         case 1:while (state!=0x1D){
        	 printf("please wait\n");
		state=*PBDR;
		 usleep(100000);
	}
         break;

          case 2:while (state!=0x1B){
        	  printf("please wait\n");
		state=*PBDR;
		 usleep(100000);
	}
        break;

          case 3:while (state!=0x17){
        	  printf("please wait\n");
		state=*PBDR;
		 usleep(100000);
          }
        break;

           case 4:while (state!=0x0F){
        	   printf("please wait\n");
		state=*PBDR;
		 usleep(100000);
	}
          break;
}
	while (1){
		*PFDR=0xFF;
		msync(NULL,4096,MS_SYNC);
		usleep(100000);
		*PFDR=0xFD;
		msync(NULL,4096,MS_SYNC);
		usleep(100000);
	}  //create square wave
	close(fp);
	return 0;
}
