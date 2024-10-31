#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int num = 1;
pthread_mutex_t mutex;
pthread_rwlock_t rwlock;

void *writeNum(void *arg) {
    while (1) {
        /* pthread_mutex_lock(&mutex); */
        pthread_rwlock_wrlock(&rwlock);
        num++;
        printf("+++write, tid : %ld, num : %d\n", pthread_self(), num);
        /* pthread_mutex_unlock(&mutex); */
        pthread_rwlock_unlock(&rwlock);
        usleep(100);
    }
    return NULL;
}

void *readNum(void *arg) {
    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("===read, tid : %ld, num : %d\n", pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(100);
    }
    return NULL;
}

int main(void) {
    /* pthread_mutex_init(&mutex, NULL); */
    pthread_rwlock_init(&rwlock, NULL);
    pthread_t wtids[3], rtids[3];
    for (int i = 0; i < 3; i++) {
        pthread_create(&wtids[i], NULL, writeNum, NULL);
    }

    for (int i = 0; i < 5; i++) {
        pthread_create(&rtids[i], NULL, readNum, NULL);
    }

    for (int i = 0; i < 3; i++) {
        pthread_detach(wtids[i]);
    }
    for (int i = 0; i < 5; i++) {
        pthread_detach(rtids[i]);
    }

    pthread_exit(NULL);

    /* pthread_mutex_destroy(&mutex); */
    printf(">>> destroy rwlock...");
    pthread_rwlock_destroy(&rwlock); // 实际上永远不会被执行，因为主线程在调用
                                     // pthread_exit 后会立即退出。

    return EXIT_SUCCESS;
}
