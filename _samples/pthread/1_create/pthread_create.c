#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void *callback(void *arg) {
    printf("child thread...\n");
    printf("arg : %d\n", *(int *)arg);
    return NULL;
}

int main(void) {

    pthread_t tid;
    int num = 10;
    // 创建子线程
    int ret = pthread_create(&tid, NULL, callback, (void *)&num);
    if (ret != 0) {
        char *errstr = strerror(ret);
        printf("error : %s\n", errstr);
    }
    printf("---------------\n");

    sleep(1);

    return EXIT_SUCCESS;
}
