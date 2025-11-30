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

int main(int argv, char* argc){
    // create a server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if(server_fd == 0){
        // int logs_network = open("network_logs.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        // write(logs_network, "Socket creation failed\n", 23);
        // close(logs_network);
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Step 2: Bind the socket to an IP/port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port= htons(PORT);

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        // int logs_network = open("network_logs.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        // write(logs_network, "Binding failed\n", 15);
        // close(logs_network);
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Step 3: Listen for incoming connections
    if(listen(server_fd, 5) < 0){
        // int logs_network = open("network_logs.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        // write(logs_network, "Listening failed\n", 17);
        // close(logs_network);
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Step 4: Accept a connection
    struct sockaddr_in client_address;
    socklen_t client_addrlen = sizeof(client_address);


    int client_array[5];
    int i=0;
    while(1){
        if (i>=5){
            break;
        }
        int newSocket = accept(server_fd, (struct sockaddr* )&client_address, &client_addrlen);
        if(newSocket < 0){
            // int logs_network = open("network_logs.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
            // write(logs_network, "Accepting connection failed\n", 28);
            // close(logs_network);
            perror("Accepting connection failed");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        client_array[i]=newSocket;
        i++;

    }

    char buffer[BUFFER_SIZE] = {0};
    int bytesRead = read(newSocket, buffer, BUFFER_SIZE);
    if(bytesRead < 0){
        // int logs_network = open("network_logs.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        // write(logs_network, "Reading from socket failed\n", 27);
        // close(logs_network);
        perror("Reading from socket failed");
        close(newSocket);
        exit(EXIT_FAILURE);
    }

    printf("Message from client: %s\n", buffer);

    return 0;
}