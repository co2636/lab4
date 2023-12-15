#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_CLIENTS 5

pthread_mutex_t mutex;
pthread_cond_t cond;
int message = 0;

void* client_thread(void* arg) {
    int client_id = *(int*)arg;

    // 클라이언트 쓰레드가 메시지를 보낼 때까지 대기
    pthread_mutex_lock(&mutex);
    while (message != client_id) {
        pthread_cond_wait(&cond, &mutex);
    }
    printf("클라이언트 %d: 메시지 수신.\n", client_id);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void* server_thread(void* arg) {
    for (int i = 1; i <= MAX_CLIENTS; ++i) {
        // 클라이언트에게 메시지를 보냄
        pthread_mutex_lock(&mutex);
        message = i;
        printf("서버: 메시지 %d 발송.\n", i);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);

        // 임시 딜레이
        usleep(500000);
    }

    return NULL;
}

int main() {
    pthread_t server, clients[MAX_CLIENTS];
    int client_ids[MAX_CLIENTS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // 서버 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);

    // 클라이언트 쓰레드들 생성
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_ids[i] = i + 1;
        pthread_create(&clients[i], NULL, client_thread, (void*)&client_ids[i]);
    }

    // 서버 쓰레드 및 클라이언트 쓰레드들 종료 대기
    pthread_join(server, NULL);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        pthread_join(clients[i], NULL);
    }

    // 뮤텍스 및 조건 변수 해제
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}

