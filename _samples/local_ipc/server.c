#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main() {

    unlink("server.sock");

    int lfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(lfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, "server.sock");
    int ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    ret = listen(lfd, 100);
    if(ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un clientAddr;
    socklen_t len = sizeof(clientAddr);
    int cfd = accept(lfd, (struct sockaddr*)&clientAddr, &len);
    if(cfd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("client socket file name: %s\n", clientAddr.sun_path);

    while (1) {
        char buf[128];
        int len = recv(cfd, buf, sizeof(buf), 0);
        if(len == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        } else if (len == 0) {
            printf("client closed...\n");
            break;
        } else if (len > 0) {
            printf("client say : %s\n", buf);
            send(cfd, buf, len, 0);
        }
    }

    close(cfd);
    close(lfd);

    return EXIT_SUCCESS;
}