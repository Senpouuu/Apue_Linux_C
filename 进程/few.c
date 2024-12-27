#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc,char**argv)
{
	puts("Begin");
	pid_t pid;
	fflush(NULL);
	pid = fork();

	if(pid == 0)
	{
		execl("/bin/date","date","+%s",NULL);
		perror("execl()");
		exit(1);
	}
	

	wait(NULL);
	puts("End");
	exit(0);
}

