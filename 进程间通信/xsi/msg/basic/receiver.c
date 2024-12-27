#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "proto.h"

int main()
{   

    struct msg_st rec_msgst;

    key_t key = ftok(KEY_PATH,KEY_PORJ);
    if(key < 0)
    {
        perror("ftok");
        exit(1);
    }

    //创建消息队列
    int msgid = msgget(key,IPC_CREAT|0600);
    if(msgid < 0) 
    {
        perror("msgget");
        exit(1);
    }   
 
       
    while(1)
    {
        if (msgrcv(msgid, &rec_msgst, sizeof(rec_msgst) - sizeof(long), 0, 0) < 0)
        {
            perror("msgrcv");
            exit(1);
        }
        printf("Name: %s\n", rec_msgst.name);
        printf("Math: %d\n", rec_msgst.math);
        printf("Chinese: %d\n", rec_msgst.chinese);
        
    }
     
    int ret = msgctl(msgid, IPC_RMID, NULL); 
    if(ret < 0)
    {
        perror("msgctl");
        exit(1);
    }
    exit(0);
}