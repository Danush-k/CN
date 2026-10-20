#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#define SERVER_PORT 9053
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 512
#define CACHE_SIZE 10
#define CACHE_TTL 60 // Cache Time-To-Live in seconds

// Client-Side Cache Structure
struct CacheEntry {
    char domain[100];
    char ip[50];
    time_t timestamp;
    int is_valid;
};

struct CacheEntry cache[CACHE_SIZE];

// Initialize cache
void init_cache() {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache[i].is_valid = 0;
    }
}

// Check if domain exists in local cache (Cache Lookup)
int check_cache(const char *domain, char *ip_out) {
    time_t now = time(NULL);
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].is_valid && strcasecmp(cache[i].domain, domain) == 0) {
            if (difftime(now, cache[i].timestamp) < CACHE_TTL) {
                strcpy(ip_out, cache[i].ip);
                return 1; // Cache Hit
            } else {
                cache[i].is_valid = 0; // Cache Expired
            }
        }
    }
    return 0; // Cache Miss
}

// Store resolved entry into local cache
void add_to_cache(const char *domain, const char *ip) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (!cache[i].is_valid || strcasecmp(cache[i].domain, domain) == 0) {
            strcpy(cache[i].domain, domain);
            strcpy(cache[i].ip, ip);
            cache[i].timestamp = time(NULL);
            cache[i].is_valid = 1;
            return;
        }
    }
    // If full, overwrite the first entry (FIFO)
    strcpy(cache[0].domain, domain);
    strcpy(cache[0].ip, ip);
    cache[0].timestamp = time(NULL);
    cache[0].is_valid = 1;
}

// Client-side domain format validation
int validate_format(const char *domain) {
    if (strlen(domain) < 3) return 0;
    if (strchr(domain, ' ') != NULL) return 0; // No spaces
    if (strchr(domain, '.') == NULL) return 0; // Must have a dot
    return 1;
}

// Print analysis of UDP suitability for DNS
void print_udp_analysis() {
    printf("\n--------------------------------------------------\n");
    printf("   ANALYSIS: SUITABILITY OF UDP FOR DNS SERVICES  \n");
    printf("--------------------------------------------------\n");
    printf("1. Communication Overhead : Minimal. UDP header is only 8 bytes\n");
    printf("                            (compared to 20-60 bytes for TCP).\n");
    printf("                            No connection setup (SYN/ACK) packets.\n");
    printf("2. Latency                : Ultra-low (1 RTT).\n");
    printf("                            A single request packet and response packet.\n");
    printf("3. Reliability            : Handled at Application Layer.\n");
    printf("                            Client uses timeout & retransmission.\n");
    printf("                            (Fallback to TCP only if response > 512B).\n");
    printf("--------------------------------------------------\n");
}

int main() {
    init_cache();

    // 1. Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Configure 2-second timeout on socket (Timeout Simulation)
    struct timeval tv;
    tv.tv_sec = 2; // 2 seconds timeout
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error setting socket timeout");
    }

    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    printf("===========================================\n");
    printf("              UDP DNS CLIENT               \n");
    printf("===========================================\n");
    printf("Connected to DNS Server : %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Timeout Duration        : 2.0 seconds\n");
    printf("Client Cache TTL        : %d seconds\n", CACHE_TTL);
    printf("Type 'exit' to quit | 'analysis' for UDP report\n");
    printf("===========================================\n\n");

    char domain[100], buffer[BUFFER_SIZE], cached_ip[50];

    while (1) {
        printf("Enter Domain Name: ");
        if (scanf("%99s", domain) != 1) break;

        if (strcmp(domain, "exit") == 0) break;
        if (strcmp(domain, "analysis") == 0) {
            print_udp_analysis();
            continue;
        }

        // Error Handling: Invalid Format Check
        if (!validate_format(domain)) {
            printf("[ERROR] Invalid domain format! Example: 'google.com'\n\n");
            continue;
        }

        // Step 1: Check Client-side Cache
        if (check_cache(domain, cached_ip)) {
            printf("[CACHE HIT] Resolved from local cache: %s -> %s (Latency: 0.00 ms)\n\n", domain, cached_ip);
            continue;
        }

        // Step 2: Cache Miss -> Send UDP Query to DNS Server
        printf("[CACHE MISS] Querying DNS Server over UDP...\n");

        struct timespec t_start, t_end;
        clock_gettime(CLOCK_MONOTONIC, &t_start);

        sendto(sock, domain, strlen(domain), 0, (struct sockaddr *)&serv_addr, len);

        memset(buffer, 0, sizeof(buffer));
        int bytes = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&serv_addr, &len);

        clock_gettime(CLOCK_MONOTONIC, &t_end);

        // Step 3: Handle Timeout Error
        if (bytes < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("[ERROR] Request Timed Out! DNS Server did not respond within 2.0s.\n\n");
            } else {
                perror("[ERROR] Receive error");
            }
            continue;
        }

        buffer[bytes] = '\0';
        double latency = (t_end.tv_sec - t_start.tv_sec) * 1000.0 +
                         (t_end.tv_nsec - t_start.tv_nsec) / 1000000.0;

        // Step 4: Handle NXDOMAIN or Success
        if (strcmp(buffer, "NXDOMAIN") == 0) {
            printf("[ERROR] NXDOMAIN: Domain '%s' not found in DNS database.\n", domain);
            printf("        Lookup Latency : %.2f ms\n\n", latency);
        } else if (strcmp(buffer, "INVALID_FORMAT") == 0) {
            printf("[ERROR] DNS Server rejected format for '%s'.\n\n", domain);
        } else {
            // Success: Store into Cache & Display Result
            add_to_cache(domain, buffer);
            printf("[SUCCESS] %s -> %s\n", domain, buffer);
            printf("          Lookup Latency : %.2f ms\n", latency);
            printf("          Cached         : Yes (TTL %ds)\n\n", CACHE_TTL);
        }
    }

    close(sock);
    return 0;
}
