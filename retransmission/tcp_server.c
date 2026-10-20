#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_size = sizeof(client_addr);
    char buffer[100];
    int dropped = 0; // Flag to simulate dropping one frame for demo

    // 1. Create Socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Server Address Config
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // 4. Listen
    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        return 1;
    }

    printf("==========================================\n");
    printf("   TCP Server Waiting for Connection...   \n");
    printf("==========================================\n");

    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_size);
    if (client_socket < 0) {
        perror("Accept failed");
        return 1;
    }
    printf("Client Connected!\n\n");

    // 5. Receive Frames
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            printf("\nClient disconnected. Exiting.\n");
            break;
        }

        int frame_num;
        sscanf(buffer, "Frame %d", &frame_num);

        // Simulate loss on Frame 2 on its first arrival
        if (frame_num == 2 && dropped == 0) {
            printf("[Server] Received '%s' -> [SIMULATING LOSS: Dropping frame, no ACK sent]\n", buffer);
            dropped = 1;
            continue; // Client will timeout and retransmit
        }

        printf("[Server] Received '%s' -> Sending ACK\n", buffer);
        send(client_socket, "ACK", 3, 0);
    }

    close(client_socket);
    close(server_socket);
    return 0;
}
