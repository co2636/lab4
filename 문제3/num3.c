#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0;  // 현재 버퍼에 있는 아이템의 수

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    int item;
    for (int i = 0; i < 10; ++i) {
        item = rand() % 100;  // 생산할 아이템 생성

        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            // 버퍼가 가득 찼으면 기다림
            pthread_cond_wait(&empty, &mutex);
        }

        // 버퍼에 아이템 추가
        buffer[count] = item;
        ++count;

        printf("Produced: %d, Count: %d\n", item, count);

        // 소비자에게 알림
        pthread_cond_signal(&full);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int item;
    for (int i = 0; i < 10; ++i) {
        pthread_mutex_lock(&mutex);
        while (count == 0) {
            // 버퍼가 비었으면 기다림
            pthread_cond_wait(&full, &mutex);
        }

        // 버퍼에서 아이템 소비
        item = buffer[count - 1];
        --count;

        printf("Consumed: %d, Count: %d\n", item, count);

        // 생산자에게 알림
        pthread_cond_signal(&empty);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t producer_thread, consumer_thread;

    // 생산자와 소비자 쓰레드 생성
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // 쓰레드 종료 대기
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // 뮤텍스와 조건 변수 해제
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    return 0;
}

