#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mysem.h"

struct mysem_st
{
	int val;
	pthread_mutex_t mut;
	pthread_cond_t cond;	

};


void* mysem_init(int initval)
{
	struct mysem_st *ptr;
	ptr = malloc(sizeof(*ptr));
		

	ptr->val = initval;
	pthread_mutex_init(&ptr->mut,NULL);
	pthread_cond_init(&ptr->cond,NULL);
	

	if(ptr == NULL)
		return NULL;
	else
		return ptr;

}


int mysem_add(mysem_t* ptr,int num)
{
	struct mysem_st * me = ptr;	

	pthread_mutex_lock(&me->mut);
	
	me->val += num;
	
	pthread_cond_broadcast(&me->cond);
	pthread_mutex_unlock(&me->mut);
	
	return num;	
}



int mysem_sub(mysem_t* ptr,int num)
{
	struct mysem_st * me = ptr;	
	pthread_mutex_lock(&me->mut);

	while(me->val < num)
	{
		pthread_cond_wait(&me->cond,&me->mut);
	}
	
	me->val -= num;

	pthread_mutex_unlock(&me->mut);
	
	return num;	
}


int mysem_free(mysem_t * ptr)
{
	struct mysem_st * me = ptr;	
	pthread_mutex_destroy(&me->mut);
	pthread_cond_destroy(&me->cond);
	free(me);
	return 0;
}



