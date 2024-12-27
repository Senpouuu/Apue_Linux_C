#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>

static off_t flen(const char* path)
{
	
	struct stat statbuff;
	if (stat(path,&statbuff)< 0)
	{
		perror("stat()");
		exit(1);
	}
	else
		return statbuff.st_size; 
}



int main(int argc,char**argv)
{
	if(argc < 2)
	{
		fprintf(stderr,"Usage:\r\n");
		exit(1);
	}	

	printf("%ld\n",flen(argv[1]));
	exit(0);
}

