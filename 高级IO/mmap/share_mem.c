#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MEM_SIZE 1024

int main(int argc, char** argv)
{

    char* addr = mmap(NULL,MEM_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    
    if(addr == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    pid_t pid = fork();

    if(pid == 0)
    {
        // child process
        printf("Child process: pid = %d\n",getpid());
        for(int i=0;i<13;i++)
            printf("%c",addr[i]);
        exit(0);
    }
    else    
    {
        // parent process
        wait(NULL);
        printf("Parent process: pid = %d\n",getpid());
        memcpy(addr,"Hello,world!",13);
        exit(0);
    }

    printf("\n");
    munmap(addr,MEM_SIZE);
} 