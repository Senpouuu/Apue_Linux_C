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
	for(int n = 1;n <= 3;n++)
	{
		pid = fork();
		if(pid == 0)
		{
			for(int i = LEFT+n;i<=RIGHT;i+=3)
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


