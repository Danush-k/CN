#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    char roll[20], buffer[1024];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // 1. Connect to TCP Server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed!\n");
        return 1;
    }

    // 2. Prompt for Roll Number
    printf("Enter Roll Number: ");
    scanf("%s", roll);

    // 3. Send to server
    write(sock, roll, strlen(roll));

    // 4. Receive and display response
    memset(buffer, 0, sizeof(buffer));
    read(sock, buffer, sizeof(buffer) - 1);
    printf("%s\n", buffer);

    // 5. Close gracefully
    close(sock);
    return 0;
}
