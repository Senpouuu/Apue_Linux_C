#include <stdio.h>
#include <stdlib.h>
#include <string.h>     
#include <pthread.h>   

#include "mypipe.h"

// 管道队列
struct mypipe_t 
{
    int head;
    int tail;
    int size;
    char buf[PIPE_SIZE];
    pthread_mutex_t mux;
    pthread_cond_t cond;
    int count_read;
    int count_write;

};

static int next(int pos)
{
    return (pos + 1) % PIPE_SIZE;
}


static int mypip_readbyte_ulock(struct mypipe_t *p, void *buf)
{  
    if(p->size <= 0)
        return -1;

    int ret = memcpy(buf, p->buf + p->head, sizeof(p->buf[p->head]));

    p->head = next(p->head); 
    p->size--;
    return 0;
}


static int mypip_writebyte_ulock(struct mypipe_t *p, void* buff)
{
    if(p->size >= PIPE_SIZE)
        return -1;
    int ret = memcpy(p->buf + p->tail, buff, sizeof(p->buf[p->tail]));
    p->tail = next(p->tail);
    p->size++;
    return 0;
}

pipe_t *mypipe_init(void)
{
    struct mypipe_t *pipe_st;
    pipe_st = (sizeof(*pipe_st));
    if(pipe_st == NULL)
        return NULL;

    pipe_st->size = pipe_st->head = pipe_st->tail = 0;
    pipe_st->count_read = pipe_st->count_write = 0;

    pthread_mutex_init(&pipe_st->mux, NULL);
    pthread_cond_init(&pipe_st->cond, NULL);
    
    return pipe_st;
}

int mypipe_read(pipe_t *p, void *buf, int size)
{
    struct mypipe_t *ptr = p;
    
    pthread_mutex_lock(&ptr->mux);
    while(1)
        // 管道为空，且有写者
        if(ptr->size <= 0 && ptr->count_write > 0)
            pthread_cond_wait(&ptr->cond, &ptr->mux);

        if(ptr->size <= 0 && ptr->count_write <= 0)  
        {
            pthread_mutex_unlock(&ptr->mux);
            return -1;
        } 
    // 读取数据
    int i;
    for(i = 0; i < size; i++)
    {
        if(mypip_readbyte_ulock(ptr, &buf[i]) != 0)
            break;
    }          

    pthread_cond_broadcast(&ptr->cond);
    pthread_mutex_unlock(&ptr->mux);

    return i;
}

int mypipe_write(pipe_t *p, void *buf, int size)
{
    struct mypipe_t *ptr = p;

    pthread_mutex_lock(&ptr->mux);
    
    // 等待空间
    while(1)
        if(PIPE_SIZE - ptr->size < size && ptr->count_read > 0)
            pthread_cond_wait(&ptr->cond, &ptr->mux);
        
        if(PIPE_SIZE - ptr->size < size && ptr->count_read <= 0)
        {
            pthread_mutex_unlock(&ptr->mux);
            return -1;
        }
    // 写入数据
    int i; 
    for(i = 0; i < size; i++)  
    {
        if(mypip_writebyte_ulock(ptr, &buf[i]) != 0)
            break;
    }

    pthread_cond_broadcast(&ptr->cond);
    pthread_mutex_unlock(&ptr->mux);
    return i;
}

int mypipe_register(pipe_t *p, int bitmap)
{
    struct mypipe_t *ptr = p;

    if(!(bitmap & (READER | WRITER)))
        return -1;
    
    pthread_mutex_lock(&ptr->mux);
    if(bitmap & READER)
        ptr->count_read++;
    if(bitmap & WRITER)
        ptr->count_write++;
    
    while(ptr->count_read <= 0 || ptr->count_write <= 0)
        pthread_cond_wait(&ptr->cond, &ptr->mux);

    
    pthread_cond_broadcast(&ptr->cond);
    pthread_mutex_unlock(&ptr->mux);
    return 0;   
}

int mypipe_unregister(pipe_t *p, int bitmap)
{
    struct mypipe_t *ptr = p;

    if(!(bitmap & (READER | WRITER)))
        return -1;

    pthread_mutex_lock(&ptr->mux);
    if(bitmap & READER)
        ptr->count_read--;
    if(bitmap & WRITER)
        ptr->count_write--;

    pthread_cond_broadcast(&ptr->cond); 
    pthread_mutex_unlock(&ptr->mux);
    return 0;   
}

void mypipe_destroy(pipe_t *p)
{
    struct mypipe_t *ptr = p;
    pthread_mutex_destroy(&ptr->mux);
    pthread_cond_destroy(&ptr->cond);
    free(ptr);
}