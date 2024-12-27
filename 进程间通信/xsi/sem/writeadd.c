#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>


#define PROC_NUM 20
#define FNAME "./out"
#define F_ULOCK 0	/* Unlock a previously locked region.  */
#define F_LOCK  1	/* Lock a region for exclusive use.  */
#define F_TLOCK 2	/* Test and lock a region for exclusive use.  */
#define F_TEST  3	/* Test a region for other processes locks.  */


static char buff[1024];
static int sem_id;


static void P(void)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;  	// 资源量减1
	sem_b.sem_flg = 0;	
	
	while (semop(sem_id,&sem_b,1) < 0)
	{
		if(errno != EINTR || errno!= EAGAIN)
		{
			perror("P->semop():");
			exit(1);
		}
	}



}


static void V(void)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;  	// 资源量减1
	sem_b.sem_flg = 0;	
	
	while (semop(sem_id,&sem_b,1) < 0)
	{
		if(errno != EINTR || errno!= EAGAIN)
		{
			perror("V->semop():");
			exit(1);
		}
	}



}

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
	sem_id = semget(IPC_PRIVATE,1,0666|IPC_CREAT);
	if(sem_id < 0)
	{
		perror("semget():");
		exit(1);
	}

	// 初始化，设置信号量的值为1
	if(semctl(sem_id,0,SETVAL,1) < 0)
	{
		perror("semctl():");
		exit(1);
	}



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
	
			// lockf(fd,F_LOCK,0);

			P();
			
			off_t len = lseek(fd,0,SEEK_END);
			lseek(fd,0,SEEK_SET);	
			read(fd,buff,len);
			
			int num = atoi(buff);
			num++;
			// sleep(1);
			lseek(fd,0,SEEK_SET);	
			dprintf(fd,"%d\n",num);			
			// lockf(fd,F_ULOCK,0);
			
			V();

			close(fd);
			exit(0);

		}
		if(pid > 0)	// parent process
		{
			for(int i = 0;i < PROC_NUM;i++)
				wait(NULL);
		}
	}

	semctl(sem_id,0,IPC_RMID);
	exit(0);
}

