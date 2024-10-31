#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 设置端口复用
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    // 绑定
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 监听
    listen(lfd, 8);

    // 创建 epoll 实例
    int epfd = epoll_create(100);

    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);

    struct epoll_event epevs[1024];

    while (1) {

        int ret = epoll_wait(epfd, epevs, 1024, -1);
        if (ret == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        printf("ret = %d\n", ret);

        for (int i = 0; i < ret; i++) {
            int curfd = epevs[i].data.fd;

            if (curfd == lfd) {
                // 监听的文件描述符有数据到达，有客户端连接
                struct sockaddr_in clientAddr;
                socklen_t len = sizeof(clientAddr);
                int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);

                // 设置cfd属性非阻塞
                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                /* epev.events = EPOLLIN | EPOLLOUT; */
                epev.events = EPOLLIN | EPOLLET; // 设置ET--epoll边沿触发
                epev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
            } else {
                // 此处比较糙，不是正经处理。应该分条件判断
                if (epevs[i].events & EPOLLOUT) {
                    continue;
                }
                // 有数据到达，需要通信
                /* char buf[1024] = {0}; */
                /* int len = read(curfd, buf, sizeof(buf)); */
                /* if (len == -1) { */
                /*     perror("read"); */
                /*     exit(EXIT_FAILURE); */
                /* } else if (len == 0) { */
                /*     printf("client closed...\n"); */
                /*     epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL); */
                /*     close(curfd); */
                /* } else if (len > 0) { */
                /*     printf("read buf = %s\n", buf); */
                /*     write(curfd, buf, strlen(buf) + 1); */
                /* } */
                // 循环读取所有数据
                char buf[5];
                int len = 0;
                // 循环读取所有数据
                while ((len = read(curfd, buf, sizeof(buf))) > 0) {
                    // 读取数据
                    /* printf("recv data : %.*s\n", len, */
                    /*        buf);            // 使用 %.*s 确保只打印读取的长度
                     */
                    write(STDOUT_FILENO, buf, len);
                    write(curfd, buf, len); // 回射数据
                }

                if (len == 0) {
                    printf("client closed...\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                    break; // 退出循环
                } else if (len == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        // 如果是 EAGAIN 或
                        // EWOULDBLOCK，表示没有更多数据可读，退出循环
                        break;
                    } else {
                        perror("read");
                        exit(-1);
                    }
                }
            }
        }
    }
    close(lfd);
    close(epfd);
    return 0;
}
