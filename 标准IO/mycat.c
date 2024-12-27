#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
	FILE *fps,*fpd;
	
	char ch;	
	fps = fopen(argv[1],"r");
	
	if(argc < 2)
	{
		fprintf(stderr,"Useage:%s <src_file> <dest_file>\r\n",argv[0]);
		exit(1);
	}


	if(fps == NULL)
	{
		perror("fopen FPS:");
		exit(1);
	}
	
	while(1)
	{
		ch = fgetc(fps);
		if(ch == EOF)
			break;
		fputc(ch,stdout);
		
	}
	fclose(fps);	

	exit(0);
}
