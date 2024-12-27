#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
static void* threadFunc(void* arg)
{
	puts("Thread is Working!!");	
	pthread_exit(NULL);

}


int main(int argc,char**argv)
{
	pthread_t tid; 

	puts("Begin!");
	
	int err = pthread_create(&tid,NULL,threadFunc,NULL);
	if(err)
	{
		fprintf(stderr,"pthread_create():%s",strerror(err));
		exit(1);
	}

	puts("End!");	
	pthread_join(tid,NULL);	

	exit(0);  

}

