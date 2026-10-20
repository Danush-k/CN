#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 8080
#define TOTAL_FRAMES 4
#define TIMEOUT_SEC 2
#define MAX_RETRIES 3

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[100];
    char ack[10];

    // 1. Create Socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Set Receive Timeout for ACK (Triggers Retransmission)
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC; // 2 seconds timeout
    tv.tv_usec = 0;
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt failed");
        return 1;
    }

    // 3. Server Address Config
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // 4. Connect
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        return 1;
    }

    printf("Connected to server on port %d.\n", PORT);
    printf("ACK Timeout set to %d seconds.\n\n", TIMEOUT_SEC);

    // 5. Send Frames with Retransmission on Timeout
    for (int frame = 1; frame <= TOTAL_FRAMES; frame++) {
        int ack_received = 0;
        int attempts = 0;

        while (!ack_received && attempts < MAX_RETRIES) {
            attempts++;
            sprintf(buffer, "Frame %d", frame);

            if (attempts == 1) {
                printf("[Client] Sending '%s'...\n", buffer);
            } else {
                printf("[Client] RETRANSMITTING '%s' (Attempt %d)...\n", buffer, attempts);
            }

            // Send frame to server
            send(client_socket, buffer, strlen(buffer), 0);

            // Wait for ACK
            memset(ack, 0, sizeof(ack));
            int bytes = recv(client_socket, ack, sizeof(ack) - 1, 0);

            if (bytes > 0 && strcmp(ack, "ACK") == 0) {
                printf("[Client] Received ACK for '%s'!\n\n", buffer);
                ack_received = 1;
            } else {
                // recv() returned -1 due to SO_RCVTIMEO timeout
                printf("[Client] TIMEOUT! No ACK received for '%s'.\n", buffer);
            }
        }

        if (!ack_received) {
            printf("[Client] Failed to deliver '%s' after %d attempts.\n", buffer, MAX_RETRIES);
            break;
        }

        sleep(1);
    }

    printf("All frames successfully sent. Closing socket.\n");
    close(client_socket);
    return 0;
}
