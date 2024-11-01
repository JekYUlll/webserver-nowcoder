#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // 1. 创建socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(-1);
    }

    // 2. 绑定
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    /* inet_pton(AF_INET, "172.16.183.45", &saddr.sin_addr.s_addr); */
    saddr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    saddr.sin_port = htons(8080);
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 3. 监听
    ret = listen(lfd, 8);
    if (ret == -1) {
        perror("listen");
        exit(-1);
    }

    // 4. 接受客户端连接
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    // 在 accept 处阻塞
    int cfd = accept(lfd, (struct sockaddr *)&clientaddr, &len);
    if (cfd == -1) {
        perror("accept");
        exit(-1);
    }
    // 输出客户端信息
    char clientIP[16];
    inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(clientaddr.sin_port);
    printf("client ip is %s, port is %d\n", clientIP, clientPort);

    // 5. 通信
    // 获取客户端数据
    char recvBuf[1024] = {0};
    while (1) {
        memset(recvBuf, 0, sizeof(recvBuf));
        int dataLen = read(cfd, recvBuf, sizeof(recvBuf));
        if (dataLen == -1) {
            perror("read");
            exit(-1);
        } else if (dataLen > 0) {
            printf("recv client data : %s\n", recvBuf);
            // 回射 echo
            char echoData[1048] = "[echo] ";
            strcat(echoData, recvBuf);
            // 如果用sizeof(echoData)会出问题
            write(cfd, echoData, strlen(echoData));
        } else if (dataLen == 0) {
            // 表示客户端断开连接
            printf("client closed...");
            break;
        }

        // 给客户端发送数据
        /* char *data = "hello, I am server"; */
        /* write(cfd, data, strlen(data)); */
    }

    // 关闭文件描述符
    close(cfd);
    close(lfd);

    return EXIT_SUCCESS;
}
