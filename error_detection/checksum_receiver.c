#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8082

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    listen(server_fd, 3);

    printf("===========================================\n");
    printf("  CHECKSUM RECEIVER LISTENING ON PORT %d   \n", PORT);
    printf("===========================================\n\n");

    while (1) {
        int client = accept(server_fd, NULL, NULL);
        if (client < 0) continue;

        int len = 0;
        char data[256];
        unsigned char checksum = 0;

        read(client, &len, sizeof(int));
        read(client, data, len);
        data[len] = '\0';
        read(client, &checksum, sizeof(unsigned char));

        printf("===========================================\n");
        printf("       RECEIVER VERIFICATION (TCP)         \n");
        printf("===========================================\n");
        printf("Received Message  : %s\n", data);
        printf("Received Checksum : 0x%02X\n", checksum);
        printf("-------------------------------------------\n");

        unsigned int sum = 0;

        // 1. Add each 8-bit word with end-around carry
        for (int i = 0; i < len; i++) {
            unsigned char word = (unsigned char)data[i];
            sum += word;
            unsigned int carry = sum >> 8;
            if (carry) sum = (sum & 0xFF) + carry;
            printf("Word %d (0x%02X) -> Intermediate Sum: 0x%02X | Carry: %d\n",
                   i + 1, word, sum, carry);
        }

        // 2. Add received checksum
        sum += checksum;
        unsigned int carry = sum >> 8;
        if (carry) sum = (sum & 0xFF) + carry;
        printf("Checksum (0x%02X) -> Final Sum: 0x%02X | Carry: %d\n",
               checksum, sum, carry);

        // 3. 1's complement of total sum
        unsigned char result = (~sum) & 0xFF;
        printf("-------------------------------------------\n");
        printf("1's Complement of Sum: 0x%02X\n", result);

        if (result == 0x00) {
            printf(">>> RESULT: SUCCESS (Data is Error-Free! Accepted)\n");
            write(client, "SUCCESS (Data is Error-Free! Accepted)", 38);
        } else {
            printf(">>> RESULT: FAILED (Error Detected! Rejected)\n");
            write(client, "FAILED (Error Detected! Rejected)", 33);
        }
        printf("===========================================\n\n");

        close(client);
    }

    close(server_fd);
    return 0;
}
