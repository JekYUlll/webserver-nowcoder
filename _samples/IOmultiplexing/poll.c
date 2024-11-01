#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define FDS_COUNT 1024

int main(int argc, char *argv[]) {

    int lfd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 设置端口复用
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 监听
    listen(lfd, 8);

    // 初始化检测的文件描述符数组
    struct pollfd fds[FDS_COUNT];
    for (int i = 0; i < FDS_COUNT; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }

    fds[0].fd = lfd;

    // 最大下标
    int nfds = 0;

    while (1) {

        int ret = poll(fds, nfds + 1, -1);
        if (ret == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        } else if (ret == 0) {
            continue;
        } else if (ret > 0) {

            if (fds[0].revents & POLLIN) {
                // 有新的客户端连接进来
                struct sockaddr_in clientAddr;
                socklen_t len = sizeof(clientAddr);
                int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
                // 将新的文件描述符加入到集合中
                for (int i = 1; i < FDS_COUNT; i++) {
                    if (fds[i].fd == -1) {
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                        // 自己写的判断
                        nfds = nfds > i ? nfds : i;
                        break;
                    }
                }
                // 更新最大的文件描述符的索引
                // --教程里此处有问题，用文件�述符的值和数组索引混在一起比较
                nfds = nfds > cfd ? nfds : cfd;
            }

            for (int i = 1; i <= nfds; i++) {
                if (fds[i].revents & POLLIN) {
                    // 说明对应客户端发来了数据
                    char buf[1024] = {0};
                    int len = read(fds[i].fd, buf, sizeof(buf));
                    if (len == -1) {
                        perror("read");
                        exit(EXIT_FAILURE);
                    } else if (len == 0) {
                        printf("client closed...\n");
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        // 此时nfds也应该变化
                    } else if (len > 0) {
                        printf("read buf = %s\n", buf);
                        write(fds[i].fd, buf, strlen(buf) + 1);
                    }
                }
            }
        }
    }
    close(lfd);

    return EXIT_SUCCESS;
}
