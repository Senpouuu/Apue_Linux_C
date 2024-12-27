#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#define BUFFSIZE 1024	
#define MAXJOBS 1000

static pthread_mutex_t job_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

// 状态机状态
enum
{
	STAT_R = 1,			// read
	STAT_W,				// write
	STAT_Ex,			// 异常中止
	STAT_T				// 正常退出
};

// 任务状态
enum
{
    STAT_RUNNING = 1,
    STAT_CANCLED,
    STAT_OVER
};


struct rel_fsm_st
{
	int stat;
	int sfd;
	int dfd;
	int pos;
	int len;
	char buff[BUFFSIZE];
	char* errstr;
};

struct rel_job_st
{
	int job_stat;
	int fd1,fd2;
	struct rel_fsm_st fsm12,fsm21;	
	int fd1_save,fd2_save;
	int buff_pos;
	pthread_cond_t job_cond;	
	pthread_mutex_t job_mutex;
	//struct timeval start_time;
	//struct timeval end_time;
};

static struct rel_job_st* jobs[MAXJOBS];



int get_free_jobpos()
{
for(int i=0;i<MAXJOBS;i++)
	{
		if(jobs[i] == NULL)	
			return i;
	}
	return -1;
}


void fsm_driver(struct rel_fsm_st* st){
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

static void* thread_func(void* ptr)
{
	while(1)
	{
		pthread_mutex_lock(&job_mutex);
		for (int i = 0; i < MAXJOBS; i++)
		{
			if (jobs[i] != NULL && jobs[i]->job_stat == STAT_RUNNING 
			&& (jobs[i]->fsm12.stat != STAT_T && jobs[i]->fsm21.stat != STAT_T))
			{
				fsm_driver(&jobs[i]->fsm12);
				fsm_driver(&jobs[i]->fsm21);
				if (jobs[i]->fsm12.stat == STAT_T && jobs[i]->fsm21.stat == STAT_T)
					jobs[i]->job_stat = STAT_OVER;
			}

			if(jobs[i] != NULL && 
			(jobs[i]->job_stat == STAT_OVER || jobs[i]->job_stat == STAT_CANCLED))
			{
				pthread_cond_signal(&jobs[i]->job_cond);
				close(jobs[i]->fd1);
				close(jobs[i]->fd2);
				free(jobs[i]);
				jobs[i] = NULL;
			}

		}
		pthread_mutex_unlock(&job_mutex);
	}
}

static void poll_driver()
{
	pthread_t tid;
	int err = pthread_create(&tid, NULL, thread_func, NULL);
	if (err != 0)
	{
		perror("pthread_create()");
		exit(1);
	}
}

int rel_addjob(int fd1, int fd2)
{
	struct rel_job_st *job;
	pthread_once(&init_once, poll_driver);

	job = malloc(sizeof(*job));
	if (job == NULL)
	{
		perror("malloc()");
		return -ENOMEM;
	}

	job->fd1 = fd1;
	job->fd2 = fd2;
	job->job_stat = STAT_RUNNING;

	job->fd1_save = fcntl(fd1, F_GETFL);
	fcntl(fd1, F_SETFL, job->fd1_save | O_NONBLOCK);

	job->fd2_save = fcntl(fd2, F_GETFL);
	fcntl(fd2, F_SETFL, job->fd2_save | O_NONBLOCK);

	job->fsm12.sfd = fd1;
	job->fsm12.dfd = fd2;
	job->fsm12.stat = STAT_R;

	job->fsm21.sfd = fd2;
	job->fsm21.dfd = fd1;
	job->fsm21.stat = STAT_R;

	pthread_mutex_init(&job->job_mutex, NULL);
	pthread_cond_init(&job->job_cond, NULL);


	pthread_mutex_lock(&job_mutex);
	int pos = get_free_jobpos();
	// 无空闲位置
	if (pos == -1)
	{
		pthread_mutex_unlock(&job_mutex);
		// 恢复
		fcntl(fd1, F_SETFL, job->fd1_save);
		fcntl(fd2, F_SETFL, job->fd2_save);
		free(job);
		return -ENOSPC;
	}
	
	jobs[pos] = job;
	job->buff_pos = pos;
	pthread_mutex_unlock(&job_mutex);

	return pos;
}

int rel_canclejob(int id)
{
	pthread_mutex_lock(&job_mutex);
	if(jobs[id] == NULL)
	{
		pthread_mutex_unlock(&job_mutex);
		return -EINVAL;
	}
	
	if(jobs[id]->job_stat == STAT_RUNNING)
	{
		jobs[id]->job_stat = STAT_CANCLED;
		pthread_mutex_unlock(&job_mutex);
		return 0;
	}	

	if(jobs[id]->job_stat == STAT_OVER)
	{
		pthread_mutex_unlock(&job_mutex);
		return -EBUSY;
	}
	pthread_mutex_unlock(&job_mutex);
}




int rel_waitjob(int id)
{
	if(jobs[id] == NULL)
		return -EINVAL;

	pthread_cond_wait(&jobs[id]->job_cond, &jobs[id]->job_mutex);
	return 0;	
}



int rel_statjob(int id)
{
	if(jobs[id] == NULL)
		return -EINVAL;

	return jobs[id]->job_stat;
}