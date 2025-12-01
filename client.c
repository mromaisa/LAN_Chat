#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h> // Keep this if you plan to use threads later

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "172.24.148.223"

// FIX: Corrected main arguments (argc is int, argv is array)
int main(int argc, char* argv[]){
    
    // Step 1: Create a client socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // FIX: Changed condition to check if sock is less than 0 (error)
    if(sock < 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Step 2: Define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    
    // FIX: Added error checking for IP address conversion
    if(inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    // Step 3: Connect to the server
    if(connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server %s on port %d\n", SERVER_IP, PORT);

    // Step 4: Communicate with the server
    char buffer[BUFFER_SIZE] = {0};

    printf("Enter message: ");
    
    // FIX: Use fgets for C input (cin is C++)
    if(fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        // Remove the newline character '\n' added by fgets
        buffer[strcspn(buffer, "\n")] = 0;
    }

    send(sock, buffer, strlen(buffer), 0);
    
    // FIX: Changed 'message' to 'buffer'
    printf("Message sent to server: %s\n", buffer);

    // FIX: Always close the socket
    close(sock);

    return 0;
}