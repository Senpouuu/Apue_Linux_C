#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFSIZE 1024



int main(int argc,char**argv)
{
	int sfd,dfd;	
	char* buff[BUFFSIZE];
	int len;	
	int ret;	
	int pos;	

	if(argc < 3)
	{
		fprintf(stderr,"Usage:\r\n");
		exit(1);
	}	
	
	sfd = open(argv[1],O_RDONLY);
	
	if(sfd < 0)
	{
		perror("open1()");
		exit(1);
	}


	dfd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0600);

	if(dfd < 0)
	{
		perror("open2()");
		exit(1);
	}
	

	while(1)
	{
		len = read(sfd,buff,BUFFSIZE);
		if(len < 0)
		{
			perror("read()");
			break;	
		}
		if(len == 0)
			break;

		pos = 0;
		while(len > 0)
		{
			ret = write(dfd,buff + pos,len);
			if(ret < 0)
			{
				perror("write()");
				exit(1);
			}
			pos += ret;
			len -= ret;
		}	
	}
	close(dfd);
	close(sfd);

	exit(0);
}
