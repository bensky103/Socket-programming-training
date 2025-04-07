//multi_client_TCP_chat_server.c

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
#include <signal.h>
#include <sys/wait.h>

void sigchld_handler(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(){

    int server_fd, client_fd;
    char buffer[1024] = {0};
    const char* message = "hello from server";
    char newMessage[1024] = {0};
    struct sockaddr_in server_address, client_address;
    int message_counter = 0;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("Creating socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    //address reuse
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        perror("Address reuse failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Bind complete\n");

    listen(server_fd, 3);
    printf("Listening...\n");

    signal(SIGCHLD, sigchld_handler);

    while(true){
        socklen_t client_address_len = sizeof(client_address);
        client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_address_len);
        message_counter++;
        pid_t pid = fork();
        if(pid < 0){
            perror("Fork failed");
            close(server_fd);
            close(client_fd);
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){
            if(read(client_fd, buffer, 1024) <= 0){
                perror("Read failed");
                close(server_fd);
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            printf("Read done, received message: %s\n", buffer);
            
            snprintf(newMessage, sizeof(newMessage), "%s%d", message, message_counter);
            if(write(client_fd, newMessage, strlen(newMessage)) <= 0){
                perror("Write failed");
                close(server_fd);
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            printf("Write done\n");
            exit(0);
        }
        else{
            printf("Parent process PID: %d, child process PID: %d\n", getpid(), pid);
        }
    }

    close(server_fd);
    close(client_fd);
}