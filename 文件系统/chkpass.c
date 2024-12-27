#include <stdio.h>
#include <stdlib.h>
#include <shadow.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    struct spwd *pspwd = NULL;
    char *in_pass = NULL;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        exit(1);
    }

    // 输入密码
    in_pass = getpass("Password:");
    if (in_pass == NULL) {
        perror("getpass");
        exit(1);
    }

    // 获取用户信息
    pspwd = getspnam(argv[1]);
    if (pspwd == NULL) {
        perror("getspnam");
        exit(1);
    }

    // 输出加密密码
    printf("加密密码: %s\n", pspwd->sp_pwdp);

    // 检查密码
    if (strcmp(crypt(in_pass, pspwd->sp_pwdp), pspwd->sp_pwdp) == 0) {
        printf("密码验证成功！\n");
    } else {
        printf("密码验证失败！\n");
    }

    exit(0);
}
