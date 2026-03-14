#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port> <initial_seq_num>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    int x = atoi(argv[3]);

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(socketfd);
        return 1;
    }

    if (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(socketfd);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "HELLO %d", x);

    int len = strlen(buffer);
    if (send(socketfd, buffer, len, 0) < 0) {
        perror("send");
        close(socketfd);
        return 1;
    }

    memset(buffer, 0, sizeof(buffer));
    ssize_t rec = recv(socketfd, buffer, sizeof(buffer) - 1, 0);
    if (rec <= 0) {
        perror("recv");
        close(socketfd);
        return 1;
    }

    buffer[rec] = '\0';

    printf("%s\n", buffer);
    fflush(stdout);

    int y;
    if (sscanf(buffer, "HELLO %d", &y) != 1) {
        fprintf(stderr, "ERROR invalid response format\n");
        close(socketfd);
        return 1;
    }

    if (y != x + 1) {
        fprintf(stderr, "ERROR incorrect sequence number\n");
        close(socketfd);
        return 1;
    }

    int z = y + 1;
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "HELLO %d", z);

    if (send(socketfd, buffer, strlen(buffer), 0) < 0) {
        perror("send");
        close(socketfd);
        return 1;
    }

    close(socketfd);
    return 0;
}