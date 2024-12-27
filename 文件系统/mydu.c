#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>
#include <glob.h>
#include <string.h>

#define PATHSIZE 1024

static int8_t path_noloop(const char* path)
{
	char *pos = NULL;
	pos = strrchr(path,'/');
	
	if(pos == NULL)
	{
		perror("strrchr");
		exit(1);
	}
	
	if(strcmp(pos+1,".") == 0 || strcmp(pos+1,"..") == 0)
		return 0;
		
	return 1;	

}

static int64_t mydu(const char* path)
{
	struct stat statres;
	char nextpath[PATHSIZE];
	glob_t globres;	
	int64_t sum = 0;	

	if(lstat(path,&statres) < 0)
	{
		perror("lstat()");
		exit(1);
	}
	if(!S_ISDIR(statres.st_mode))
		return statres.st_blocks;

	strncpy(nextpath,path,PATHSIZE);
	strncat(nextpath,"/*",PATHSIZE);
	glob(nextpath,0,NULL,&globres);
	
	strncpy(nextpath,path,PATHSIZE);
	strncat(nextpath,"/.*",PATHSIZE);
	glob(nextpath,GLOB_APPEND,NULL,&globres);
	
	for(int i = 0;i<globres.gl_pathc;i++)
		if(path_noloop(globres.gl_pathv[i]))
			sum += mydu(globres.gl_pathv[i]);
			

	sum += statres.st_blocks;
	printf("%ld   %s\r\n",sum,path);		
	
	globfree(globres);
	return sum;	

}

int main(int argc,char**argv)
{
	if(argc < 2)
	{
		fprintf(stderr,"Usage:\r\n");
		exit(1);
	}	
	printf("SIZE:%ld\r\n",mydu(argv[1])/2);
	exit(0);
}

