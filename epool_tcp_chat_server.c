//epoll_tcp_chat_server.c

#include <sys/epoll.h>
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
#include <fcntl.h>

#define MAX_EVENTS 64
int client_fds[1024] = {0};
int client_count = 0;

// Set a socket to non-blocking mode
void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL) failed");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL) failed");
        exit(EXIT_FAILURE);
    }
}

void remove_client(int fd){
    for(int i = 0; i < client_count; i++){
        if(client_fds[i] == fd){
            client_fds[i] = client_fds[client_count - 1];
            client_count--;
            break;
        }
    }
}
    

int main(){
    int server_fd, client_fd;
    struct sockaddr_in serv_addr, client_addr;
    char buffer[1024] = {0};
    int epfd = epoll_create1(0);
    struct epoll_event ev; //Used to add sockets to epoll
    struct epoll_event events[MAX_EVENTS]; //Used to keep track of I/O operations for the registered sockets

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd <= 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");
    set_nonblocking(server_fd);
    
    //address reuse
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
    }
    printf("Set address reuse\n");

    //Adding server socket to epoll instance
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Bind complete\n");

    listen(server_fd, 3);
    printf("Listening...\n");
    
    socklen_t client_addr_len = sizeof(client_addr);

    while(true){
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1); //n = amount of file descriptors ready for I/O

        for(int i = 0; i < n; i++){
            int active_fd = events[i].data.fd;
            if(active_fd == server_fd){ //New client wants to connect
                client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
                if(client_fd <= 0){
                    perror("Accept failed\n");
                    exit(EXIT_FAILURE);
                }
                printf("Accepted connection succesfully\n");
                set_nonblocking(client_fd);

                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                client_fds[client_count++] = client_fd;
                printf("Added client_fd: %d to epoll\n", client_fd);

            }
            else{ //Client wants to perform I/O operation

                //read
                memset(buffer,0, sizeof(buffer));
                if(read(active_fd, buffer, 1024) <= 0){
                    perror("Read failed");
                    close(active_fd);
                    exit(EXIT_FAILURE);
                }
                printf("Read done\n");

                for(int j = 0; j < client_count; j++){ //looping through all active clients
                    if(client_fds[j] != server_fd){
                        if(write(client_fds[j], "hello from server", sizeof("hello from server")) <= 0){
                            perror("Write failed");
                            printf("Failed for fd: %d\n", client_fds[j]);
                            epoll_ctl(epfd, EPOLL_CTL_DEL, client_fds[j], &ev);
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                printf("Broadcast done\n");
                /*epoll_ctl(epfd, EPOLL_CTL_DEL, active_fd, &ev);
                remove_client(active_fd);
                close(active_fd); */
            }
        }
        
    }
}