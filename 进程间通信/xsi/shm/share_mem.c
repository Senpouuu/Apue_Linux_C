#include <stdio.h> 
#include <stdlib.h> 
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define MEM_SIZE 1024


int main(int argc, char** argv)
{
    int shmid = shmget(IPC_PRIVATE, MEM_SIZE, 0600);
    char* ptr;

    if(shmid < 0)
    {
        perror("shmget");
        exit(1);
    }

    pid_t ret = fork();
    if(ret < 0)
    {
        perror("fork");
        exit(1);
    }

    if(ret == 0)     //child process writes to shared memory
    {
        ptr = shmat(shmid, NULL, 0);
        if(ptr == (void *)-1)
        {
            perror("shmat");
            exit(1);
        }
        memcpy(ptr, "Hello, world!", 13);
        printf("Child wrote: %s\n", ptr);
        shmdt(ptr);
        exit(0);
    }
    if(ret > 0)      //parent process reads from shared memory
    {
        wait(NULL);
        ptr = shmat(shmid, NULL, 0);
        if(ptr == (void *)-1)
        {
            perror("shmat");
            exit(1);
        }
        printf("I am reading: %s\n",ptr);
        shmdt(ptr);
        shmctl(shmid, IPC_RMID, 0);
        exit(0);
    }

} 