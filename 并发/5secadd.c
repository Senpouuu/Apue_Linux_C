#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

static long long int i = 0;

static volatile int flag = 1;

void alarmHandler(int sig)
{
	flag = 0;
}


int main(int argc,char**argv)
{
	signal(SIGALRM,alarmHandler);
	alarm(5);
	while(flag)
	{
		i=i+1;
	}	
	printf("%lld\n",i);
	exit(0);
}

