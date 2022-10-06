#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define TRUE 1
#define NUM_LINES 20
#define LINE_SIZE 255
#define MAX_CLIENTS 10

pthread_mutex_t mutex ;

char *FILENAME = "file.txt";
char *TEMPFILE = "temp.tmp";

typedef struct parse {
    int op;
    int index;
    char *text;
} parse_t;

parse_t *parse_text(char *text) {
    char DELIMITER = ',';
    parse_t *parse = malloc(sizeof *parse);

    if (text[0] == '3') {
        parse->op = 3;
        parse->index = 0;
        parse->text = "";
        return parse;
    }

    char *token = strtok(text, &DELIMITER);
    parse->op = atoi(token);

    token = strtok(NULL, &DELIMITER);
    parse->index = atoi(token);

    token = strtok(NULL, &DELIMITER);
    parse->text = token;

    return parse;
}

char *read_file(parse_t *parse) {
    FILE *file;
    char line[LINE_SIZE];
    file = fopen(FILENAME, "r");

    if (file == NULL) {
        printf("There was an error opening the file. File does not exist \n");
        file = fopen(FILENAME, "w");
    }

    int i = 0;
    while ((fgets(line, LINE_SIZE, file)) != NULL) {
        i++;
        if (i == parse->index)
            return strdup(line);
    }

    fclose(file);
    return NULL;
}

char *write_file(parse_t *parse) {
    FILE *file;
    FILE *file_temp;
    char line[LINE_SIZE];
    int count = 0;

    file = fopen(FILENAME, "r");
    file_temp = fopen(TEMPFILE, "w");
    if (file_temp == NULL || file == NULL) {
        printf("There was an error opening files. \n");
        return NULL;
    }

    while ((fgets(line, LINE_SIZE, file)) != NULL) {
        count++;

        if (count == parse->index) {
            fputs(strcat(parse->text, "\n"), file_temp);
        } else {
            fputs(line, file_temp);
        }
    }

    // close files
    fclose(file);
    fclose(file_temp);

    // delete original source
    remove(FILENAME);

    // rename temp to original file
    rename(TEMPFILE, FILENAME);

    char *response = "1";

    return response;
}

void *socket_thread(void *args) {
    int client_socket = *(int *)args;
    char str_in[1024];

    while (TRUE) {
        read(client_socket, &str_in, 1024);
        parse_t *parse = parse_text(str_in);
        char *result = NULL;
        char response[LINE_SIZE+1];

        // index out of range
        if (parse->index > NUM_LINES || parse->index < 0) {
            response[0] = '3';
            write(client_socket, &response, LINE_SIZE+1);
            free(parse);
            continue;
        }

        // parse by op code
        switch (parse->op) {
            case 1:
                pthread_mutex_lock(&mutex);
                    result = write_file(parse);
                pthread_mutex_unlock(&mutex);
                break;
            case 2:
                pthread_mutex_lock(&mutex);
                    result = read_file(parse);
                pthread_mutex_unlock(&mutex);
                break;
            case 3:
                return NULL;
        }

        // treat answer
        if (result != NULL) {
            response[0] = parse->op == 1 ? '0' : '1';
            strcpy(response+1, result);
        } else
            response[0] = '2';

        free(parse);
        write(client_socket, &response, LINE_SIZE+1);
    }
}

void create_empty_file() {
    FILE *file;
    char *content = "\n";

    file = fopen(FILENAME, "r");

    // test if file already exists
    if (file == NULL) {
        file = fopen(FILENAME, "w");
        for (int i = 0; i < NUM_LINES-1; i++) {
            fputs(content, file);
        }
        fclose(file);
    }
}

int main() {
    int server_socket, client_socket;
    int server_length, client_length;
    int bind_socket;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    // create empty file, if it is already not created
    create_empty_file();

    // initialize mutex
    pthread_mutex_init(&mutex, NULL);

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
    }

    // destroy mutex
    pthread_mutex_destroy(&mutex);
    // close(server_socket);
}
