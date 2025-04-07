//multi_client_TCP_chat_client.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <stdbool.h>

int main(){
    int client_fd;
    char const* message = "hello from client";
    char buffer[1024] = {0};
    struct sockaddr_in server_address;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0){
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
    printf("Created socket\n");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    if(connect(client_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        perror("Connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected successfully\n");

    if(write(client_fd, message, strlen(message)) < 0){
        perror("Write failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Write success\n");

    if(read(client_fd, buffer, 1024) < 0){
        perror("Read failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Read success, message received: %s\n", buffer);

    close(client_fd);
}