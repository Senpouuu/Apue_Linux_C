#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define LEFT 30000000
#define RIGHT 30000200
#define THREAD_NUM 5

static int task_num = 0;
static int time2Kill = 0;
static pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t taskdone_mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_func(void* ptr)
{
	while(1)
	{
		int mark = 1;
		pthread_mutex_lock(&task_mutex);
		for(int j = 2; j < task_num/2; j++)
		{
			if(task_num % j == 0)
			{
				mark = 0;
				break;
			}
		}	
		if(mark)
		{
			printf("Thread %lu: %d is a prime\n", pthread_self(), task_num);
		}
		if(time2Kill)
			break;
		pthread_mutex_unlock(&taskdone_mutex);
	}
	pthread_exit(NULL);
}


int main(int argc,char**argv)
{
	int mark;
	pthread_t tid[THREAD_NUM];

	for(int i = 0;i<THREAD_NUM;i++)
		pthread_create(&tid[i],NULL,thread_func,(void*)tid[i]);

	for(int i = LEFT;i<=RIGHT;i++)
	{
		pthread_mutex_lock(&taskdone_mutex);
		task_num = i;
		pthread_mutex_unlock(&task_mutex);
	}
	
	time2Kill = 1;	

	for(int i = 0;i<THREAD_NUM;i++)
		pthread_join(tid[i],NULL);

	exit(0);
}



