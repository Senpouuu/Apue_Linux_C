#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void * func(void * ptr)
{
	while(1)
	{
		pause();
	}
	pthread_exit(NULL);
}



int main(int argc,char**argv)
{
	int i;
	pthread_t tid;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr,1024*1024);
	
	for(i = 0; ; i++)
	{
		int err = pthread_create(&tid,&attr,func,NULL);
	}
	

	printf("%d",i);	
	pthread_attr_destroy(&attr);
	exit(0);
}

