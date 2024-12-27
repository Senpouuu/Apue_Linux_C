#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
	FILE *fps,*fpd;
	
	char ch;	
	fps = fopen(argv[1],"r");
	fpd = fopen(argv[2],"w");
	
	if(argc < 3)
	{
		fprintf(stderr,"Useage:%s <src_file> <dest_file>\r\n",argv[0]);
		exit(1);
	}


	if(fps == NULL)
	{
		perror("fopen FPS:");
		exit(1);
	}
	
	if(fpd == NULL)
	{
		fclose(fps);
		perror("fopen FPD:");
		exit(1);
	}	

	while(1)
	{
		ch = fgetc(fps);
		if(ch == EOF)
			break;
		fputc(ch,fpd);
		
	}
	fclose(fpd);
	fclose(fps);	

	exit(0);
}
