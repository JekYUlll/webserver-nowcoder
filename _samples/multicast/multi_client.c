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

    struct ip_mreq mreq;
    inet_pton(AF_INET, "239.0.0.10", &mreq.imr_multiaddr.s_addr);
    mreq.imr_interface.s_addr = INADDR_ANY;
    // 加入到多播组
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    
    // 通信
    while (1) {

        char buf[128];
        // 接收数据
        int num = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        printf("server says : %s", buf);
    }

    close(fd);

    return EXIT_SUCCESS;
}
