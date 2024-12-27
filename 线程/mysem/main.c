#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "mysem.h"

#define LEFT 30000000
#define RIGHT 30000200
#define N 5
#define THREAD RIGHT-LEFT+1


static mysem_t *sem;

void* primeFunc(void* arg)
{
	sleep(3);
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
		mysem_add(sem,1);
		pthread_exit(NULL); // ·µ»Ø±ê¼Ç
	}
}


int main(int argc, char** argv)
{
	pthread_t tid[THREAD];
	int* p = NULL;
	for(int i = LEFT; i <= RIGHT; i++)
	{
		sem = mysem_init(N);
		p = malloc(sizeof(int));
		if(p == NULL)
			exit(1);
		*p = i;
		
		if (i < 2)
			continue;
		mysem_sub(sem,1);
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
