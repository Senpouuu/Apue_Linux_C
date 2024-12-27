#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>


int main(int argc,char**argv)
{

	if(argc < 2)
	{
		fprintf(stderr,"Usage:\r\n");
		exit(1);
	}	


	struct passwd* ppasswd = NULL;
	
	ppasswd = getpwuid(atoi(argv[1]));
	
	if(ppasswd == NULL)
	{
		perror("getpwuid()");
		exit(1);
	}
	
	printf("%s\r\n",ppasswd->pw_name);
	exit(0);
}

