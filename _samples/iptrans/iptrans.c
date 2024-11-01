#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {

    char buf[] = "192.168.1.4";
    unsigned int num = 0;
    inet_pton(AF_INET, buf, &num);
    unsigned char *p = (unsigned char *)&num;
    printf("%d %d %d %d\n", *p, *(p + 1), *(p + 2), *(p + 3));
    printf("%d\n", num);

    char ip[16] = "";
    const char *str = inet_ntop(AF_INET, &num, ip, sizeof(ip));
    printf("ip  : %s\n", ip);
    printf("str : %s\n", str);
    printf("%d\n", ip == str);

    return EXIT_SUCCESS;
}
