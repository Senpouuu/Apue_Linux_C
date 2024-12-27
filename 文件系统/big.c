#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc,char**argv)
{

	int fd;
	if(argc < 2)
	{
		fprintf(stderr,"Usage:\r\n");
		exit(1);
	}
	
	fd = open(argv[1],O_WRONLY|O_CREAT|O_TRUNC,0600);
	if (fd < 0)
	{
		perror("open()");
		exit(1);
	}

	lseek(fd,5*1024*1024*100-1,SEEK_SET);

	write(fd,"",1);

	close(fd);	
		
	exit(0);
}

