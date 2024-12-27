#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define LEFT 30000000
#define RIGHT 30000200
#include <sys/wait.h>

int main(int argc,char**argv)
{
	int mark;
	pid_t pid;
	
	for(int i = LEFT;i<=RIGHT;i++)
	{
		mark = 1;
		pid = fork();
		
		if(pid < 0)
		{
			perror("fork()");
			exit(1);
		}
		if(pid > 0)
			continue;
		if(pid == 0)
		{
			for(int j = 2; j<i/2; j++)
			{
				if(i%j == 0)
				{
					mark = 0;
					break;
				}
			
			}
			if(mark)
				printf("%d is a primer\n",i);
			exit(1);
		}	
	}		
	for(int i = LEFT;i<RIGHT;i++)
	{
		wait(NULL);
	}
	exit(0);
}

