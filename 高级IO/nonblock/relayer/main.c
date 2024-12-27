#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "relayer.h"	

#define TTY1 "/dev/tty3"
#define TTY2 "/dev/tty4"
#define TTY3 "/dev/tty5"
#define TTY4 "/dev/tty6"


int main(int argc,char**argv)
{
	int fd1,fd2;
	int fd3,fd4;
	int job1,job2;

	fd1 = open(TTY1,O_RDWR);
	if(fd1 < 0)
 	{
		perror("open(TTY1)");
		exit(1);
	}

	fd2 = open(TTY2,O_RDWR|O_NONBLOCK);
	if(fd2 < 0)
	{
		perror("open(TTY2)");
		exit(1);
	}
	
	job1 = rel_addjob(fd1,fd2);
	
	if(job1 < 0)
	{
		fprintf(stderr,"rel_addjob():%s",strerror(-job1));
		exit(1);
	}

	fd3 = open(TTY3,O_RDWR);
	if(fd3 < 0)
 	{
		perror("open(TTY3)");
		exit(1);
	}

	fd4 = open(TTY4,O_RDWR|O_NONBLOCK);
	if(fd4 < 0)
	{
		perror("open(TTY4)");
		exit(1);
	}
	
	job2 = rel_addjob(fd3,fd4);
	
	if(job2 < 0)
	{
		fprintf(stderr,"rel_addjob():%s",strerror(-job2));
		exit(1);
	}

	while (1)
	{
		pause();
	}
	


	close(fd4);
	close(fd3);
	close(fd2);
	close(fd1);	

	exit(0);
}

