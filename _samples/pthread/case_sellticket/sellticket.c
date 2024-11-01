#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 现存门票数量
int tickets_count = 1000;

pthread_mutex_t mutex;

void *sellticket(void *arg) {
    // 卖票
    while (1) {
        // 加锁
        pthread_mutex_lock(&mutex);
        if (tickets_count > 0) {
            // usleep(5000); // 睡眠5000微秒
            printf("%ld 正在卖第 %d 张门票\n", pthread_self(), tickets_count);
            tickets_count--;
        } else {
            // 解锁
            pthread_mutex_unlock(&mutex);
            break;
        }
        // 解锁
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // 初始化互斥量
    pthread_mutex_init(&mutex, NULL);

    // 创建三个子线程
    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, sellticket, NULL);
    pthread_create(&tid2, NULL, sellticket, NULL);
    pthread_create(&tid3, NULL, sellticket, NULL);

    // 回收子线程的资源，阻塞
    /* pthread_join(tid1, NULL); */
    /* pthread_join(tid2, NULL); */
    /* pthread_join(tid3, NULL); */

    // 设置线程分离
    pthread_detach(tid1);
    pthread_detach(tid2);
    pthread_detach(tid3);

    // 退出主线程
    pthread_exit(NULL);

    // 释放互斥量资源
    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}
