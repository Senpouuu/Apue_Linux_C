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

#include "proto.h"


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
    printf("send stamp to client\n");
}

int main(int argc, char *argv[])
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sfd < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in laddr,raddr;
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

    char ipstr[16];
    int new_sfd;
    memset(&raddr, 0, sizeof(raddr));
    while(1)
    {
        socklen_t s_size = sizeof(raddr);
        new_sfd = accept(sfd,(void *)&raddr, &s_size);
        if(new_sfd < 0 && (errno != EINTR || errno != EAGAIN)) 
        {
            perror("accept");
            exit(1);    
        }
        inet_ntop(AF_INET,&raddr.sin_addr.s_addr, ipstr,sizeof(ipstr));
        printf("connect from %s:%d\n",ipstr,ntohs(raddr.sin_port));
        sever_job(new_sfd);             
    } 
    
    
    close(sfd);


}