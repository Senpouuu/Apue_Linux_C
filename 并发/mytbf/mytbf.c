#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "mytbf.h"
#include <unistd.h>
#include <errno.h>


struct mytbf_st
{
	int cps;
	int burst;	
	int token;
	int pos;
};

typedef void (*sighandler_t)(int);
static int INITED = 0; 
static struct mytbf_st* mytbf_buff[MYTBF_MAX];
static sighandler_t alrm_handler_save;


void alrm_handler(int sig)
{
	alarm(1);
	for(int i = 0;i<MYTBF_MAX;i++)
	{
		if(mytbf_buff[i] != NULL)
		{
			if(mytbf_buff[i]->token < mytbf_buff[i]->burst)
				mytbf_buff[i]->token += mytbf_buff[i]->cps;
		}
	}

}

static void module_exit()
{
	signal(SIGALRM,alrm_handler_save);
	alarm(0);
	free(mytbf_buff);
}



static void module_load()
{
	alrm_handler_save = signal(SIGALRM,alrm_handler);
	alarm(1);
	INITED = 1;
	atexit(module_exit);
}




static int get_free_pos()
{
	for(int i = 0;i < MYTBF_MAX;i++)
		if(mytbf_buff[i] == NULL)
			return i;
	return -1;
}

static int get_ptr_pos(struct mytbf_st* ptr)
{
	for(int i = 0;i < MYTBF_MAX;i++)
		if(mytbf_buff[i] == ptr)
			return i;
	return -1;
}


mytbf_t* mytbf_init(int cps,int burst)
{
	if(INITED == 0)
	{
		module_load();
		INITED = 1;
	}
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
	mytbf_buff[pos] = me;

	return me;
}

int mytbf_fetchtoken(mytbf_t* ptr,int size)
{
	int n = 0;
	struct mytbf_st* me = ptr;
	if(size <= 0)
		return -EINVAL;

	while(me->token <= 0)
		pause();	
	
	if(me->token < size)
		n = me->token;
	else
		n = size;	
	
	me->token -= n;

	return n;
}


int mytbf_returntoken(mytbf_t* ptr ,int size)
{	
	struct mytbf_st* me = ptr;

	if(size <= 0)
		return -EINVAL;
	
	me->token += size;
	if(me->token > me->burst)
		me->token = me->burst;

	return size;

}

int mytbf_free(mytbf_t* ptr)
{
	struct mytbf_st* me = ptr;
	mytbf_buff[me->pos] = NULL;
	free(ptr);
	return 0;
}
