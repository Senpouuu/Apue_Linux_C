#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>



static int ftype(const char* fname)
{
	struct stat statres;
	stat(fname,&statres);
	switch (statres.st_mode & S_IFMT) {
           case S_IFBLK:  return 'B';		break;
           case S_IFCHR:  return 'C';		break;
           case S_IFDIR:  return 'D';       break;
           case S_IFIFO:  return 'I';       break;
           case S_IFLNK:  return 'L';       break;
           case S_IFREG:  return '-';       break;
           case S_IFSOCK: return 'S';       break;
           default:       return '?';       break;
           }

}


int main(int argc,char**argv)
{


	if(argc < 2)
	{
		fprintf(stderr,"Usage:\r\n");
		exit(1);
	}	

	printf("File Type is : %c\r\n",ftype(argv[1]));
	exit(0);
}

