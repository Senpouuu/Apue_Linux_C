#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define LEFT 30000000
#define RIGHT 30000200
#define THREAD RIGHT-LEFT+1


void* primeFunc(void* arg)
{
	int mark = 1;
	int i = *(int*)arg;
	for(int j = 2; j <= i / 2; j++)
	{
		if(i % j == 0)
		{
			mark = 0;
		}
	}
	if(mark)
	{
		printf("%d is a prime\n",i);
		free(arg);
		pthread_exit(NULL); // 返回标记
	}
}

int main(int argc, char** argv)
{
	pthread_t tid[THREAD];
	int* p = NULL;
	for(int i = LEFT; i <= RIGHT; i++)
	{
		p = malloc(sizeof(int));
		if(p == NULL)
			exit(1);
		*p = i;
		
		if (i < 2)
			continue;
		int err = pthread_create(&tid[i-LEFT],NULL,primeFunc,p);

		if(err < 0)
		{
			fprintf(stderr,"pthread_create:%s\n",strerror(err));
			exit(1);
		}

	}        
	
	for(int i = LEFT; i <= RIGHT; i++)
		pthread_join(tid[i-LEFT],NULL);
	
	return 0;
}
