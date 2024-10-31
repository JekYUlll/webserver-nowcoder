#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    // 1. 判断文件是否存在
    int ret = access("test", F_OK);
    if (ret == -1) {
        printf("管道不存在，创建管道\n");
        // 2. 创建管道文件
        ret = mkfifo("test", 0664);
        if (ret == -1) {
            perror("mkfifo");
            exit(0);
        }
    }

    // 3. 以只写的方式打开管道
    int fd = open("test", O_WRONLY);
    if(fd == -1) {
        perror("open");
        exit(0);
    }

    // 4. 写数据
    for(int i = 0; i < 1000; i++) {
        char buf[1024];
        sprintf(buf, "hello, %d\n", i);
        printf("write data : %s\n", buf);
        write(fd, buf, strlen(buf));
        sleep(1);
    }

    close(fd);
    
    return EXIT_SUCCESS;
}
