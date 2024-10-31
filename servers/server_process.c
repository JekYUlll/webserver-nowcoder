#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

void recycleChild(int arg) {
    while (1) {
        int ret = waitpid(-1, NULL, WNOHANG);
        if (ret == -1) {
            // 所有子进程都回收了
            break;
        } else if (ret == 0) {
            // 还有子进程活着
            break;
        } else if (ret > 0) {
            printf("子进程 %d 被回收了\n", ret);
        }
    }
}

int main(void) {

    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = recycleChild;
    // 注册信号捕捉
    sigaction(SIGCHLD, &act, NULL);

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

    while (1) {

        struct sockaddr_in clientAddr;
        socklen_t len = sizeof(clientAddr);
        int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
        if (cfd == -1) {
            // 用信号回收子进程，系统调用导致的中断
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // 每个连接进来，创建一个子进程进行通信
        pid_t pid = fork();
        if (pid == 0) {
            char clientIP[16];
            inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP,
                      sizeof(clientIP));
            unsigned short clientPort = ntohs(clientAddr.sin_port);

            printf("client IP : %s, prot : %d\n", clientIP, clientPort);

            // 接收数据
            char recvBuf[1024] = {0};
            while (1) {
                int len = read(cfd, &recvBuf, sizeof(recvBuf));
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
                    write(cfd, echoData, strlen(echoData) + 1);

                } else {
                    printf("[IP : %s:%d]client closed...\n", clientIP,
                           clientPort);
                    break;
                }
                /* write(cfd, recvBuf, strlen(recvBuf)); */
            }
            close(cfd);
            exit(EXIT_SUCCESS);
        }
    }
    close(lfd);

    return EXIT_SUCCESS;
}
