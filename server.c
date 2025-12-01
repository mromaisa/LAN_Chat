#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 5

struct client_info {
    int socket_fd;
    struct sockaddr_in address;
    char username[50];
};

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

struct client_info* clients[MAX_CLIENTS];
int client_count = 0;

void broadcast_message(const char* message, struct client_info* sender) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i]->socket_fd != sender->socket_fd) {
            send(clients[i]->socket_fd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* client_handler(void* arg) {
    struct client_info* cli = (struct client_info*) arg;
    char buffer[BUFFER_SIZE];
    int bytesRead;

    // Ask username
    send(cli->socket_fd, "Enter username: ", 16, 0);
    bytesRead = read(cli->socket_fd, buffer, BUFFER_SIZE);
    buffer[bytesRead - 1] = '\0';
    strcpy(cli->username, buffer);

    printf("User connected: %s\n", cli->username);

    // Chat loop
    while ((bytesRead = read(cli->socket_fd, buffer, BUFFER_SIZE)) > 0) {
        buffer[bytesRead] = '\0';

        char formatted[1100];
        snprintf(formatted, sizeof(formatted), "%s: %s", cli->username, buffer);

        printf("%s", formatted);
        broadcast_message(formatted, cli);
    }

    // Cleanup
    close(cli->socket_fd);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == cli) {
            clients[i] = clients[client_count - 1];
            break;
        }
    }
    client_count--;
    pthread_mutex_unlock(&clients_mutex);

    free(cli);
    return NULL;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("Socket failed"); exit(1); }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    listen(server_fd, MAX_CLIENTS);
    printf("Server running on port %d\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);

        int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);

        struct client_info* cli = malloc(sizeof(struct client_info));
        cli->socket_fd = new_socket;
        cli->address = client_addr;

        pthread_mutex_lock(&clients_mutex);
        clients[client_count++] = cli;
        pthread_mutex_unlock(&clients_mutex);

        pthread_t thread;
        pthread_create(&thread, NULL, client_handler, cli);
        pthread_detach(thread);
    }

    return 0;
}
