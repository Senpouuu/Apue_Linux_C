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


     
    struct sockaddr_in raddr;
    memset(&raddr, 0, sizeof(raddr));
    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(80);
    inet_pton(AF_INET, argv[1], &raddr.sin_addr.s_addr);

    if(connect(sfd,(void*) &raddr, sizeof(raddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    char buffer[1024];

    const char *request = "GET /test.png HTTP/1.1\nHost: 127.0.0.1\r\n\r\n";
    write(sfd, request, strlen(request));

    char rbuf[1024];
    while(1)
    {
        int len = read(sfd, rbuf, 1024);
        write(fileno(stdout), rbuf, len);
        if(len <= 0)
            break;
    }

    exit(0);
}