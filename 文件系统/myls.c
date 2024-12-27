#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <glob.h>
#include <string.h>
#define PATH "./"
#define PATT "./*"
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>

const char* getTypeAuth(mode_t gl_mode)
{
	char* fileType;
	static char fileAuth[1024] ; 
	memset(fileAuth,0,sizeof(fileAuth));		
	
	switch (gl_mode & S_IFMT) {
           case S_IFBLK:  fileType =  "B";		break;
           case S_IFCHR:  fileType =  "C";		break;
           case S_IFDIR:  fileType =  "D";       break;
           case S_IFIFO:  fileType =  "I";       break;
           case S_IFLNK:  fileType =  "L";       break;
           case S_IFREG:  fileType =  "-";       break;
           case S_IFSOCK: fileType =  "S";       break;
           default:       fileType =  "?";       break;
    }
	
	strncat(fileAuth,fileType,1);

	
	// 检查权限并拼接
	if (gl_mode & S_IRUSR)  // 所有者可读
	    strncat(fileAuth, "r", 1);
	else
	    strncat(fileAuth, "-", 1); // 没有读权限
	
	if (gl_mode & S_IWUSR)  // 所有者可写
	    strncat(fileAuth, "w", 1);
	else
	    strncat(fileAuth, "-", 1); // 没有写权限
	
	if (gl_mode & S_IXUSR)  // 所有者可执行
	    strncat(fileAuth, "x", 1);
	else
	    strncat(fileAuth, "-", 1); // 没有执行权限
	
	if (gl_mode & S_IRGRP)  // 组用户可读
	    strncat(fileAuth, "r", 1);
	else
	    strncat(fileAuth, "-", 1); // 没有读权限
	
	if (gl_mode & S_IWGRP)  // 组用户可写
	    strncat(fileAuth, "w", 1);
	else
	    strncat(fileAuth, "-", 1); // 没有写权限
	
	if (gl_mode & S_IXGRP)  // 组用户可执行
	    strncat(fileAuth, "x", 1);
	else
	    strncat(fileAuth, "-", 1); // 没有执行权限
	
	if (gl_mode & S_IROTH)  // 其他用户可读
	    strncat(fileAuth, "r", 1);
	else
	    strncat(fileAuth, "-", 1); // 没有读权限
	
	if (gl_mode & S_IWOTH)  // 其他用户可写
	    strncat(fileAuth, "w", 1);
	else
	    strncat(fileAuth, "-", 1); // 没有写权限
	
	if (gl_mode & S_IXOTH)  // 其他用户可执行
	    strncat(fileAuth, "x", 1);
	else
    	strncat(fileAuth, "-", 1); // 没有执行权限
	
	return fileAuth;	

}

const char* convertgid(gid_t gid) 
{
	struct passwd* ppasswd = NULL;
	
	ppasswd = getpwuid(gid);
	
	if(ppasswd == NULL)
	{
		perror("getpwuid()");
		exit(1);
	}
	
	return	ppasswd->pw_name;

}

const char* convertime(time_t time)
{
	static char timebuff[128] ; 
	memset(timebuff,0,sizeof(timebuff));		
	struct tm *ntm;
	ntm = localtime(&time);	

	if (strftime(timebuff, sizeof(timebuff), "%Y-%m-%d %H:%M:%S", ntm) == 0) {
            perror("格式化时间字符串失败");
            exit(EXIT_FAILURE);
    }
	return timebuff;

}


int main(int argc,char**argv)
{
	glob_t globres;	
	char lsbuff[1024];	
	struct stat statres;
	memset(lsbuff, 0, sizeof(lsbuff)); // 清空缓冲区	
	int flag = 0;
	while(1)
	{
		char c = getopt(argc,argv,"-la");
		if(c < 0)
			break;

		switch(c)
		{
			case 1:
			glob(strncat(argv[optind - 1],"/*",2),0,NULL,&globres);
			//glob(argv[optind - 1],0,NULL,&globres);
			
			for(int i = 0;i < globres.gl_pathc;++i)
			{
				if(stat(globres.gl_pathv[i],&statres) == -1)
				{
					perror("stat");
					exit(1);
				}
				sprintf(lsbuff, "%s  %lu  %s  %s  %lu  %s  %s", 
		        getTypeAuth(statres.st_mode), 
		        statres.st_nlink, 
		        convertgid(statres.st_gid), 
		        convertgid(statres.st_gid), 
		        (long long)statres.st_size, 
		        convertime(statres.st_mtime),
				globres.gl_pathv[i]);
			
				printf("%s\n",lsbuff);	
			}
			flag = 1;		
			break;

			case 'a': 
			break;
			
			case 'l':
			glob(PATT,0,NULL,&globres);
			for(int i = 0;i < globres.gl_pathc;++i)
			{
				if(stat(globres.gl_pathv[i],&statres) == -1)
				{
					perror("stat");
					exit(1);
				}
				sprintf(lsbuff, "%s  %lu  %s  %s  %lu  %s  %s", 
			    getTypeAuth(statres.st_mode), 
			    statres.st_nlink, 
			    convertgid(statres.st_gid), 
			    convertgid(statres.st_gid), 
			    (long long)statres.st_size, 
			    convertime(statres.st_mtime),
				globres.gl_pathv[i]);
			
				printf("%s\n",lsbuff);	
			}		
			flag = 1;
			break;
			
			default:
			break;
		}
	}
	
	if(flag == 0)
	{
		glob(PATT,0,NULL,&globres);
		for(int i = 0;i < globres.gl_pathc;++i)
		{
			if(stat(globres.gl_pathv[i],&statres) == -1)
			{
				perror("stat");
				exit(1);
			}
			sprintf(lsbuff, "%s  ", 
			globres.gl_pathv[i]);
			printf("%s\n",lsbuff);	
		}		
	}		 
	globfree(&globres);
	exit(0);
}

