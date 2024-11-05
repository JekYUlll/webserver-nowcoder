#include <stdio.h>

int main() { // 牛客教的判断方法
    union {
        short value;
        char bytes[sizeof(short)];
    } test;
    test.value = 0x0102;
    if ((test.bytes[0] == 1) && (test.bytes[1] == 2)) {
        printf("Big-Endian\n");
    } else if ((test.bytes[0] == 2) && (test.bytes[1] == 1)) {
        printf("Little-Endian\n");
    } else {
        printf("Unknown\n");
    }
}
