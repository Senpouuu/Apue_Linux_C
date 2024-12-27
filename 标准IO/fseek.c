#include <stdio.h>
#include <stdlib.h>

#define BUFFSIZE 1024


int main(int argc,char **argv)
{
	FILE *fp;

	fp= fopen(argv[1],"r");
	
	
	if(fp== NULL)
	{
		perror("fopen FPS");
		exit(1);
	}


	fseek(fp,0,SEEK_END);	

	printf("I am in %ld\r\n",ftell(fp));	
	fclose(fp);	

	exit(0);
}
