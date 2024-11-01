#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void *callback(void *arg) {
    printf("child thread ID: %ld\n", pthread_self());
    return NULL;
}

int main(int argc, char *argv[]) {
    // 创建子线程
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, callback, NULL);
    if (ret != 0) {
        char *errstr = strerror(ret);
        printf("error1: %s\n", errstr);
    }
    // 输出线程ID
    printf("tid: %ld, main thread ID: %ld\n", tid, pthread_self());
    // 子线程分离，分离后子线程资源会自己释放
    ret = pthread_detach(tid);
    if (ret != 0) {
        char *errstr = strerror(ret);
        printf("error2: %s\n", errstr);
    }
    // 分离之后不能再用join连接

    pthread_exit(NULL); // 退出主线程

    return EXIT_SUCCESS;
}
