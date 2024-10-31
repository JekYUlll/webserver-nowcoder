// 通过mmap进行文件拷贝
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // 1. 对原始文件进行内存映射
    int fd = open("english.txt", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(0);
    }
    // 获取原始文件大小
    int len = lseek(fd, 0, SEEK_END);

    // 2. 创建新文件
    int fd_cpy = open("cpy.txt", O_RDWR | O_CREAT, 0664);
    if (fd_cpy == -1) {
        perror("open");
        exit(0);
    }

    // 拓展新文件
    truncate("cpy.txt", len);
    write(fd_cpy, " ", 1);

    // 3. 分别做内存映射
    void *ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    void *ptr_cpy = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd_cpy, 0);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }
    if (ptr_cpy == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }

    // 内存拷贝
    memcpy(ptr_cpy, ptr, len);

    // 释放资源
    munmap(ptr_cpy, len);
    munmap(ptr, len);
    close(fd_cpy);
    close(fd);

    return EXIT_SUCCESS;
}
