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
	int begin = LEFT;
	for(int i = 0;i<3;i++)
	{
		begin += 67;
		pid = fork();
		if(pid == 0)
		{
			for(int i = begin-67;i<=begin;i++)
			{
				mark = 1;
				for(int j = 2; j<i/2; j++)
				{
					if(i%j == 0)
					{
						mark = 0;
						break;
					}
				}
				if(mark)
					printf("[%d]:%d is a primer\n",getpid(),i);
			}
			exit(1);
		}
	}
	
	for(int i = 0;i<3;i++)
		wait(NULL);
	
	exit(0);
}		


