#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8083
#define GENERATOR "10011" // G(x) = x^4 + x + 1

// Modulo-2 division (XOR) to calculate remainder
void calculate_crc_remainder(const char *codeword, const char *gen, char *rem) {
    int code_len = strlen(codeword);
    int gen_len = strlen(gen);
    char temp[1024];
    strcpy(temp, codeword);

    for (int i = 0; i <= code_len - gen_len; i++) {
        if (temp[i] == '1') {
            for (int j = 0; j < gen_len; j++) {
                temp[i + j] = (temp[i + j] == gen[j]) ? '0' : '1';
            }
        }
    }

    strncpy(rem, temp + code_len - (gen_len - 1), gen_len - 1);
    rem[gen_len - 1] = '\0';
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }

    listen(server_fd, 3);
    printf("===========================================\n");
    printf("     CRC RECEIVER LISTENING ON PORT %d      \n", PORT);
    printf("Generator Polynomial G(x) : %s (x^4 + x + 1)\n", GENERATOR);
    printf("===========================================\n");

    while (1) {
        int client_sock = accept(server_fd, NULL, NULL);
        if (client_sock < 0) continue;

        char codeword[1024];
        memset(codeword, 0, sizeof(codeword));
        int bytes = read(client_sock, codeword, sizeof(codeword) - 1);
        if (bytes <= 0) {
            close(client_sock);
            continue;
        }

        printf("\n[FRAME RECEIVED] Codeword: %s (Length: %lu bits)\n",
               codeword, strlen(codeword));

        // Recalculate CRC remainder using the same generator
        char remainder[16];
        calculate_crc_remainder(codeword, GENERATOR, remainder);

        printf("-------------------------------------------\n");
        printf("Generator Polynomial   : %s\n", GENERATOR);
        printf("Calculated Remainder   : %s\n", remainder);
        printf("-------------------------------------------\n");

        // Check if remainder is all zeroes
        int is_error_free = 1;
        for (int i = 0; i < (int)strlen(remainder); i++) {
            if (remainder[i] != '0') {
                is_error_free = 0;
                break;
            }
        }

        if (is_error_free) {
            printf("[VERIFICATION RESULT] Remainder is all 0s.\n");
            printf(">>> Frame is ERROR-FREE! (Accepted)\n");

            // Extract original data (without 4-bit CRC)
            int data_len = strlen(codeword) - (strlen(GENERATOR) - 1);
            char orig_data[1024];
            strncpy(orig_data, codeword, data_len);
            orig_data[data_len] = '\0';
            printf("Original Data Bits: %s\n", orig_data);
            write(client_sock, "SUCCESS (Frame is Error-Free! Accepted)", 39);
        } else {
            printf("[VERIFICATION RESULT] Non-zero remainder detected (%s).\n", remainder);
            printf(">>> ERROR DETECTED! Frame contains bit errors. (Rejected)\n");
            write(client_sock, "FAILED (Error Detected! Rejected)", 33);
        }
        printf("===========================================\n");

        close(client_sock);
    }

    close(server_fd);
    return 0;
}
