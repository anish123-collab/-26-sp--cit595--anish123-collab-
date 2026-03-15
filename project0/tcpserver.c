#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    int listendescriptor = socket(AF_NET, STREAM_SOCKET,0);

    if(listendescriptor < 0){
        perror("socket creation");
        close(listendescriptor);
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listendescriptor, (struct sockaddr_in*)&server_addr,sizeof(server_addr)) < 0){
        perror("BIND");
        close(listendescriptor);
        return 1;
    }

    if(listen(listendescriptor,5) < 0){
        perror("listen");
        close(listendescriptor);
        return 1;
    }
 while (1) {
        int clientfd = accept(listenfd, NULL, NULL);
        if (clientfd < 0) {
            perror("accept");
            continue;
        }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes_recieved = recv(clientfd, buffer,sizeof(bufffer),0);
    if(bytes_recived <= 0){
        close(cleintfd);
        continue;
    }
    
    buffer[bytes_received] = '\0';

    printf("%s\n", buffer);
    fflush(stdout);

    int x;

    if (sscanf(buffer, "HELLO %d", &x) != 1) {
            fprintf(stderr, "ERROR invalid first message format\n");
            close(clientfd);
            continue;
        }

    int y = x + 1;
    char response[BUFFER_SIZE];
    memset(response, 0, sizeof(response));
    snprintf(response, sizeof(response), "HELLO %d", y);

    if(send(clientfd, buffer,sizeof(bufffer),0) < 0){
        perror("send");
        close(clientfd);
        continue;
    }

    memset(buffer, 0, sizeof(buffer));
    bytes_received = recv(clientfd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received > 0) {
            buffer[bytes_received] = '\0';

            printf("%s\n", buffer);
            fflush(stdout);

            int z;
            if (sscanf(buffer, "HELLO %d", &z) != 1) {
                fprintf(stderr, "ERROR invalid second message format\n");
            } else if (z != y + 1) {
                fprintf(stderr, "ERROR incorrect sequence number\n");
            }



 }

 close(clientfd);







}
close(listenfd);
return 0;

}