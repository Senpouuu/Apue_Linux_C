#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE *fp;
    int count = 0; 
    
    while(1)
    {
    	fp = fopen("log","r");
    	if (fp == NULL)
		break;
    	count++;
    }
    printf("我一共打开了：,%d\r\n",count);
    perror("ERROR");
    exit(0);
    return 0;
}
