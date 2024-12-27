#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define PROC_NUM 20
#define FNAME "./out"
#define F_ULOCK 0	/* Unlock a previously locked region.  */
#define F_LOCK  1	/* Lock a region for exclusive use.  */
#define F_TLOCK 2	/* Test and lock a region for exclusive use.  */
#define F_TEST  3	/* Test a region for other processes locks.  */


static char buff[1024];


void * threadFunc(void* var)
{
	FILE* fp = NULL;
	
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

	pthread_exit(NULL);
}


int main(int argc,char**argv)
{
	for(int i = 0;i < PROC_NUM;i++)
	{
		int pid = fork();
		if(pid == 0)	// child process
		{
			int fd = open(FNAME,O_RDWR,0666);
			if(fd < 0)
			{
				perror("open():");
				exit(1);
			}
	
			lockf(fd,F_LOCK,0);

			off_t len = lseek(fd,0,SEEK_END);
			lseek(fd,0,SEEK_SET);	
			read(fd,buff,len);
			
			int num = atoi(buff);
			num++;
			sleep(1);
			lseek(fd,0,SEEK_SET);	
			dprintf(fd,"%d\n",num);			
			lockf(fd,F_ULOCK,0);
			
			close(fd);
			exit(0);

		}
		if(pid > 0)	// parent process
		{
			for(int i = 0;i < PROC_NUM;i++)
				wait(NULL);
		}
	}

	

	exit(0);
}

