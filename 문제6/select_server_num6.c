#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

int main() {
    int server_socket, new_socket, client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    fd_set readfds;
    char buffer[MAX_BUFFER_SIZE];

    // 초기화
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_sockets[i] = 0;
    }

    // 서버 소켓 생성
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 소켓 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 서버 소켓 바인딩
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("소켓 바인딩 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 소켓 리스닝
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("소켓 리스닝 실패");
        exit(EXIT_FAILURE);
    }

    printf("포트 %d에서 서버 대기 중...\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

        int max_socket = server_socket;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int socket_fd = client_sockets[i];

            if (socket_fd > 0) {
                FD_SET(socket_fd, &readfds);
            }

            if (socket_fd > max_socket) {
                max_socket = socket_fd;
            }
        }

        // 클라이언트 소켓들 감지
        int activity = select(max_socket + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            perror("select 오류");
        }

        // 새로운 연결 수락
        if (FD_ISSET(server_socket, &readfds)) {
            if ((new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len)) == -1) {
                perror("연결 수락 실패");
                continue;
            }

            printf("클라이언트가 %s:%d에서 연결되었습니다.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // 새로운 클라이언트를 배열에 추가
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // 클라이언트로부터 메시지 수신 및 방송
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int socket_fd = client_sockets[i];

            if (FD_ISSET(socket_fd, &readfds)) {
                memset(buffer, 0, sizeof(buffer));
                int read_size = recv(socket_fd, buffer, sizeof(buffer), 0);

                if (read_size == 0) {
                    // 클라이언트 연결 종료
                    printf("클라이언트가 연결을 종료했습니다.\n");
                    close(socket_fd);
                    client_sockets[i] = 0;
                } else {
                    // 받은 메시지를 모든 클라이언트에게 방송
                    for (int j = 0; j < MAX_CLIENTS; ++j) {
                        int dest_socket = client_sockets[j];
                        if (dest_socket != 0 && dest_socket != socket_fd) {
                            send(dest_socket, buffer, read_size, 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}

