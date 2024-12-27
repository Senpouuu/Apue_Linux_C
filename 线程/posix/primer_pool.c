#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define LEFT 30000000
#define RIGHT 30000200
#define THREAD_NUM 5

static int task_num = 0;
static pthread_mutex_t num_mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_func(void* ptr)
{
	while(1)
	{
		int mark = 1;

		pthread_mutex_lock(&num_mutex);
		while(task_num == 0)
		{
			pthread_mutex_unlock(&num_mutex);
			sched_yield();
			pthread_mutex_lock(&num_mutex);
		}
		
		if(task_num == -1)
		{
			pthread_mutex_unlock(&num_mutex);
			break;
		}
		

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
			printf("%d is a primer\n",task_num);
		}
		task_num = 0;
		pthread_mutex_unlock(&num_mutex);
		
	}
	pthread_exit(NULL);
}


int main(int argc,char**argv)
{
	pthread_t tid[THREAD_NUM];

	for(int i = 0;i<THREAD_NUM;i++)
		pthread_create(&tid[i],NULL,thread_func,NULL);

	for(int i = LEFT;i <= RIGHT;i++)
	{
		pthread_mutex_lock(&num_mutex);
		while(task_num != 0)
		{
			pthread_mutex_unlock(&num_mutex);
			sched_yield();
			pthread_mutex_lock(&num_mutex);
		}
		task_num = i;
		pthread_mutex_unlock(&num_mutex);
	}

	pthread_mutex_lock(&num_mutex);
	while(task_num != 0)
	{
		pthread_mutex_unlock(&num_mutex);
		sched_yield();
		pthread_mutex_lock(&num_mutex);
	}
	task_num = -1;	
	pthread_mutex_unlock(&num_mutex);


	for(int i = 0;i<THREAD_NUM;i++)
		pthread_join(tid[i],NULL);
	

	exit(0);
}



