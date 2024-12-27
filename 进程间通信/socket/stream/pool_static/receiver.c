#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>

#include "proto.h"

#define PROC_NUM 5

void sever_loop(int sfd);

static void sever_job(int sfd)
{
    char buf[1024];
    int len = sprintf(buf,FMT_STAMP,(long long)time(NULL));
    
    if(len < 0)
    {
        perror("sprintf");
        exit(1);
    }
    
    while(send(sfd,buf,len,0) < 0)
    {
        perror("send"); 
        exit(1);
    }

    sleep(3);
    printf("server job done\n");
}

int main(int argc, char *argv[])
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in laddr;
    if(sfd < 0)
    {
        perror("socket");
        exit(1);
    }

    memset(&laddr, 0, sizeof(laddr));
    laddr.sin_family = AF_INET;
    inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr.s_addr);
    laddr.sin_port = htons(atoi(SERVER_PROT));
    
    int val = 1;
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    if(bind(sfd,(void *)&laddr,sizeof(laddr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    
    if(listen(sfd,200) < 0)
    {  
        perror("listen"); 
        exit(1);
    }


    for (int i = 0; i < PROC_NUM; i++)
    {
        int pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
        if (pid == 0) // child process
        {
            sever_loop(sfd);
        }

        if (pid > 0) // parent process
        {
        }
    }

    for (int i = 0; i < PROC_NUM; i++)
        wait(NULL);

    close(sfd);
    exit(0);
}


void sever_loop(int sfd)
{
    char ipstr[16];
    int new_sfd;

    struct sockaddr_in raddr;
    memset(&raddr, 0, sizeof(raddr));
    socklen_t s_size = sizeof(raddr);

    while(1)
    {
        new_sfd = accept(sfd, (void *)&raddr, &s_size);
        if (new_sfd < 0 && (errno != EINTR || errno != EAGAIN))
        {
            perror("accept");
            exit(1);
        }
        inet_ntop(AF_INET, &raddr.sin_addr.s_addr, ipstr, sizeof(ipstr));
        printf("[%d]connect from %s:%d\n",getpid(), ipstr, ntohs(raddr.sin_port));
        sever_job(new_sfd);
        close(new_sfd);
    }
}