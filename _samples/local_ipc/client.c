#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main() {

    unlink("client.sock");

    int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, "client.sock");
    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 连接服务器
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_LOCAL;
    strcpy(serverAddr.sun_path, "server.sock");
    ret = connect(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (ret == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    int num = 0;
    while (1) {
        // 发送
        char buf[128];
        sprintf(buf, "hello, i am client %d\n", num++);
        send(fd, buf, strlen(buf) + 1, 0);
        printf("client say : %s\n", buf);

        // 接收
        int len = recv(fd, buf, sizeof(buf), 0);
        if(len == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        } else if (len == 0) {
            printf("server closed...\n");
            break;
        } else if (len > 0) {
            printf("server say : %s\n", buf);
        }

        sleep(1);
    }

    close(fd);

    return EXIT_SUCCESS;
}