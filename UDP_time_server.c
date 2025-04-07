//UDP_time_server.c

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
#include <string.h>

int main(){
    printf("UDP_TIME_SERVER.C\n");
    printf("#################\n");

    int server_fd;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    char buffer[1024] = {0};
    char *timeBuf;
    char client_ip[INET_ADDRSTRLEN];
    time_t now = time(NULL);
    socklen_t client_len = sizeof(client_address);

    //create socket
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");  // prints: "Socket creation failed: <system error message>"
        exit(EXIT_FAILURE);               // exits the program with error code
    }
    printf("Created socket\n");

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Bind complete\n");

    
    while(true){
        
        if(recvfrom(server_fd, buffer, 1024, 0, (struct sockaddr*)&client_address, &client_len) < 0){
            perror("recvfrom failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        inet_ntop(AF_INET, &client_address.sin_addr, client_ip, sizeof(client_ip));
        printf("Received message from %s:%d, message: %s\n", client_ip, ntohs(client_address.sin_port), buffer);

        if(buffer != NULL){
            timeBuf = ctime(&now);
            if(sendto(server_fd, timeBuf, strlen(timeBuf), MSG_CONFIRM, (struct sockaddr*)&client_address, sizeof(client_address)) < 0){
                perror("sendto failed");
                close(server_fd);
                exit(EXIT_FAILURE);
            }
            printf("sendto sent: %s", timeBuf);
        }
    }

    close(server_fd);
}