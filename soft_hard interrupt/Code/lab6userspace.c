/*
 * lab6u.c
 *
 *  Created on: Nov 16, 2016
 *      Author: ccv5f
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include<sys/mman.h>
#include<fcntl.h>


#define MSG_SIZE 40
int fd_fifo2;
pthread_t thrd;
unsigned long *VIC2SINT;
unsigned long *ptr;   //create pointer to __mmap
int fp;
int sign=0;
int sock,length,n;
int flag=0;  //whether there has been a master already,and the master is not my server
int f;
int i;   //using for loop
int total=0,t=0;   //counter
int randonumber;
int master=0; //if I am the master
int boolval=1;
socklen_t fromlen;
char s1[50],s2[50],s[50];
struct sockaddr_in server;
struct sockaddr_in from;
char buf[MSG_SIZE];
char s[50];

void *extra(void *p)   //thread
{
	char cha[40];
	while (1)
	{
		if((read(fd_fifo2, &cha, sizeof(cha)))<0) //read from pipe 2
		{
			printf("Named pipe UtoP read error\n");
			exit(-1);
		}

		//send to slaves
		if (master==1)
		{
			//broadcast
			from.sin_addr.s_addr=inet_addr("10.3.52.255");
			sendto(sock,cha,sizeof(cha),0,(struct sockaddr*)&from,fromlen);
		}
	}
	pthread_exit(NULL);
}

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc,char *argv[])
{
	if (argc<2)   //port number
	{
		fprintf(stderr,"ERROR,no port provided\n");
		exit(0);
	}

	//create a socket
	sock=socket(AF_INET,SOCK_DGRAM,0);
	if (sock<0)
		error("Opening socket");
	//initialize
	length=sizeof(server);
	bzero(&server,length);
	//fill out the fields in a sockaddr_in structure
	server.sin_family=AF_INET;    //protocol
	server.sin_addr.s_addr=INADDR_ANY;   //ip address of this machine
	server.sin_port=htons(atoi(argv[1]));//atoi[argv[1]] port number  atoi: string to integer
	//bind the socket
	if (bind(sock,(struct sockaddr *)&server,length)<0)
		error("binding");
	//change socket permissions to allow broadcast
	if (setsockopt(sock,SOL_SOCKET,SO_BROADCAST,&boolval,
			sizeof(boolval))<0)
	{
		printf("error setting socket options\n");
		exit(-1);
	}
	//structure length
	fromlen=sizeof(struct sockaddr_in);
	//get the dynamic IP address
	char host[50],addr[50];
	gethostname(host,sizeof(host));
	struct hostent *hostaddr;
	struct in_addr **boardIPList;
	hostaddr=(struct hostent*)gethostbyname(host);
	boardIPList=(struct in_addr**)hostaddr->h_addr_list;
	strcpy(addr,inet_ntoa(*boardIPList[0]));
	//map the physical address
	fp=open("/dev/mem",O_RDWR);
	if (fp==-1){
		printf("\n error\n");
		return(-1);  //failed open
	}
	ptr=(unsigned long*)
								mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fp,0x800C0000);
	if (ptr==MAP_FAILED){
		printf("\n Unable to map memory space \n");
		return(-2);
	}
	//registers related to interrupt 63
	VIC2SINT=ptr+6;
	//initialize registers related to interrupt 63
	*VIC2SINT  &= 0x00000000;

	//create a fifo
	int fd_fifo1;
	if ((fd_fifo1=open("/dev/rtf/1",O_WRONLY))<0)
	{
		printf("pipe fd_fifo_in error\n");
		exit(-1);
	}
	if ((fd_fifo2=open("/dev/rtf/2",O_RDONLY))<0)
	{
		printf("pipe fd_fifo_out error\n");
		exit(-1);
	}
	pthread_create(&thrd,NULL,(void*)&extra,NULL);
	while (1)
	{
		//clear the string before receiving the message
		memset(buf,0,sizeof(buf));
		memset(s1,0,sizeof(s1));
		memset(s2,0,sizeof(s2));
		n=recvfrom(sock,buf,MSG_SIZE,0,(struct sockaddr *)&from,&fromlen);
		if (n<0)
			error("recvfrom");
		else
		{
			//WHOIS
			if (strncmp (buf,"WHOIS",5)==0)
			{
				if (master==1)
				{
					sprintf(buf,"Chen on board %s is the master",addr); //HOST NAME IP ADDRESS
					sendto(sock,buf,sizeof(buf),0,(struct sockaddr*)&from,fromlen);

				}

			}
			else
			{
				//VOTE
				if (strncmp(buf,"VOTE",4)==0)
				{
					randonumber=rand()%10+1; //generate the random number from 1 to 10
					sprintf(buf,"# %s %d",addr,randonumber);
					strcpy(s,buf);
					//broadcast
					from.sin_addr.s_addr=inet_addr("10.3.52.255");
					sendto(sock,buf,sizeof(buf),0,(struct sockaddr*)&from,fromlen);
					printf("%s\n",buf);
					flag=0; //clear the master before,restart
					sign=0;
				}
				else
				{
					//IP from other machines
					if ((strncmp(buf,"#",1)==0) && (flag==0)) //if I am not the master,there is no need to compare
					{
						for (i=0;i<MSG_SIZE;i++)
						{
							if ((buf[i])==32)
								total++;
							if (total==2) break;
						}
						total=0;
						//get the random number
						s1[0]=buf[i+1];
						s1[1]=buf[i+2];
						if (s1[1]>=58 || s1[1]<=47) s1[1]=47; //if they range from [0..9]

						for (i=0;i<MSG_SIZE;i++)
						{
							if (s[i]==32)
								t++;
							if (t==2) break;
						}
						t=0;
						//get the random number
						s2[0]=s[i+1];
						s2[1]=s[i+2];
						if (s2[1]>=58 || s2[1]<=47) s2[1]=47;  //if they range from [0..9]
						//if my random number is 10,the other's is not,I am the master temporarily
						if ((s2[0]==49) && (s2[1]==48) && !((s1[0]==49) && (s1[1]==48)))
							master=1;
						//if the other's random number is 10,mine is not,I will never be the master
						else if ((s1[0]==49) && (s1[1]==48) && !((s2[0]==49) && (s2[1]==48)))
						{
							master=0;
							flag=1;
						}
						//comparing random number first
						else
							//my random number is bigger
							if (strncmp(s2,s1,2)>0)
							{
								master=1;
								//printf("MASTER1=%d\n",master);
							}
						//my random number is smaller
							else if (strncmp(s2,s1,2)<0)
							{
								master=0;
								flag=1;
							}
						//there is a tie in random number,comparing IP number
							else
							{
								if (strncmp(s,buf,MSG_SIZE)<0)
								{
									master=0;
									flag=1;
								}
								else
								{
									master=1;
								}
							}
					}
					else
					{
						//if ((strncmp(buf,"@",1)==0) && (sign==0) && (f==0))
						strcpy(s,inet_ntoa(from.sin_addr));
						//printf("HERE %s\n",s);
						 //printf("1 %s\n",addr);
			if ((strncmp(buf,"@",1)==0) && ((strcmp(addr,s)!=0)))
						{
							//send to fifo
			//	printf("buf %c\n",buf[1]);
							if (write(fd_fifo1,&buf,sizeof(buf))!=sizeof(buf))
							{
								printf("write error!\n");
								exit(-1);
							}
							//printf("1\n");
							//send software interrupt signal
							*VIC2SINT |=0X80000000;
							if (master==1)
							{
								//printf("master\n");
								//broadcast
								from.sin_addr.s_addr=inet_addr("10.3.52.255");

								sendto(sock,buf,sizeof(buf),0,(struct sockaddr*)&from,fromlen);

							}
						}
					}
				}
			}
		}
	}
}

