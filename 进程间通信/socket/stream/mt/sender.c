#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "proto.h"


int main(int argc, char *argv[])
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);


    if(argc < 2)
    {
        fprintf(stderr,"Usage: %s <server ip>\n", argv[0]);
        exit(1);
    }


    if(sfd < 0)
    {
        perror("socket");
        exit(1);
    }

    
    // bind();
    struct sockaddr_in raddr;
    memset(&raddr, 0, sizeof(raddr));
    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(atoi(SERVER_PROT));
    inet_pton(AF_INET, argv[1], &raddr.sin_addr.s_addr);


    if(connect(sfd,(void*) &raddr, sizeof(raddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    long long timestamp;
    char buffer[1024];

    read(sfd, buffer, 1024);
    sscanf(buffer, FMT_STAMP, &timestamp);
    printf("接收到时间戳: %lld\n", timestamp);
    
    
     // 将时间戳转换为结构化时间
    time_t rawtime = (time_t)timestamp;
    struct tm *timeinfo = localtime(&rawtime);

    // 定义格式化的输出字符串缓冲区
    char formatted_time[64];
    strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M:%S", timeinfo);

    printf("可读时间: %s\n", formatted_time);

    return 0;
    // recv();

    exit(0);
}