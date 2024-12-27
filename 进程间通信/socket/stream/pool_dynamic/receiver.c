#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>

#include "proto.h"

/* 在此模型中一个
 * 进程服务一个客户端
 */
#define MIN_PROC 5                   //最小进程数
#define MAX_PROC 10                  //最大进程数
#define MAX_CLIENT 20                //最大客户端数  
#define SIG_NOTIFY SIGUSR2
#define BUFFER_SIZE 1024

enum
{
    STAT_IDLE = 0,
    STAT_BUSY
};

struct pid_info
{
    int status;
    pid_t pid;
};

static struct pid_info *server_st;
static int idle_num = 0, busy_num = 0;
static int sfd;  

static void usr2_handler(int s)
{
    // printf("SIG_NOTIFY received\n");
}

void server_job(int inx)
{
    struct sockaddr_in raddr;
    memset(&raddr, 0, sizeof(raddr));
    socklen_t rlen = sizeof(raddr);

    pid_t ppid = getppid();

    int rfd;
    char ipbuf[28];
    char sendbuf[BUFFER_SIZE];

    while(1)
    {
        server_st[inx].status = STAT_IDLE;
        kill(ppid, SIG_NOTIFY);
        rfd = accept(sfd,(void*)&raddr,&rlen);
        if(rfd < 0)
        {
            if(errno == EINTR || errno == EAGAIN)
                continue;
            else
            {
                perror("accept");
                exit(1);
            }
        }
        inet_ntop(AF_INET, &raddr.sin_addr, ipbuf, sizeof(ipbuf));

        server_st[inx].status = STAT_BUSY; 
        kill(ppid, SIG_NOTIFY);
        
        printf("PID[%d]server %d:%s:%d\n",getpid(), inx, ipbuf,ntohs(raddr.sin_port));
        
        long long stamp = time(NULL);
        int len = sprintf(sendbuf,FMT_STAMP,stamp);
        if(send(rfd, sendbuf, len, 0) < 0)
        {
            perror("send");
            exit(1);
        }
        sleep(3);
        close(rfd);
    }


}

static int del_1_server()
{
    int inx;
    
    if(idle_num == 0)
        return -1;
    
    // 找到空位置
    for(inx = 0; inx < MAX_CLIENT; inx++)
        if(server_st[inx].pid != -1 && server_st[inx].status == STAT_IDLE)
        {
            kill(server_st[inx].pid, SIGTERM);
            break;
        }
    
    server_st[inx].pid = -1;
    server_st[inx].status = STAT_IDLE;
    idle_num--;
    return 0;
}


static int add_1_server()
{
    int inx;
    
    if(idle_num + busy_num >= MAX_CLIENT)
        return -1;

    // 找到空位置
    for(inx = 0; inx < MAX_CLIENT; inx++)
        if(server_st[inx].pid == -1)
            break; 
    
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork");
        return -1;
    }

    if(pid == 0) //child process
    {
        server_job(inx);
    }
    else //parent process
    {
        server_st[inx].status = STAT_IDLE;
        server_st[inx].pid = pid;
        idle_num++;
        return 0;
    }
}

void scan_pool()
{
    int idle = 0,busy = 0;
    for(int i = 0; i < MAX_CLIENT; i++)
    {
        if(server_st[i].pid == -1)
            continue;
        if(kill(server_st[i].pid, 0))
        {
            // 进程不存在
            server_st[i].pid = -1;
            continue;
        }

        if(server_st[i].status == STAT_IDLE)
            idle++;
        else if(server_st[i].status == STAT_BUSY)
            busy++;
        else
        {
            fprintf(stderr, "unknown status\n");
            abort();
        } 
    }
    idle_num = idle;
    busy_num = busy;
}

int main(int argc, char *argv[])
{
    /*重定义信号SIGCHLD, 忽略子进程结束信号, 不做任何处理*/
    struct sigaction sa, sa_old;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &sa, &sa_old);

    /*自定义信号, 处理SIG_NOTIFY*/
    sa.sa_handler = usr2_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIG_NOTIFY, &sa, &sa_old);

    /*
     * set 只有 SIG_NOTIFY
     * 而set_old 包括 SIGINT, SIGTERM, SIGCHLD....
     */

    // 获取当前的信号集,用于下面的sigsuspend屏蔽，也就是说只接收SIG_NOTIFY信号
    sigset_t set, set_old;
    sigemptyset(&set);
    sigaddset(&set, SIG_NOTIFY);
    sigprocmask(SIG_UNBLOCK, &set, &set_old);

    server_st = mmap(NULL, sizeof(struct pid_info) * MAX_CLIENT, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (server_st == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    for(int i = 0; i < MAX_CLIENT; i++)
        server_st[i].pid = -1;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)
    {
        perror("socket");
        exit(1);
    }

    int var = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &var, sizeof(var)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in laddr;
    memset(&laddr, 0, sizeof(laddr));
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVER_PROT));
    laddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sfd, (void*)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    if (listen(sfd, 200) < 0)
    {
        perror("listen");
        exit(1);
    }

    // 初始化服务器
    for (int i = 0; i < MIN_PROC; i++)
    {
        if(add_1_server() < 0)
        {
            perror("add_1_server");
            exit(1);
        }
    }

    while (1)
    {
        sigsuspend(&set_old);
        // printf("SIG_NOTIFY received\n");

        scan_pool();

        // ctl pool

        /*空闲服务器过多，杀掉一些*/
        if (idle_num > MAX_PROC)
        {
            for (int i = 0; i < (idle_num - MAX_PROC); i++)
            {
                if (del_1_server() < 0)
                {
                    perror("del_1_server");
                    exit(1);
                }
            }
        }

        /*空闲服务器太少，添加一些*/
        else if (idle_num < MIN_PROC)
        {
            for (int i = 0; i < (MIN_PROC - idle_num); i++)
            {
                if (add_1_server() < 0)
                {
                    perror("add_1_server");
                    exit(1);
                }
            }
        }

        /*打印*/
        for(int i = 0; i < MAX_CLIENT; i++)
        {
            if(server_st[i].pid == -1)
                putchar('-');
                else if(server_st[i].status == STAT_IDLE)
                    putchar('.');
                    else
                        putchar('x');
        }
        puts("\r\n");
        sigprocmask(SIG_SETMASK, &set_old, NULL); // 恢复原信号

    }
    exit(0);
}