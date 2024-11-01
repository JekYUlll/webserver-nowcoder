#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // 创建 socket.  SOCK_DGRAM 表示数据报，0 表示UDP
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // ?设置端口复用
    int optval = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;
    // 绑定
    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 通信
    while (1) {
        char recvBuf[128];
        char ipbuf[16];
        struct sockaddr_in clientAddr;
        socklen_t len = sizeof(clientAddr);

        // 接收数据
        int num = recvfrom(fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&clientAddr, &len);
        if(num == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("client IP : %s, Port : %d\n", 
            inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, ipbuf, sizeof(ipbuf)),
            ntohs(clientAddr.sin_port));

        printf("client say : %s", recvBuf);

        // 发送数据
        sendto(fd, recvBuf, strlen(recvBuf) + 1, 0, (struct sockaddr*)&clientAddr, len);


    }

    close(fd);

    return EXIT_SUCCESS;
}
