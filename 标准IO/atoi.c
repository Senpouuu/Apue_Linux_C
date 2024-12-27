#include <stdio.h>
#include <stdlib.h>

int main(int argc,char**argv)
{	

	char buff [1024];
	int year = 2024;
	int month = 8;
	int day = 28;	

	sprintf(buff,"%d-%d-%d",year,month,day);
	puts(buff);

	exit(0);
}

