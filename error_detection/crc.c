#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8084
#define GENERATOR "10011" // G(x) = x^4 + x + 1

// Modulo-2 division (XOR)
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

void check_crc(const char *codeword, const char *gen, char *rem) {
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

// Convert text to binary string
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

// Receiver thread running concurrently on TCP socket
void *crc_receiver_thread(void *arg) {
    int server_fd = *(int *)arg;
    int client = accept(server_fd, NULL, NULL);
    if (client < 0) return NULL;

    char codeword[1024];
    memset(codeword, 0, sizeof(codeword));
    read(client, codeword, sizeof(codeword) - 1);

    printf("\n===========================================\n");
    printf("        RECEIVER CRC VERIFICATION (TCP)    \n");
    printf("===========================================\n");
    printf("Received Codeword    : %s\n", codeword);
    printf("Generator Polynomial : %s\n", GENERATOR);

    char remainder[16];
    check_crc(codeword, GENERATOR, remainder);
    printf("Calculated Remainder : %s\n", remainder);
    printf("-------------------------------------------\n");

    int error_free = 1;
    for (int i = 0; i < (int)strlen(remainder); i++) {
        if (remainder[i] != '0') {
            error_free = 0;
            break;
        }
    }

    if (error_free) {
        printf(">>> RESULT: SUCCESS (Remainder is 0000 -> Frame is Error-Free! Accepted)\n");
    } else {
        printf(">>> RESULT: FAILED (Non-zero remainder -> Error Detected! Rejected)\n");
    }
    printf("===========================================\n");

    close(client);
    close(server_fd);
    return NULL;
}

int main() {
    char input[256], binary_data[1024];

    printf("===========================================\n");
    printf("              CRC SIMULATION               \n");
    printf("Generator Polynomial G(x) : %s (x^4 + x + 1)\n", GENERATOR);
    printf("===========================================\n");
    printf("Enter data (binary e.g. '100100' or text e.g. 'A'): ");
    if (!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\r\n")] = '\0';

    if (strlen(input) == 0) return 1;

    if (is_binary_string(input)) {
        strcpy(binary_data, input);
        printf("\nInput is binary bits   : %s\n", binary_data);
    } else {
        text_to_binary(input, binary_data);
        printf("\nConverted text to binary: %s\n", binary_data);
    }

    // 1. Sender CRC Computation
    char crc_rem[16];
    compute_crc(binary_data, GENERATOR, crc_rem);

    printf("Generator Polynomial   : %s\n", GENERATOR);
    printf("Computed CRC (4 bits)  : %s\n", crc_rem);

    char codeword[1024];
    strcpy(codeword, binary_data);
    strcat(codeword, crc_rem);
    printf("Transmitted Codeword   : %s\n", codeword);
    printf("-------------------------------------------\n");

    // 2. Error Injection Option
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

    // 3. Setup TCP Server Socket for Receiver
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return 1;
    }
    listen(server_fd, 1);

    pthread_t r_tid;
    pthread_create(&r_tid, NULL, crc_receiver_thread, &server_fd);

    // 4. Sender connects via TCP client socket
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &dest.sin_addr);

    usleep(50000); // 50ms wait for server
    if (connect(client_sock, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("TCP Connection failed");
        close(client_sock);
        return 1;
    }

    write(client_sock, codeword, strlen(codeword));
    close(client_sock);

    pthread_join(r_tid, NULL);
    return 0;
}
