#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

#define TRUE 1

int main() {
    int server_socket, client_socket;
    int server_length, client_length;
    int bind_socket;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    char str_in[1024];

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

    listen(server_socket, 10);
    while(TRUE) {
        printf("Server is waiting for connection\n");

        client_length = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, (unsigned int *)&client_length);

        read(client_socket, &str_in, 1024);

        char response = '0';
        write(client_socket, &response, 1);

        printf("It was passed to the server: %s\n", str_in);
//        close(client_socket);
    }
}
