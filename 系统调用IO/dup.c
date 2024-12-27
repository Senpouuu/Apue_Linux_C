#include <stdio.h>
#include <stdlib.h>
#define FNAME "./out"
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char**argv)
{	
	
	int fd = open(FNAME,O_WRONLY|O_CREAT|O_TRUNC,0600);
	
	
	dup2(fd,1);	
	puts("hello!");
	close(fd);
	
	exit(0);
}

