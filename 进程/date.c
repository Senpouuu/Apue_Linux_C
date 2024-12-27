#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc,char**argv)
{
	puts("Begin!\n");
	
	execl("/bin/date","date","+%s",NULL);
	perror("execl()");
	exit(1);
	
	puts("End!\n");

	exit(0);
}

