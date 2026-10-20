#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 9053
#define BUFFER_SIZE 512

// DNS Database Table (Hostname -> IP Address)
struct DNSRecord {
    char domain[100];
    char ip[50];
};

struct DNSRecord dns_table[] = {
    {"google.com",  "142.250.190.46"},
    {"yahoo.com",   "98.137.11.163"},
    {"github.com",  "140.82.121.4"},
    {"example.com", "93.184.216.34"},
    {"openai.com",  "104.18.3.161"},
    {"timeout.com", "SIMULATE_TIMEOUT"} // For timeout simulation
};
int table_size = sizeof(dns_table) / sizeof(dns_table[0]);

// Validate domain format
int is_valid_domain(const char *domain) {
    if (strlen(domain) < 3) return 0;
    if (strchr(domain, ' ') != NULL) return 0; // No spaces
    if (strchr(domain, '.') == NULL) return 0; // Must have at least one dot
    return 1;
}

// Resolve domain name to IP address
const char* resolve_dns(const char *domain) {
    if (!is_valid_domain(domain)) {
        return "INVALID_FORMAT";
    }

    for (int i = 0; i < table_size; i++) {
        if (strcasecmp(dns_table[i].domain, domain) == 0) {
            return dns_table[i].ip;
        }
    }
    return "NXDOMAIN"; // Domain not found
}

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t len = sizeof(cli_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        close(sock);
        return 1;
    }

    printf("===========================================\n");
    printf("     UDP DNS SERVER LISTENING ON PORT %d    \n", PORT);
    printf("===========================================\n");

    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr *)&cli_addr, &len);
        if (bytes <= 0) continue;

        buffer[strcspn(buffer, "\r\n")] = '\0'; // Remove newline
        printf("\n[QUERY RECEIVED] Client %s:%d requested: '%s'\n",
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);

        const char *result = resolve_dns(buffer);

        // Simulate timeout if requested
        if (strcmp(result, "SIMULATE_TIMEOUT") == 0) {
            printf("[TIMEOUT SIMULATION] Dropping packet for '%s' to trigger client timeout...\n", buffer);
            continue; // Do not respond, simulating packet drop/server delay
        }

        printf("[RESPONSE SENT] %s -> %s\n", buffer, result);
        sendto(sock, result, strlen(result), 0, (struct sockaddr *)&cli_addr, len);
    }

    close(sock);
    return 0;
}
