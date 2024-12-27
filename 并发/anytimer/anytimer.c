#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>

#include "anytimer.h"

enum job_stat
{
    AT_JOB_BUSY = 0,
    AT_JOB_DONE,
    AT_JOB_CANCEL
};

struct at_job_st 
{
    int id;
    int sec;
    int time_left;
    at_jobfunc_t *func;
    void *arg;
    int stat;
    int repeat;
};

static struct at_job_st* job_list[AT_MAX_JOBS];
static int inited = 0;
static struct sigaction old_act;
static struct itimerval old_itv;
static void at_exit();

static void handle_alarm(int sig,siginfo_t *info,void *unused)
{
    putchar('.');
    fflush(NULL);
    if(info->si_code!= SI_KERNEL)
        return;
    for(int i = 0; i < AT_MAX_JOBS; i++)
    {
        if(job_list[i] == NULL)
            continue;

        if(job_list[i]->stat == AT_JOB_CANCEL || job_list[i]->stat == AT_JOB_DONE)
            continue;


        job_list[i]->time_left--;
        if (job_list[i]->time_left <= 0)
        {
            if(job_list[i]->repeat == 0)
            {
                job_list[i]->func(job_list[i]->arg);
                job_list[i]->stat = AT_JOB_DONE;
            }
            else
            {
                job_list[i]->func(job_list[i]->arg);
                job_list[i]->time_left = job_list[i]->sec;
            }

        }
    }
}

static void at_init()
{
    struct sigaction at;
    at.sa_sigaction = handle_alarm;
    sigemptyset(&at.sa_mask);
    at.sa_flags = SA_SIGINFO;
    if (sigaction(SIGALRM, &at, &old_act) < 0)
    {
        perror("sigaction");
        exit(1);
    }

    struct itimerval itv;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &itv, &old_itv) < 0)
    {
        perror("setitimer");
        exit(1);
    }

    inited = 1;
    atexit(at_exit);
}

static void at_exit()
{
    sigaction(SIGALRM, &old_act, NULL);
    setitimer(ITIMER_REAL, &old_itv, NULL);
}

int find_pos()
{
    for (int i = 0; i < AT_MAX_JOBS; i++)
    {
        if (job_list[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}

int at_addjob(int sec, at_jobfunc_t *func, void *arg, int repeat)
{
    if (!inited)
        at_init();

    struct at_job_st *job;
    job = malloc(sizeof(*job));
    if (job == NULL)
    {
        perror("malloc");
        return -1;
    }

    int pos = find_pos();
    if (pos < 0)
    {
        free(job);
        return -ENOSPC;
    }

    job->id = pos;
    job->sec = sec;
    job->time_left = sec;
    job->func = func;
    job->arg = arg;
    job->stat = AT_JOB_BUSY;
    job->repeat = repeat;
    job_list[pos] = job;

    printf("add job %d, sec %d\n", pos, sec);
    return pos;
}

int at_canceljob(int id)
{

    if (id < 0 || id >= AT_MAX_JOBS || job_list[id] == NULL)
        return -EINVAL;

    if (job_list[id]->stat == AT_JOB_DONE || job_list[id]->stat == AT_JOB_CANCEL)
        return -ECANCELED;

    job_list[id]->stat = AT_JOB_CANCEL;

    return 0;
}

int at_waitjob(int id)
{
    if (id < 0 || id >= AT_MAX_JOBS || job_list[id] == NULL || job_list[id]->repeat == 1)
        return -EINVAL;

    while (job_list[id]->stat == AT_JOB_BUSY)
        pause();

    if (job_list[id]->stat == AT_JOB_DONE || job_list[id]->stat == AT_JOB_CANCEL)
    {
        free(job_list[id]);
        job_list[id] = NULL;
    }

    return 0;
}
