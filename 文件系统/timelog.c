#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char **argv) {
    struct tm *ntm = NULL;
    char buff[1024];
    int fd = open("./timelog.log", O_WRONLY | O_CREAT | O_APPEND, 0600); // 使用 O_APPEND

    if (fd == -1) {
        perror("打开文件失败");
        exit(EXIT_FAILURE);
    }	

    while (1) {
        time_t ntime = time(NULL);
        memset(buff, 0, sizeof(buff)); // 将 buffer 清空

		if (ntime == (time_t)-1) {
            perror("获取当前时间失败");
            close(fd);
            exit(EXIT_FAILURE);
        }

        ntm = localtime(&ntime);
        if (ntm == NULL) {
            perror("转换时间失败");
            close(fd);
            exit(EXIT_FAILURE);
        }

        // 格式化时间字符串
        if (strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S\r\n", ntm) == 0) {
            perror("格式化时间字符串失败");
            close(fd);
            exit(EXIT_FAILURE);
        }

        // 写入文件
        if (write(fd, buff, strlen(buff)) == -1) {
            perror("写入文件失败");
            close(fd);
            exit(EXIT_FAILURE);
        }

        sleep(1);
    }

    close(fd);
    return 0;
}
