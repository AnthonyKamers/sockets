#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

int connect_socket() {
    int client_socket, length, result;
    struct sockaddr_in address;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9735);

    length = sizeof(address);
    result = connect(client_socket, (struct sockaddr *)&address, length);
    if (result == -1) {
        perror("There was an error starting the client_socket.\n");
        exit(1);
    }

    return client_socket;
}

void write_to_server(char *text, int client_socket) {
    write(client_socket, text, 1024);

    char response_server[256];
    char answer[255];
    read(client_socket, &response_server, 256);
    strcpy(answer, response_server+1);

    switch (response_server[0]) {
        case '0':
            // write successfully
            printf("Wrote to file successfully. \n");
            break;
        case '1':
            // read successfully
            printf("Content: %s \n", answer);
            break;
        case '2':
            // error reading
            printf("Error reading line of the file \n");
        case '3':
            printf("Line index not in the range of the file \n");
        default:
            printf("Unexpected error. \n");
            break;
    }
}

int main() {
    int client_socket;
    char user_input[1024];
    int is_connected = FALSE;

    printf("Client program to communicate with the server\n");
    printf("Commands: \n"
           "0                           - Connect to the server \n"
           "1,INDEX,TEXT -> int,int,str - Add something to the file \n"
           "2,INDEX      -> int,int     - Get some line of the file \n"
           "3                           - Exit the server\n");

    while (TRUE) {
        printf("\n\nType your command: ");
        scanf("%s", user_input);
        char command = user_input[0];
        int exit_now = FALSE;

        printf("command chosen: %c\n", command);

        switch(command) {
            case '0':
                if (is_connected) {
                    printf("It is already connected. \n");
                } else {
                    client_socket = connect_socket();
                    is_connected = TRUE;
                }
                break;
            case '1':
            case '2':
                write_to_server((char *) &user_input, client_socket);
                break;
            case '3':
                close(client_socket);
                exit_now = TRUE;
                break;
            default:
                printf("Command invalid. Try again.\n");
                break;
        }

        if (exit_now) break;
    }

    exit(0);
}