#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "172.24.148.223"

int main(int argv, char* argc[]){
    // Step 1: Create a client socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock , 0){
        // int network_logs = open("network_logs.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        // write(network_logs, "Socket creation failed\n", 23);
        // close(network_logs);
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Step 2: Define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    // Step 3: Connect to the server
    if(connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        // int network_logs = open("network_logs.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        // write(network_logs, "Connection to server failed\n", 28);
        // close(network_logs);
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server %s on port %d\n", SERVER_IP, PORT);
    // Step 4: Communicate with the server
    char buffer[BUFFER_SIZE] = {0};
    const char* message = "Hello from client";
    send(sock, message, strlen(message), 0);
    printf("Message sent to server: %s\n", message);

    return 0;

}
