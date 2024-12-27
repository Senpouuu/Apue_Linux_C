#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char **argv) {
    struct tm ntm;
    char buff[1024];
    char fmtchr[1024];
	time_t ntime = time(NULL);
	int c;	
	int fd;
	
	fmtchr[0] = '\0';	

    if (ntime == (time_t)-1) {
        perror("获取当前时间失败");
        exit(EXIT_FAILURE);
    }

    ntime += 24 * 60 * 60 * 60;

    if (localtime_r(&ntime, &ntm) == NULL) {
        perror("转换时间失败");
        exit(EXIT_FAILURE);
    }
	

	while(1)
	{
		c = getopt(argc,argv,"-H:MSymd");
		if(c < 0)
			break;

		switch(c)
		{
			case 1:
				fd = open(argv[optind - 1],O_CREAT|O_WRONLY|O_APPEND,0600);
				if(fd < 0)
				{
					perror("open");
					exit(1);
				}
				break;
			
			case 'H': 
				if(strcmp(optarg,"12") == 0)
					strncat(fmtchr,"%I(%P)-",sizeof(fmtchr));

				else if(strcmp(optarg,"24") == 0)
					strncat(fmtchr,"%H-",sizeof(fmtchr));

			break;

			case 'M': strncat(fmtchr,"%M-",sizeof(fmtchr));
			break;

			case 'S': strncat(fmtchr,"%S-",sizeof(fmtchr));
			break;

			case 'y': strncat(fmtchr,"%y-",sizeof(fmtchr));
			break;

			case 'm': strncat(fmtchr,"%m-",sizeof(fmtchr));
			break;

			case 'd': strncat(fmtchr,"%d-",sizeof(fmtchr));
			break;

			default: break;
		}
	}

	fmtchr[strlen(fmtchr) - 1] = '\0';	
	puts(fmtchr);
	
    strftime(buff, sizeof(buff),fmtchr, &ntm);
	
	ssize_t result = write(fd, buff, strlen(buff));

	if (result == -1) 
    	// 写入失败
    	perror("write failed");
	
	close(fd);
    printf("After 24 Hours is: %s\r\n", buff);

    exit(0);
}
