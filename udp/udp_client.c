#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // 创建 socket.  SOCK_DGRAM 表示数据报，0 表示UDP
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 服务器的地址信息
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;

    int num = 0;

    // 通信
    while (1) {
        // 发送数据
        char sendBuf[128];
        sprintf(sendBuf, "hello, this is client %d\n", num++);
        sendto(fd, sendBuf, strlen(sendBuf) + 1, 0,
               (struct sockaddr *)&addr, sizeof(addr)); 

        // 接收数据
        int num = recvfrom(fd, sendBuf, sizeof(sendBuf), 0,
                           NULL, NULL); // 不用指定地址
        if (num == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("server say : %s\n", sendBuf);

        sleep(1);
    }

    close(fd);

    return EXIT_SUCCESS;
}
