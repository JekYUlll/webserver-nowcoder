#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int ret = access("fifo1", F_OK);
    if (ret == -1) {
        printf("管道不存在，创建对应有名管道\n");
        ret = mkfifo("fifo1", 0664);
        if (ret == -1) {
            perror("mkfifo");
            exit(0);
        }
    }

    ret = access("fifo2", F_OK);
    if (ret == -1) {
        printf("管道不存在，创建对应有名管道\n");
        ret = mkfifo("fifo2", 0664);
        if (ret == -1) {
            perror("mkfifo");
            exit(0);
        }
    }

    // 2. 以只写的方式打开管道 fifo1
    int fdw = open("fifo1", O_WRONLY);
    if (fdw == -1) {
        
        perror("open fifo1");
        exit(0);
    }
    printf("打开管道fifo1成功，等待写入...\n");

    // 3. 以只读的方式打开管道 fifo2
    int fdr = open("fifo2", O_RDONLY);
    if (fdr == -1) {
        perror("open fifo2");
        exit(0);
    }
    printf("打开管道fifo2成功，等待读取...\n");

    char buf[128];
    // 4. 循环写读数据
    while (1) {
        memset(buf, 0, 128);
        // 获取标准输入的数据
        fgets(buf, 128, stdin);
        // 写数据
        ret = write(fdw, buf, strlen(buf));
        if (ret == -1) {
            perror("write");
            exit(0);
        }
        // 5. 读管道数据
        memset(buf, 0, 128);
        ret = read(fdr, buf, 128);
        if (ret <= 0) {
            perror("read");
            break;
        }
        printf("buf: %s\n", buf);
    }

    // 6. 关闭文件描述符
    close(fdr);
    close(fdw);

    return EXIT_SUCCESS;
}
