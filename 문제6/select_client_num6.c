#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER_SIZE];

    // 클라이언트 소켓 생성
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 소켓 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("서버 연결 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버에 연결되었습니다. 채팅을 시작하세요.\n");

    while (1) {
        // 사용자로부터 메시지 입력
        printf("나: ");
        fgets(buffer, sizeof(buffer), stdin);

        // 서버에 메시지 전송
        send(client_socket, buffer, strlen(buffer), 0);

        // 서버로부터 메시지 수신
        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);

        // 수신한 메시지 출력
        printf("상대방: %s", buffer);
    }

    // 클라이언트 소켓 종료
    close(client_socket);

    return 0;
}

