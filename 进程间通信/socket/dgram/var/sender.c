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
    if(argc < 3)
    {
        printf("Usage: %s ip_address name\n", argv[0]);
        exit(1);
    }
    srand(time(NULL));

    if(strlen(argv[2]) > NAMEMAX)
    {
        fprintf(stderr, "name too long\n");
        exit(1);
    }

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct msg_st *pbuffer_send;
    size_t send_size = sizeof(struct msg_st) + strlen(argv[2]);
    pbuffer_send = malloc(send_size);
    if(pbuffer_send == NULL)
    {
        perror("malloc");  
        exit(1);
    }

    strcpy(pbuffer_send->name, argv[2]);
    pbuffer_send->chinese = htonl(rand() % 100);
    pbuffer_send->math = htonl(rand() % 100);
    pbuffer_send->english = htonl(rand() % 100);

    if(sfd < 0)
    {
        perror("socket");
        exit(1);
    }
    
    // bind();
    struct sockaddr_in raddr;
    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(atoi(RCV_PORT));
    
    inet_pton(AF_INET,argv[1], &raddr.sin_addr.s_addr);
    

    char ip_recv[30];
    if(sendto(sfd, pbuffer_send, send_size, 0, (void *)&raddr, sizeof(raddr)) < 0)
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
