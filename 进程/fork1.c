#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char**argv)
{
	pid_t pid;
	printf("[%d]Begin!\n",getpid());	

	fflush(stdout);		
	pid = fork();
	
	if(pid < 0)			
	{
		perror("fork()");
		exit(1);
	}
	else if(pid == 0)		//parent
	{
		printf("[%d]Parent is running!\n",getpid());
	}
	else
	{
		printf("[%d]Child is running!\n",getpid());
	}
	

	printf("[%d]End!\n",getpid());	


	exit(0);
}

