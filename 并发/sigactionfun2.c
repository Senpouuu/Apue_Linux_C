#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#define BUFFSIZE 128 
#define BURST 100


static int flag = 0;

void alarmHandler(int sig)
{
	alarm(1);
	flag++;
	if(flag > BURST)
		flag = BURST;
}

void alarmHandlerPlus(int sig,siginfo_t* siginfo,void* ptr)
{
	if(siginfo->si_code != SI_KERNEL)
		return;
	alarm(1);
	flag++;
	if(flag > BURST)
		flag = BURST;
}


int main(int argc,char**argv)
{
	int sfd;	
	char* buff[BUFFSIZE];
	int len;	
	int ret;	
	int pos;	

	if(argc < 2)
	{
		fprintf(stderr,"Usage:\r\n");
		exit(1);
	}	

	sfd = open(argv[1],O_RDONLY);

	if(sfd < 0)
	{
		perror("open1()");
		exit(1);
	}
	
	struct sigaction sigact;
	sigact.sa_handler = NULL;
	sigact.sa_sigaction = alarmHandlerPlus;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_SIGINFO;
	sigact.sa_restorer = NULL;


	sigaction(SIGALRM,&sigact,NULL);

//	signal(SIGALRM,alarmHandler);
	alarm(1);
	flag = 0;
	while(1)
	{
		len = read(sfd,buff,BUFFSIZE);
		if(len < 0)
		{
			perror("read()");
			break;	
		}
		if(len == 0)
			break;

		pos = 0;
		while(flag <= 0)
			pause();
		flag--;
		while(len > 0)
		{
			ret = write(1,buff + pos,len);
			if(ret < 0)
			{
				perror("write()");
				exit(1);
			}
			pos += ret;
			len -= ret;
		}
		flag = 0;
	}
	close(sfd);

	exit(0);
}
