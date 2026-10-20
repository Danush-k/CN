#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);
    char roll[20], buffer[1024];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 1. Prompt for Roll Number
    printf("Enter Roll Number: ");
    scanf("%s", roll);

    // 2. Send datagram to server
    sendto(sock, roll, strlen(roll), 0, (struct sockaddr *)&serv_addr, len);

    // 3. Receive and display response
    memset(buffer, 0, sizeof(buffer));
    recvfrom(sock, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
    printf("%s\n", buffer);

    // 4. Terminate
    close(sock);
    return 0;
}
