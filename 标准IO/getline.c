#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc,char**argv)
{
	FILE* fp;
	char* linebuff = NULL;		
	size_t linesize = 0;	


	if(argc < 2)
	{
		fprintf(stderr,"Usage:\r\n");
		exit(1);
	}	
	
	fp = fopen(argv[1],"r");
	
	if(fp == NULL)
	{
		perror("fopen");
		exit(1);
	}
	
	while(1)
	{
		if(getline(&linebuff,&linesize,fp) < 0)
			break;
		printf("%s\n",linebuff);
		printf("%ld\n",linesize);
	}
 
	fclose(fp);
	exit(0);
}

