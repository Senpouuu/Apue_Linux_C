#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void sigFunc(int sig)
{
	puts("Unlocking...");	
	pthread_mutex_unlock(&mutex);
}

void* thrFunc(void* ptr) 
{
	while(1)
	{
		pthread_mutex_lock(&mutex);
		puts("I am doing........");
	}
	return NULL;
}


int main(int argc,char**argv)
{
	pthread_t tid;
	signal(SIGALRM,sigFunc);
	alarm(3);
	pthread_mutex_lock(&mutex);
	pthread_create(&tid,NULL,thrFunc,NULL);
	while(1);	

	exit(0);
}

