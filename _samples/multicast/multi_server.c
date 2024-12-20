#include <arpa/inet.h>
#include <netinet/in.h>
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

    // 设置多播属性，设置外出接口
    struct in_addr imr_multiaddr;
    // 初始化多播地址
    inet_pton(AF_INET, "239.0.0.10", &imr_multiaddr.s_addr);
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &imr_multiaddr, sizeof(imr_multiaddr));

    // 初始化客户端的地址信息
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    inet_pton(AF_INET, "239.0.0.10", &addr.sin_addr.s_addr);

    int num = 0;
    // 通信
    while (1) {
        char sendBuf[128];
        sprintf(sendBuf, "hello, client... %d", num++);
        // 发送数据
        sendto(fd, sendBuf, strlen(sendBuf) + 1, 0, (struct sockaddr *)&addr,
               sizeof(addr));
        printf("多播的数据: %s\n", sendBuf);
        sleep(1);
    }

    close(fd);

    return EXIT_SUCCESS;
}
