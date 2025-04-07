//tcp_server_echo.c

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
    printf("TCP_SERVER.C\n");
    printf("############\n");

    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    //create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");  // prints: "Socket creation failed: <system error message>"
        exit(EXIT_FAILURE);               // exits the program with error code
    }
    printf("Created socket\n");

    //address reuse
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
    }
    printf("Set address reuse\n");

    //Bind to IP and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Bind complete\n");

    //listen
    if(listen(server_fd, 3) < 0){
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Listening...\n");

    while(true){
        //accepting the connection
        int addrlen = sizeof(address);
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if(new_socket < 0){
            perror("Accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        printf("Accepted new connection\n");

        if(read(new_socket, buffer, 1024) < 0){
            perror("Read failed");
            close(server_fd);
            close(new_socket);
            exit(EXIT_FAILURE);
        }

        printf("Read recived: %s\n", buffer);

        if(write(new_socket, buffer, 1024) < 0){
            perror("Write failed");
            close(server_fd);
            close(new_socket);
            exit(EXIT_FAILURE);
        }
        printf("Echoed message back\n");

        close(new_socket);
    }

    close(new_socket);
    return 0;
}
