#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define TRUE 1
#define NUM_LINES 20
#define LINE_SIZE 256
#define MAX_CLIENTS 10

void *socket_thread(void *args) {
    int client_socket = *(int *)args;
    char str_in[1024];

    while (TRUE) {
        read(client_socket, &str_in, 1024);

        printf("A resposta foi: %s\n", str_in);
        char response = '0';
        write(client_socket, &response, 1);
    }

    return NULL;
}

int main() {
    int server_socket, client_socket;
    int server_length, client_length;
    int bind_socket;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9735);

    server_length = sizeof(server_address);

    bind_socket = bind(server_socket, (struct sockaddr *)&server_address, server_length);
    if (bind_socket == -1) {
        perror("There was an error binding the socket!\n");
        exit(1);
    }

    listen(server_socket, MAX_CLIENTS);
    while(TRUE) {
        printf("Server is waiting for connection\n");

        client_length = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, (unsigned int *)&client_length);

        // create thread that will be responsible for this socket
        pthread_t thread_socket;
        int thread = pthread_create(&thread_socket, NULL, &socket_thread, (void *)&client_socket);
        if (thread) {
            printf("Error on socket thread creating");
        }

//        read(client_socket, &str_in, 1024);
//        char response = '0';
//        write(client_socket, &response, 1);

//        printf("It was passed to the server: %s\n", str_in);
//        close(client_socket);
    }
}
