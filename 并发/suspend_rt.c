#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>      /* Definition of SYS_* constants */

#define MYRTS (SIGRTMIN+7)


static void mysig_handler(int s)
{
 	write(1,"Want to Ini Me??",sizeof("Want to Ini Me??"));	
}

int main(int argc,char**argv)
{
	sigset_t sig_set,old_set,save_set;
	signal(MYRTS,mysig_handler);
	sigemptyset(&sig_set);
	sigaddset(&sig_set,MYRTS);
	
	sigprocmask(SIG_UNBLOCK,NULL,&save_set);

	for(int j = 0;j < 1000;j++)
	{
		sigprocmask(SIG_BLOCK,&sig_set,&old_set);
		for(int i = 0;i < 3;i++)
		{
			write(1,"*",1);
			sleep(1);
		}
		write(1,"\n",1);
		sigprocmask(SIG_SETMASK,&old_set,&sig_set);
		sigsuspend(&old_set);
		//pause();sigsuspend(&old_set);
	}
	
	sigprocmask(SIG_SETMASK,&save_set,&sig_set);
	exit(0);
}


