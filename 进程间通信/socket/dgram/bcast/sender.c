#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "proto.h"

int main(int argc, char** argv)
{
    srand(time(NULL));
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct msg_st send_buffer;
    strcpy(send_buffer.name, "Gulu");
    send_buffer.chinese = htonl(rand() % 100);
    send_buffer.math = htonl(rand() % 100);
    send_buffer.english = htonl(rand() % 100);

    if(sfd < 0)
    {
        perror("socket");
        exit(1);
    }

    int var = 1;

    if(setsockopt(sfd,SOL_SOCKET,SO_BROADCAST,&var,sizeof(var)) < 0)
    {
        perror("setsockopt");
        exit(1);    
    }

    // bind();
    struct sockaddr_in raddr;
    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(atoi(RCV_PORT));
    
    inet_pton(AF_INET, "255.255.255.255", &raddr.sin_addr.s_addr);
    

    char ip_recv[30];
    if(sendto(sfd, &send_buffer, sizeof(send_buffer), 0, (void *)&raddr, sizeof(raddr)) < 0)
    {
        perror("sendto");
        exit(1);
    }

    inet_ntop(AF_INET, &raddr.sin_addr.s_addr, ip_recv, sizeof(ip_recv));

    printf("send message to %s:%d\n", ip_recv, ntohs(raddr.sin_port));
    puts("OK\n");

    close(sfd);



    exit(0);
}
