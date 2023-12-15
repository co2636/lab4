#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 3

void *hello_thread(void *arg) {
    // arg를 정수로 캐스팅하여 출력
    printf("Thread %ld: Hello, World!\n", (long)arg);
    return arg;
}

int main() {
    pthread_t tid[NUM_THREADS];
    int i, status;

    for (i = 0; i < NUM_THREADS; i++) {
        // long으로 형변환
        status = pthread_create(&tid[i], NULL, hello_thread, (void *)(long)i);
        if (status != 0) {
            fprintf(stderr, "Create thread %d: %d", i, status);
            exit(1);
        }
    }

    pthread_exit(NULL);
}

