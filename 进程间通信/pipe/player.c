#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main()
{   
    int pipefd[2];

    if(pipe(pipefd) < 0)
    {
        perror("pipe");
        exit(1);
    }
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }
    
    if(pid == 0) // child process
    {
        char buf[1024];
        close(pipefd[1]);
        ssize_t len = read(pipefd[0], buf, 1024);
        write(1, buf, len);
        fflush(stdout);
        close(pipefd[0]);
        exit(0);
    }

    if(pid > 0) // parent process
    {
        char* str = "hello, world!\r\n";
        close(pipefd[0]);
        write(pipefd[1], str, strlen(str)+1);
        close(pipefd[1]);
        wait(NULL);
    }

    exit(0);

}