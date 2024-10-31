#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // 1. 创建socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(-1);
    }

    // 2. 连接服务器
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr)); // 清零结构
    serveraddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr) <= 0) {
        perror("inet_pton");
        exit(-1);
    }
    serveraddr.sin_port = htons(9999);
    int ret = connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (ret == -1) {
        perror("connect");
        exit(-1);
    }

    // 3. 通信
    char *data = "hello, I am client";
    char recvBuf[1024] = {0};
    char inputBuf[1024] = {0};
    while (1) {
        memset(inputBuf, 0, sizeof(inputBuf));
        // 读取输入
        printf(">>> ");
        fgets(inputBuf, sizeof(inputBuf), stdin);
        // 给服务端发送数据
        write(fd, inputBuf, strlen(inputBuf) + 1); // + 1 是加上字符串结束

        // 读取服务端数据
        memset(recvBuf, 0, sizeof(recvBuf));
        int len = read(fd, recvBuf, sizeof(recvBuf));
        if (len == -1) {
            perror("read");
            exit(-1);
        } else if (len > 0) {
            printf("[recv server data] : %s\n", recvBuf);
        } else if (len == 0) {
            printf("server closed...");
            break;
        }
    }

    // 关闭连接
    close(fd);

    return EXIT_SUCCESS;
}
