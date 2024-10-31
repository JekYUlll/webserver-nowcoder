#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct sockInfo {
    int fd;
    pthread_t tid;
    struct sockaddr_in addr;
};

// 算是一种池化技术？不然在循环里创建新的sockInfo并且传递给working函数，需要在堆上开辟内存，还需要手动释放
struct sockInfo sockinfos[128];

void *working(void *arg) {

    struct sockInfo *pinfo = (struct sockInfo *)arg;
    char clientIP[16];
    inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, clientIP,
              sizeof(clientIP));
    unsigned short clientPort = ntohs(pinfo->addr.sin_port);

    printf("client IP : %s, prot : %d\n", clientIP, clientPort);

    // 接收数据
    char recvBuf[1024] = {0};
    while (1) {
        int len = read(pinfo->fd, &recvBuf, sizeof(recvBuf));
        if (len == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        } else if (len > 0) {
            printf("[recv client data] : [IP : %s:%d] %s\n", clientIP,
                   clientPort, recvBuf);
            // 回射 echo
            char echoData[1048] = "[echo] ";
            strcat(echoData, recvBuf);
            // 如果用sizeof(echoData)会出问题
            write(pinfo->fd, echoData, strlen(echoData) + 1);

        } else {
            printf("[IP : %s:%d]client closed...\n", clientIP, clientPort);
            break;
        }
        /* write(cfd, recvBuf, strlen(recvBuf)); */
    }
    close(pinfo->fd);
    return NULL;
}

int main(void) {

    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 设置端口复用
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    // 绑定
    int ret = bind(lfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 监听
    ret = listen(lfd, 128);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // 初始化数据
    int max = sizeof(sockinfos) / sizeof(sockinfos[0]);
    for (int i = 0; i < max; i++) {
        bzero(&sockinfos[i], sizeof(sockinfos[i]));
        sockinfos[i].fd = -1;
        sockinfos[i].tid = -1;
    }

    while (1) {

        struct sockaddr_in clientAddr;
        socklen_t len = sizeof(clientAddr);
        int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
        if (cfd == -1) {
            // 系统调用导致的中断
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            exit(EXIT_FAILURE);
        }
        struct sockInfo *pinfo;
        for (int i = 0; i < max; i++) {
            // 从数组中找到可用的sockinfo
            if (sockinfos[i].fd == -1) {
                pinfo = &sockinfos[i];
                break;
            }
            if (i == max - 1) {
                sleep(1);
                i--;
            }
        }
        pinfo->fd = cfd;
        // 是结构体，没法直接=
        memcpy(&pinfo->addr, &clientAddr, len);
        // 子线程 此处pinfo作为参数传入到子线程的函数
        pthread_create(&pinfo->tid, NULL, working, pinfo);
        // 分离线程
        pthread_detach(pinfo->tid);
    }
    close(lfd);

    return EXIT_SUCCESS;
}
