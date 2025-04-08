//epoll_chat_client

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
    printf("TCP_CHAT_CLIENT.C\n");
    printf("############\n");

    int client_fd;
    struct sockaddr_in serv_address;
    char buffer[1024] = {0};

    //create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("Socket creation failed");  // prints: "Socket creation failed: <system error message>"
        exit(EXIT_FAILURE);               // exits the program with error code
    }
    printf("Created socket\n");

    serv_address.sin_family = AF_INET;
    serv_address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_address.sin_addr);

    if(connect(client_fd, (struct sockaddr *)&serv_address, sizeof(serv_address)) < 0){
        perror("Connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");

    if(write(client_fd, "Hello from client Guy Bensky", sizeof("Hello from client Guy Bensky")) < 0){
        perror("Write failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("Written message\n");

    memset(buffer,0, sizeof(buffer));
    while(true){
        if(read(client_fd, buffer, 1024) <= 0){
            
            break;
        }
    }
    
    printf("Read done, buffer: %s\n", buffer);
    close(client_fd);
    printf("Closed connection");
    return 0;
}