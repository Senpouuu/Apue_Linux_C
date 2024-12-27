#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define FNAME	"./out"
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

static int fd;
// 创建一个守护进程
static int daemonize()
{
	pid_t pid = fork();
 	
	if(pid < 0)
	{
		perror("fork()");
		return -1;
	}

	if(pid > 0)
		exit(0);
	else
	{
		fd = open("/dev/null",O_RDWR);
		if(fd < 0)
			return -1;


		// 守护进程不需要012三个流，重定向至垃圾堆
		dup2(fd,0);
		dup2(fd,1);
		dup2(fd,2);		
		
		if(fd > 2)
			close(fd);
				
		setsid();
		// 改变工作路径，防止路径被删除，移动....
		chdir("/home/gulu/桌面/linux_c/进程/daemon_dir");
		
	}
}

static void daemon_exit(int sig)
{
	close(fd);
	closelog();
	exit(0);
}

int main(int argc,char**argv)
{
	openlog("mydaemon",LOG_PID,LOG_DAEMON);
	
	struct sigaction sigact;
	sigact.sa_handler = daemon_exit;
	sigemptyset(&sigact.sa_mask);
	sigaddset(&sigact.sa_mask,SIGQUIT);
	sigaddset(&sigact.sa_mask,SIGTERM);
	sigaddset(&sigact.sa_mask,SIGINT);
	sigact.sa_flags = 0; 	
	
	sigaction(SIGINT,&sigact,NULL); 
	sigaction(SIGINT,&sigact,NULL); 
	sigaction(SIGINT,&sigact,NULL); 

//	signal(SIGINT,daemon_exit);	
//	signal(SIGQUIT,daemon_exit);
//	signal(SIGTERM,daemon_exit);

	if(daemonize() < 0)
	{
		syslog(LOG_ERR,"DAEMONLIZE FAILED!");
		exit(1);
	}
	else
	{
		syslog(LOG_INFO,"DAEMONLIZE SUCCESS!");
	}	

	// 父进程已退出，下面是守护进程要做的事....
	int fd = open(FNAME,O_WRONLY|O_CREAT|O_APPEND);

	
	if(fd < 0)
	{
		syslog(LOG_ERR,"DAEMONLIZE OPEN FD FAILED:%s",strerror(errno));
		exit(1);
	}	
	else
		syslog(LOG_INFO,"DAEMONLIZE OPEN SUCCESSFUL!");

	for(int i = 1;i < 100;i++)
	{
		dprintf(fd,"%d\n",i);
		syslog(LOG_DEBUG,"DAEMONLIZE WRITING:%d!",i);
		sleep(1);
	}

	close(fd);
	closelog();
	
	exit(0);
}

