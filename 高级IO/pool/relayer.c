#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#define TTY1 "/dev/tty3"
#define TTY2 "/dev/tty4"
#define BUFFSIZE 1024

enum
{
	STAT_R = 1,			// read
	STAT_W,				// write
	STAT_AUTO,			// 自动模式
	STAT_Ex,			// 异常中止
	STAT_T				// 正常退出
};
struct fsm_st
{
	int stat;
	int sfd;
	int dfd;
	int pos;
	int len;
	char buff[BUFFSIZE];
	char* errstr;
};


void fsm_driver(struct fsm_st* st)
{
	int ret;
	switch(st->stat)
	{
		case STAT_R:
			// 等待读结束
			st->len = read(st->sfd,st->buff,BUFFSIZE);
			if(st->len == 0)
				st->stat = STAT_T;
			else if(st->len < 0)
			{
				if(errno == EAGAIN)
					st->stat = STAT_R;
				else
				{
					st->stat = STAT_Ex;
					st->errstr = "read()";
				}
			}
			else
			{
				st->pos = 0;
				st->stat = STAT_W;
			}	
			break;

		case STAT_W:
			// cp sfd -> dfd....
			ret = write(st->dfd,st->buff + st->pos,st->len);
			if(ret < 0)
			{
				if(errno == EAGAIN)
					st->stat = STAT_W;
				else
				{
					st->stat = STAT_Ex;
					st->errstr = "write()";
				}
			}
			else
			{
				st->len -= ret;
				st->pos += ret;
				if(st->len == 0)
					st->stat = STAT_R;
				else
					st->stat = STAT_W;
			}			

			break;
		
		case STAT_Ex:
			perror(st->errstr);	
			st->stat = STAT_T;
			break;

		case STAT_T:
			/*close ... free...*/
			break;			

		default: 
		abort();
		break;	
	}
}


static void relay(int fd1,int fd2)
{
	int fd1_save,fd2_save;
	struct fsm_st st_rR,st_lR;	
	struct pollfd pfd[2];


	fd1_save = fcntl(fd1,F_GETFL);
	fcntl(fd1,F_SETFL,fd1_save|O_NONBLOCK);

	fd2_save = fcntl(fd2,F_GETFL);
	fcntl(fd2,F_SETFL,fd2_save|O_NONBLOCK);
	
	st_rR.stat = STAT_R;
	st_rR.sfd = fd1;
	st_rR.dfd = fd2;

	st_lR.stat = STAT_R;
	st_lR.sfd = fd2;
	st_lR.dfd = fd1;


	pfd[0].fd = fd1;
	pfd[1].fd = fd2;


	while(st_lR.stat!= STAT_T || st_rR.stat!= STAT_T)
	{
		
		pfd[0].events = 0;
		pfd[1].events = 0;

		if(st_lR.stat == STAT_R)
		{
			//监视fd1 可Read
			pfd[0].events |= POLLIN;
		}	

		if(st_lR.stat == STAT_W)
		{
			//监视fd2 可Write			
			pfd[1].events |= POLLOUT;
		}	
			
		if(st_rR.stat == STAT_R)
		{
			//监视fd2 可Read
			pfd[1].events |= POLLIN;
		}	

		if(st_rR.stat == STAT_W)
		{
			//监视fd1 可Write			
			pfd[0].events |= POLLOUT;
		}	
 		
		if(st_lR.stat < STAT_AUTO || st_rR.stat < STAT_AUTO)
		{
			while(poll(pfd,2,-1) < 0)
			{
				if(errno == EINTR)
					continue;
				perror("poll()");
				exit(1);
			}
		}


		if(st_lR.stat > STAT_AUTO)
			fsm_driver(&st_lR);
		if(st_rR.stat > STAT_AUTO)
			fsm_driver(&st_rR);

		if(pfd[0].revents & POLLIN || pfd[1].revents & POLLOUT)
			fsm_driver(&st_lR);

		if(pfd[1].revents & POLLIN || pfd[0].revents & POLLOUT)
			fsm_driver(&st_rR);

	}

	// 恢复
	fcntl(fd1,F_SETFL,fd1_save);
	fcntl(fd2,F_SETFL,fd2_save);
}


int main(int argc,char**argv)
{
	int fd1,fd2;
	fd1 = open(TTY1,O_RDWR);
	if(fd1 < 0)
 	{
		perror("open(TTY1)");
		exit(1);
	}

	fd2 = open(TTY2,O_RDWR|O_NONBLOCK);
	if(fd2 < 0)
	{
		perror("open(TTY2)");
		exit(1);
	}
	
	relay(fd1,fd2);

	close(fd2);
	close(fd1);	

	exit(0);
}

