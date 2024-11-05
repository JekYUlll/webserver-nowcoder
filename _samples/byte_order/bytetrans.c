#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // htons 转换端口
    unsigned short a = 0x0102;
    printf("a : %x\n", a);
    unsigned short b = htons(a);
    printf("b : %x\n", b);
    printf("-------------------------------\n");

    // htonl 转换IP
    char buf[4] = {192, 168, 1, 100};
    int num = *(int *)buf;
    int res = htonl(num);
    unsigned char *p = (unsigned char *)&res;
    printf("%d %d %d %d \n", *p, *(p + 1), *(p + 2), *(p + 3));
    printf("-------------------------------\n");

    // ntohs

    // ntohl

    return EXIT_SUCCESS;
}
