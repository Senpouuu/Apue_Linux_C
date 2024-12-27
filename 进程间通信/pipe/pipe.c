#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

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

    // Read    
    if(pid == 0) // child process
    {
        close(pipefd[1]);
        // stdin -> pipefd[0]
        dup2(pipefd[0], stdin);
        int fd = open("/dev/null",O_RDWR);
        dup2(fd, stdout);
        dup2(fd, stderr);
        close(fd);

        execl("/usr/bin/mpg123","mpg123","-",NULL);
        perror("execl");        
        exit(0);
    }
    // Write
    if(pid > 0) // parent process
    {
        close(pipefd[0]);
        // Read MP3 file to Pipe

        wait(NULL);
    }

    exit(0);

}