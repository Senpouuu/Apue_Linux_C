#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUM 20
#define FNAME "./out"

static char buff[1024];

static pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;

void * threadFunc(void* var)
{
	FILE* fp = NULL;
	
	pthread_mutex_lock(&mux);
	fp = fopen(FNAME,"r+");
	if(fp == NULL)
	{
		perror("fopen():");
		pthread_exit(NULL);
	}
	
	fgets(buff,1024,fp);
	fseek(fp,0,SEEK_SET);
	fprintf(fp,"%d\n",(atoi(buff))+1);
	fclose(fp);
	pthread_mutex_unlock(&mux);

	pthread_exit(NULL);
}


int main(int argc,char**argv)
{
	pthread_t thread_buff[THREAD_NUM];
	for(int i = 0;i < THREAD_NUM;i++)
	{
		int err = pthread_create(&thread_buff[i],NULL,threadFunc,NULL);
		if(err)
			exit(1);
	}
	
	for(int i = 0;i < THREAD_NUM;i++)
	{
		pthread_join(thread_buff[i],NULL);
	}

	pthread_mutex_destroy(&mux);
	exit(0);
}

