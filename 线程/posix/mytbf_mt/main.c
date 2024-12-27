#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "mytbf.h"
#define BUFFSIZE 256


int main(int argc,char**argv)
{
	int sfd;	
	char* buff[BUFFSIZE];
	int len;	
	int ret;	
	int pos;	
	
	int cps = 50,brust = 1024;	

	mytbf_t* tbfptr = mytbf_init(cps,brust);

	if(argc < 2)
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

	while(1)
	{
		int size = mytbf_fetchtoken(tbfptr,BUFFSIZE);
		
		len = read(sfd,buff,size);
		
		if(len < size)
			mytbf_returntoken(tbfptr,size-len);
	
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
			ret = write(1,buff + pos,len);
			if(ret < 0)
			{
				perror("write()");
				exit(1);
			}
			pos += ret;
			len -= ret;
		}
	}
	close(sfd);
	mytbf_free(tbfptr);
	exit(0);
}
