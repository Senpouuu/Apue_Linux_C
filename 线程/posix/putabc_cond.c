#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define THREAD_NUM 5
#define FNAME "./out"

static int num = 0;
static char buff[1024];
static pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void * threadFunc(void* var)
{
	int n = (int)var;
	int c = 'a' + (int)var;
	while(1)
	{
		while(num != n)
		{
			pthread_cond_wait(&cond,&mux);
		}
		num = num + 1;
		if(num == 4)
			num = 0;		
		write(1,&c,1);
		pthread_cond_broadcast(&cond);
	}
	pthread_exit(NULL);
}


int main(int argc,char**argv)
{
	pthread_t thread_buff[THREAD_NUM];
	for(int i = 0;i < THREAD_NUM;i++)
	{
		int err = pthread_create(&thread_buff[i],NULL,threadFunc,(void*)i);
		if(err)
			exit(1);
	}
	alarm(5);
	
	for (int i = 0; i < THREAD_NUM; i++) {
        pthread_join(thread_buff[i], NULL); // 等待所有线程结束
    }
	pthread_mutex_destroy(&mux);
	pthread_cond_destroy(&cond);
	exit(0);
}

