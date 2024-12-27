#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include <sys/wait.h>
#include <string.h>

#define DELIMS " \t\n"

struct cmd_st
{
	glob_t globres;
	int is_inter_cmd;
};


static void prompt()
{
	printf("Enter your command$:");
}

static void parse(char* line, struct cmd_st* cmd)
{
	char* tok = NULL;
	int i = 0;
	while(1)
	{
		tok = strsep(&line,DELIMS);  // 此函数解析会自动向前移动
		if(tok == NULL)				 // 解析到最后一位
			break;

		if(tok[0] == '\0')			// 解析到空串 
			continue;
		
		if(i == 0)
		{
			glob(tok,GLOB_NOCHECK,NULL,&cmd->globres);	// 第一次不追加，申请空间并写入
			i = 1;
		}
		else
			glob(tok,GLOB_NOCHECK|GLOB_APPEND,NULL,&cmd->globres);	// globres存放解析结果,无需匹配追加模式
	}
	cmd -> is_inter_cmd = 0;
}


int main(int argc,char**argv)
{
	char* linebuff = NULL;
	size_t linebuff_size = 0;
	struct cmd_st cmd;
	
	while(1)
	{
		prompt();

		if(getline(&linebuff,&linebuff_size,stdin) < 0)
		{
			perror("getline()");
			exit(1);
		}

		parse(linebuff,&cmd);
		
		if(cmd.is_inter_cmd)	 // 内部命令
		{}
		else	 // 外部命令
		{
			pid_t pid;
			pid = fork();
			if(pid == 0) // child
			{
				execvp(cmd.globres.gl_pathv[0],cmd.globres.gl_pathv);		// 第一个元素ls只是一个名字，后面整个解析的数组才是关键
				perror("execvp()");
				exit(1);

			}
			if(pid > 0) //parent
			{
				wait(NULL);
			}
		}
			
	}

	globfree(&cmd.globres);
	exit(0);
}

