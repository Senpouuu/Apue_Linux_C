#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUM 20
#define FNAME "./out"

static char buff[1024];
static pthread_mutex_t mux[THREAD_NUM];


void * threadFunc(void* var)
{
	int c = 'a' + (int)var;
		
	while(1)
	{
		pthread_mutex_lock(&mux[(int)var]);
		write(1,&c,1);	
		if((int)var != THREAD_NUM - 1)
			pthread_mutex_unlock(&mux[(int)var + 1]);
		else
		{
			pthread_mutex_unlock(&mux[0]);
			puts("\n");
		}
	}	

	pthread_exit(NULL);
}


int main(int argc,char**argv)
{
	alarm(5);
	pthread_t thread_buff[THREAD_NUM];
	for(int i = 0;i < THREAD_NUM;i++)
	{
		pthread_mutex_init(&mux[i],NULL);
		int err = pthread_create(&thread_buff[i],NULL,threadFunc,(void*)i);
		if(err)
			exit(1);

		pthread_mutex_lock(&mux[i]);
	}

	pthread_mutex_unlock(&mux[0]);

	for(int i = 0;i < THREAD_NUM;i++)
	{
		pthread_join(thread_buff[i],NULL);
	}

	pthread_mutex_destroy(&mux);
	exit(0);
}

