//UDP_time_client.c

#include <stdio.h>
#include <time.h>
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
    printf("UDP_TIME_CLIENT.C\n");
    printf("#################\n");

    int client_fd;
    struct sockaddr_in server_address;
    char const* message = "hello";
    char buffer[1024] = {0};
    socklen_t server_len = sizeof(server_address);

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_fd < 0){
        perror("socket not created");
        exit(EXIT_FAILURE);
    }
    printf("Created socket\n");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    if(sendto(client_fd, "hello from client", sizeof("hello from client"), MSG_CONFIRM, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        perror("sendto failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("sendto success\n");

    ssize_t bytes_received = recvfrom(client_fd, buffer, 1024, 0, (struct sockaddr*)&server_address, &server_len);
    if(bytes_received < 0){
            perror("recvfrom failed");
            close(client_fd);
            exit(EXIT_FAILURE);
    }
    buffer[bytes_received] = '\0';
    printf("Current time: %s\n", buffer);

    close(client_fd);
    return 0;
}