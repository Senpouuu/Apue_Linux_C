#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#include "proto.h"

int main()
{
    struct msg_st snd_msgst;
    snd_msgst.chinese = 120;
    snd_msgst.math = 80;
    snd_msgst.mtype = 1;
    memcpy(snd_msgst.name,"gulu",4);


    key_t msgkey = ftok(KEY_PATH,KEY_PORJ);
    if(msgkey == -1)
    {
        perror("ftok");
        exit(1);
    }

    int msggid = msgget(msgkey,0);
    while (1)
    {
        sleep(5);
        if (msgsnd(msggid, &snd_msgst, sizeof(snd_msgst) - sizeof(long), 0) < 0)
        {
            perror("msgsnd");
            exit(1);
        }
        puts("OK!\n");
    }

    exit(0);
}