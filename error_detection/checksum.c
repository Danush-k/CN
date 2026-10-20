#include <stdio.h>
#include <string.h>

// 1. Sender calculates checksum
unsigned char compute_checksum(const char *data, int len) {
    unsigned int sum = 0;
    printf("\n--- SENDER: 1'S COMPLEMENT ADDITION ---\n");

    for (int i = 0; i < len; i++) {
        unsigned char word = (unsigned char)data[i];
        sum += word;
        unsigned int carry = sum >> 8;
        if (carry) sum = (sum & 0xFF) + carry; // End-around carry
        printf("Word %d (0x%02X) -> Intermediate Sum: 0x%02X | Carry: %d\n",
               i + 1, word, sum, carry);
    }

    unsigned char checksum = (~sum) & 0xFF; // 1's complement
    printf("---------------------------------------\n");
    printf("Final Sum       : 0x%02X\n", sum);
    printf("Generated Checksum : 0x%02X\n", checksum);
    printf("---------------------------------------\n");
    return checksum;
}

// 2. Receiver verifies data + checksum
void verify_checksum(const char *data, int len, unsigned char checksum) {
    unsigned int sum = 0;
    printf("\n--- RECEIVER: VERIFICATION ---\n");

    for (int i = 0; i < len; i++) {
        unsigned char word = (unsigned char)data[i];
        sum += word;
        unsigned int carry = sum >> 8;
        if (carry) sum = (sum & 0xFF) + carry;
        printf("Word %d (0x%02X) -> Intermediate Sum: 0x%02X | Carry: %d\n",
               i + 1, word, sum, carry);
    }

    // Add received checksum
    sum += checksum;
    unsigned int carry = sum >> 8;
    if (carry) sum = (sum & 0xFF) + carry;
    printf("Checksum (0x%02X) -> Final Sum: 0x%02X | Carry: %d\n",
           checksum, sum, carry);

    unsigned char result = (~sum) & 0xFF;
    printf("---------------------------------------\n");
    printf("1's Complement of Sum: 0x%02X\n", result);

    if (result == 0x00) {
        printf(">>> RESULT: SUCCESS (Data is Error-Free! Accepted)\n\n");
    } else {
        printf(">>> RESULT: FAILED (Error Detected! Rejected)\n\n");
    }
}

int main() {
    char data[256];

    printf("=======================================\n");
    printf("          CHECKSUM SIMULATION          \n");
    printf("=======================================\n");
    printf("Enter string data: ");
    if (!fgets(data, sizeof(data), stdin)) return 1;
    data[strcspn(data, "\r\n")] = '\0';

    int len = strlen(data);
    if (len == 0) return 1;

    // Step 1: Sender computes checksum
    unsigned char checksum = compute_checksum(data, len);

    // Step 2: Option to simulate transmission error
    printf("Introduce bit error? (1 = Yes, 0 = No): ");
    int choice = 0;
    scanf("%d", &choice);
    if (choice == 1) {
        data[0] ^= 0x01; // Invert 1 bit
        printf("[ERROR INJECTED] Altered byte 1 to '%c' (0x%02X)\n",
               data[0], (unsigned char)data[0]);
    }

    // Step 3: Receiver verifies
    verify_checksum(data, len, checksum);

    return 0;
}
