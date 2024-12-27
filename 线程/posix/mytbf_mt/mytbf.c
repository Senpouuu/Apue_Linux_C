#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "mytbf.h"
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

struct mytbf_st
{
	int cps;
	int burst;	
	int token;
	int pos;
	pthread_mutex_t token_mux;
	pthread_cond_t cond;
};

typedef void (*sighandler_t)(int);
static struct mytbf_st* mytbf_buff[MYTBF_MAX];
static sighandler_t alrm_handler_save;
static pthread_t tid;

static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static pthread_mutex_t tbfbuff_mux = PTHREAD_MUTEX_INITIALIZER;

void* tFunc(void* ptr)
{
	while(1)
	{
		for(int i = 0;i<MYTBF_MAX;i++)
		{
			if(mytbf_buff[i] != NULL)
			{
				pthread_mutex_lock(&mytbf_buff[i]->token_mux);
				if(mytbf_buff[i]->token < mytbf_buff[i]->burst)
				{
					mytbf_buff[i]->token += mytbf_buff[i]->cps;
				}	
				pthread_cond_broadcast(&mytbf_buff[i]->cond);
				pthread_mutex_unlock(&mytbf_buff[i]->token_mux);
			}
		}
		sleep(1);	
	}
}



static void module_exit()
{
	pthread_cancel(tid);
	pthread_join(tid,NULL);
	pthread_exit(NULL);
	pthread_mutex_destroy(&tbfbuff_mux);
	free(mytbf_buff);
}



static void module_load()
{
	pthread_create(&tid,NULL,tFunc,NULL);
	atexit(module_exit);
}


static int get_free_pos()
{
	pthread_mutex_lock(&tbfbuff_mux);
	for(int i = 0;i < MYTBF_MAX;i++)
		if(mytbf_buff[i] == NULL)
		{	
			pthread_mutex_unlock(&tbfbuff_mux);
			return i;
		}
	return -1;
}

static int get_ptr_pos(struct mytbf_st* ptr)
{
	pthread_mutex_lock(&tbfbuff_mux);
	for(int i = 0;i < MYTBF_MAX;i++)
		if(mytbf_buff[i] == ptr)
		{
			pthread_mutex_unlock(&tbfbuff_mux);
			return i; 	
		}	

	return -1;
}


mytbf_t* mytbf_init(int cps,int burst)
{
	pthread_once(&init_once,module_load);
	
	struct mytbf_st *me;
	me = malloc(sizeof(*me));

	if(me == NULL)
		return NULL;

	int pos = get_free_pos();
	if(pos == -1)
		return NULL;

	me->token = 0;
	me->burst = burst;
	me->cps = cps;
	me->pos = pos;
	pthread_mutex_init(&me->token_mux,NULL);
	pthread_cond_init(&me->cond,NULL);

	
	pthread_mutex_lock(&tbfbuff_mux);
	mytbf_buff[pos] = me;
	pthread_mutex_unlock(&tbfbuff_mux);
	
	return me;
}

int mytbf_fetchtoken(mytbf_t* ptr,int size)
{
	int n = 0;
	struct mytbf_st* me = ptr;
	if(size <= 0)
		return -EINVAL;

	while(me->token <= 0)
	{

		pthread_cond_wait(&me->cond,&me->token_mux);
	/*
		pthread_mutex_unlock(&me->token_mux);
		sched_yield();
		pthread_mutex_lock(&me->token_mux);
	*/	

	}

	
	if(me->token < size)
		n = me->token;
	else
		n = size;	

	pthread_mutex_lock(&me->token_mux);
	me->token -= n;
	pthread_mutex_unlock(&me->token_mux);
	
	return n;
}


int mytbf_returntoken(mytbf_t* ptr ,int size)
{	
	struct mytbf_st* me = ptr;

	if(size <= 0)
		return -EINVAL;

	pthread_mutex_lock(&me->token_mux);
	me->token += size;
	if(me->token > me->burst)
		me->token = me->burst;
	pthread_cond_broadcast(&me->cond);
	pthread_mutex_unlock(&me->token_mux);
		
	
	return size;

}

int mytbf_free(mytbf_t* ptr)
{
	struct mytbf_st* me = ptr;
	mytbf_buff[me->pos] = NULL;
	free(ptr);
	pthread_mutex_destroy(&me->token_mux);
	pthread_cond_destroy(&me->cond);
	
	return 0;
}

