#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#define PATH "/etc"


int main(int argc,char**argv)
{
	DIR* dir = NULL;
	struct dirent* dirres;	


	dir = opendir(PATH);
	
	while(1)
	{
		dirres = readdir(dir);
		if(dirres == NULL)
			break;

		printf("%s\r\n",dirres->d_name);
	}

	closedir(dir);
	
	exit(0);
}

