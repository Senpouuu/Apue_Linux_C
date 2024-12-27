#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>

#define PATT "/etc/*"

static int glob_ERR_FUNC(const char *epath, int eerrno)
{
	puts(epath);
	fprintf(stderr,"ERROR:%s\r\n",strerror(eerrno));
	return 0;
}

int main(int argc,char**argv)
{

	glob_t pglob;
	
	int err = glob(PATT,0,glob_ERR_FUNC,&pglob);
		
	if(err)
	{
		perror("glob()");
		exit(1);
	}	

	for(int i = 0;pglob.gl_pathv != NULL;++i)
		printf("%s\r\n",pglob.gl_pathv[i]);


	globfree(&pglob);
	
	exit(0);
}

