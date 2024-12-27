#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "proto.h"

#define RCV_MAX 512

int main(int argc, char *argv[])
{
    int sd = socket(AF_INET, SOCK_DGRAM, 0);        //取得socket描述符
    struct msg_st* pbuffer_recv;
    size_t recv_size = RCV_MAX;
    pbuffer_recv = malloc(recv_size);
    if(pbuffer_recv == NULL)
    {
        perror("malloc");
        exit(1);
    }


    struct sockaddr_in laddr,raddr;

    if(sd < 0)
    {
        perror("socket");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(RCV_PORT));
    inet_pton(AF_INET, "0.0.0.0",&laddr.sin_addr.s_addr);   //将IP地址转换为网络字节序



    if(bind(sd,(void *)&laddr,sizeof(laddr)) < 0)            //给socket取得地址
    {
        perror("bind");
        exit(1);
    }

    /*ATTENTION!*/
    socklen_t raddrlen = sizeof(raddr);
    char sendip[30];
    while(1)
    {
        recvfrom(sd, pbuffer_recv, recv_size,0, (void *)&raddr, &raddrlen);      //接收数据
        inet_ntop(AF_INET, &raddr.sin_addr.s_addr, sendip, sizeof(sendip));
        printf("Received from %s:%d\n", sendip, ntohs(raddr.sin_port));
        printf("NAME: %s\n", pbuffer_recv->name);
        printf("MATH: %u\n", ntohl(pbuffer_recv->math));
        printf("ENGL: %u\n", ntohl(pbuffer_recv->english));
        printf("CHIN: %u\n", ntohl(pbuffer_recv->chinese));
    }

    close(sd);         //关闭socket

    exit(0);
}