#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>

#include "proto.h"


int main(int argc, char *argv[])
{
    int sd = socket(AF_INET, SOCK_DGRAM, 0); // 取得socket描述符
    struct msg_st recv_buffer;

    struct sockaddr_in laddr, raddr;

    if (sd < 0)
    {
        perror("socket");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(RCV_PORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr.s_addr); 
    
    struct ip_mreqn mreq;
    inet_pton(AF_INET, MTGOUP, &mreq.imr_multiaddr.s_addr); 
    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address.s_addr); 
    mreq.imr_ifindex = if_nametoindex("lo");

    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt");
        exit(1);
    } 

    if (bind(sd, (void *)&laddr, sizeof(laddr)) < 0) // 给socket取得地址
    {
        perror("bind");
        exit(1);
    }

    /*ATTENTION!*/
    socklen_t raddrlen = sizeof(raddr);
    char sendip[30];
    while (1)
    {
        printf("Waiting for message...\n");
        recvfrom(sd, &recv_buffer, sizeof(recv_buffer), 0, (void *)&raddr, &raddrlen); // 接收数据
        inet_ntop(AF_INET, &raddr.sin_addr.s_addr, sendip, sizeof(sendip));
        printf("Received from %s:%d\n", sendip, ntohs(raddr.sin_port));
        printf("NAME: %s\n", recv_buffer.name);
        printf("MATH: %u\n", ntohl(recv_buffer.math));
        printf("ENGL: %u\n", ntohl(recv_buffer.english));
        printf("CHIN: %u\n", ntohl(recv_buffer.chinese));
    }

    close(sd); // 关闭socket

    exit(0);
}