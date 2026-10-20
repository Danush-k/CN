#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096

int main() {
    char url[256], host[100], path[100] = "", filename[120];
    int port = 80;

    // 1. Get URL from user
    printf("Enter URL: ");
    if (scanf("%255s", url) != 1) return 1;

    // 2. Parse URL (e.g. http://localhost:8000/sample.pdf)
    if (sscanf(url, "http://%[^:/]:%d/%s", host, &port, path) != 3) {
        if (sscanf(url, "http://%[^/]/%s", host, path) != 2) {
            sscanf(url, "http://%[^:/]:%d", host, &port);
        }
    }
    if (strlen(path) == 0) strcpy(path, "index.html");

    // Generate output filename (e.g. downloaded_sample.pdf)
    sprintf(filename, "downloaded_%s", path);

    printf("\nParsed URL Details:\n");
    printf("  Host : %s\n  Port : %d\n  Path : /%s\n", host, port, path);

    // 3. Create TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Resolve hostname (works for 'localhost' or domain names)
    struct hostent *he = gethostbyname(host);
    if (!he) {
        printf("DNS lookup failed for %s\n", host);
        close(sock);
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr, he->h_addr_list[0], he->h_length);

    // Connect to Web Server & measure timings
    struct timespec t_start, t_first, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    // 4. Send HTTP GET Request
    char request[512];
    sprintf(request, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, host);
    write(sock, request, strlen(request));

    // 5. Receive HTTP Response and save file
    char buffer[BUFFER_SIZE];
    int bytes = 0, first_chunk = 1;
    long total_bytes = 0;
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("File creation failed");
        close(sock);
        return 1;
    }

    while ((bytes = read(sock, buffer, sizeof(buffer))) > 0) {
        if (first_chunk) {
            clock_gettime(CLOCK_MONOTONIC, &t_first);
            first_chunk = 0;

            // Separate HTTP Header from Body
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                body += 4; // Skip \r\n\r\n
                int body_len = bytes - (body - buffer);
                fwrite(body, 1, body_len, fp);
            }
        } else {
            fwrite(buffer, 1, bytes, fp);
        }
        total_bytes += bytes;
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    fclose(fp);
    close(sock);

    // 6. Calculate & Print Performance Analysis
    double resp_time = (t_first.tv_sec - t_start.tv_sec) * 1000.0 +
                       (t_first.tv_nsec - t_start.tv_nsec) / 1000000.0;
    double down_time = (t_end.tv_sec - t_first.tv_sec) * 1000.0 +
                       (t_end.tv_nsec - t_first.tv_nsec) / 1000000.0;
    double total_time_sec = (t_end.tv_sec - t_start.tv_sec) +
                           (t_end.tv_nsec - t_start.tv_nsec) / 1000000000.0;
    if (total_time_sec <= 0) total_time_sec = 0.0001;
    double throughput = (total_bytes / 1024.0) / total_time_sec;

    printf("\nFile saved as: %s\n", filename);
    printf("========================================\n");
    printf("         PERFORMANCE ANALYSIS           \n");
    printf("========================================\n");
    printf("Response Time (TTFB) : %.2f ms\n", resp_time);
    printf("Download Time        : %.2f ms\n", down_time);
    printf("Total Data Size      : %ld bytes (%.2f KB)\n", total_bytes, total_bytes / 1024.0);
    printf("Throughput           : %.2f KB/s\n", throughput);
    printf("========================================\n");

    return 0;
}
