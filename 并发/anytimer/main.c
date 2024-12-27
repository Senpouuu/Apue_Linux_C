#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>  
#include <unistd.h>
#include <errno.h>

#include "anytimer.h"

#define NO_REPEAT 0
#define REPEAT 1

void job1_func(void *arg)
{
    printf("job1_func called\n");
}

void job2_func(void *arg)
{
    printf("job2_func called\n");
}

void job3_func(void *arg)
{
    printf("job3_func called\n");    
}



int main() 
{
    int job1,job2,job3;
    job1 = at_addjob(5,job1_func,"job1",NO_REPEAT);
    if(job1 < 0)
    {
        fprintf(stderr,"add job1 failed\n %s",strerror(-job1));
        exit(1);
    }

    job2 = at_addjob(2,job2_func,"job2",NO_REPEAT);
    if(job2 < 0)
    {
        fprintf(stderr,"add job2 failed\n %s",strerror(-job2));
        exit(1);
    }

    job3 = at_addjob(7,job3_func,"job3",REPEAT);
    if(job3 < 0)
    {
        fprintf(stderr,"add job3 failed\n %s",strerror(-job3));
        exit(1);
    }
    puts("all jobs added");

    while(1);



    exit(0);
}
