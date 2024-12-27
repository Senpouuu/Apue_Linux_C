#ifndef MYSEM_H__
#define MYSEM_H__

typedef void mysem_t;

mysem_t *mysem_init(int initval);

int mysem_add(mysem_t* ptr,int num);

int mysem_sub(mysem_t* ptr,int num);


int mysem_free(mysem_t * ptr);








#endif
