#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

static void threadExitFunc(void* arg)
{
	puts((char*)(arg));
}

static void* threadFunc(void* arg)
{
	puts("Thread is Working!!");	
	pthread_cleanup_push(threadExitFunc,"Thread Killed!1");
	pthread_cleanup_push(threadExitFunc,"Thread Killed!2");
	pthread_cleanup_push(threadExitFunc,"Thread Killed!3");
	pthread_exit(NULL);
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
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

