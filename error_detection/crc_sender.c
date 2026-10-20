#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8083
#define GENERATOR "10011" // G(x) = x^4 + x + 1

// Modulo-2 division (XOR) to compute CRC remainder
void compute_crc(const char *data, const char *gen, char *rem) {
    int data_len = strlen(data);
    int gen_len = strlen(gen);
    char temp[1024];

    strcpy(temp, data);
    for (int i = 0; i < gen_len - 1; i++) {
        temp[data_len + i] = '0';
    }
    temp[data_len + gen_len - 1] = '\0';

    int total_len = strlen(temp);
    for (int i = 0; i <= total_len - gen_len; i++) {
        if (temp[i] == '1') {
            for (int j = 0; j < gen_len; j++) {
                temp[i + j] = (temp[i + j] == gen[j]) ? '0' : '1';
            }
        }
    }
    strncpy(rem, temp + total_len - (gen_len - 1), gen_len - 1);
    rem[gen_len - 1] = '\0';
}

// Convert text string into its binary representation
void text_to_binary(const char *text, char *bin) {
    bin[0] = '\0';
    for (int i = 0; text[i] != '\0'; i++) {
        unsigned char ch = text[i];
        for (int b = 7; b >= 0; b--) {
            strcat(bin, ((ch >> b) & 1) ? "1" : "0");
        }
    }
}

int is_binary_string(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != '0' && str[i] != '1') return 0;
    }
    return 1;
}

int main() {
    char input[256], binary_data[1024];

    printf("===========================================\n");
    printf("                CRC SENDER                 \n");
    printf("Generator Polynomial G(x) : %s (x^4 + x + 1)\n", GENERATOR);
    printf("===========================================\n");
    printf("Enter data (binary e.g. '100100' or text e.g. 'A'): ");
    if (!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\r\n")] = '\0';

    if (strlen(input) == 0) return 1;

    // Convert text to binary if not already binary
    if (is_binary_string(input)) {
        strcpy(binary_data, input);
        printf("\nInput is binary bits   : %s\n", binary_data);
    } else {
        text_to_binary(input, binary_data);
        printf("\nConverted text to binary: %s\n", binary_data);
    }

    // 1. Sender computes 4-bit CRC
    char crc_rem[16];
    compute_crc(binary_data, GENERATOR, crc_rem);

    printf("Generator Polynomial   : %s\n", GENERATOR);
    printf("Computed CRC (4 bits)  : %s\n", crc_rem);

    // Form Codeword: Data + CRC
    char codeword[1024];
    strcpy(codeword, binary_data);
    strcat(codeword, crc_rem);
    printf("Transmitted Codeword   : %s\n", codeword);
    printf("-------------------------------------------\n");

    // 2. Option to inject bit error
    printf("Introduce bit error? (1 = Yes, 0 = No): ");
    int choice = 0;
    if (scanf("%d", &choice) == 1 && choice == 1) {
        int pos = 0;
        printf("Enter bit index to flip (0 to %lu): ", strlen(codeword) - 1);
        if (scanf("%d", &pos) == 1 && pos >= 0 && pos < (int)strlen(codeword)) {
            codeword[pos] = (codeword[pos] == '1') ? '0' : '1';
            printf("[ERROR INJECTED] Flipped bit at index %d!\n", pos);
            printf("Corrupted Codeword     : %s\n", codeword);
        }
    }

    // 3. Connect to CRC Receiver on Port 8083 over TCP Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    printf("\nConnecting to CRC Receiver on port %d...\n", PORT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Error: Could not connect to Receiver! Make sure './crc_receiver' is running in another terminal.\n");
        close(sock);
        return 1;
    }

    // 4. Send Codeword to Receiver
    write(sock, codeword, strlen(codeword));

    // 5. Read Receiver Response
    char response[128];
    memset(response, 0, sizeof(response));
    read(sock, response, sizeof(response) - 1);

    printf("[SUCCESS] Transmitted codeword to CRC Receiver over TCP!\n");
    printf(">>> Receiver Response: %s\n\n", response);

    close(sock);
    return 0;
}
