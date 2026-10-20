#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082

int main() {
    char data[256];

    printf("===========================================\n");
    printf("             CHECKSUM SENDER               \n");
    printf("===========================================\n");
    printf("Enter string data: ");
    if (!fgets(data, sizeof(data), stdin)) return 1;
    data[strcspn(data, "\r\n")] = '\0';

    int len = strlen(data);
    if (len == 0) return 1;

    // 1. Sender Checksum Calculation
    printf("\n--- SENDER CHECKSUM CALCULATION ---\n");
    unsigned int sum = 0;

    for (int i = 0; i < len; i++) {
        unsigned char word = (unsigned char)data[i];
        sum += word;
        unsigned int carry = sum >> 8;
        if (carry) sum = (sum & 0xFF) + carry; // End-around carry
        printf("Word %d (0x%02X) -> Intermediate Sum: 0x%02X | Carry: %d\n",
               i + 1, word, sum, carry);
    }

    unsigned char checksum = (~sum) & 0xFF; // 1's complement
    printf("-------------------------------------------\n");
    printf("Final Sum          : 0x%02X\n", sum);
    printf("Generated Checksum : 0x%02X\n", checksum);
    printf("-------------------------------------------\n");

    // 2. Error Injection Option
    printf("Introduce bit error? (1 = Yes, 0 = No): ");
    int choice = 0;
    if (scanf("%d", &choice) == 1 && choice == 1) {
        data[0] ^= 0x01; // Invert 1 bit
        printf("[ERROR INJECTED] Altered byte 1 to '%c' (0x%02X)\n",
               data[0], (unsigned char)data[0]);
    }

    // 3. Connect to Receiver over TCP Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    printf("\nConnecting to Receiver on port %d...\n", PORT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error: Could not connect to Receiver! Make sure './checksum_receiver' is running in another terminal.\n");
        close(sock);
        return 1;
    }

    // 4. Send Data + Checksum over Socket
    write(sock, &len, sizeof(int));
    write(sock, data, len);
    write(sock, &checksum, sizeof(unsigned char));

    // 5. Read Verification Response from Receiver
    char response[128];
    memset(response, 0, sizeof(response));
    read(sock, response, sizeof(response) - 1);

    printf("[SUCCESS] Transmitted over TCP!\n");
    printf(">>> Receiver Response: %s\n\n", response);

    close(sock);
    return 0;
}
