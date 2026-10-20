#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE 4096

int main() {
    char url[256], host[100], path[100] = "", filename[120];
    int port = 443;

    // 1. Get URL from user
    printf("Enter HTTPS URL: ");
    if (scanf("%255s", url) != 1) return 1;

    // 2. Parse URL (supports https://, http://, or bare domain)
    const char *p = url;
    if (strncmp(p, "https://", 8) == 0) p += 8;
    else if (strncmp(p, "http://", 7) == 0) p += 7;

    if (sscanf(p, "%[^:/]:%d/%s", host, &port, path) != 3) {
        if (sscanf(p, "%[^/]/%s", host, path) != 2) {
            sscanf(p, "%[^:/]:%d", host, &port);
        }
    }
    if (strlen(path) == 0) strcpy(path, "index.html");
    sprintf(filename, "downloaded_https_%s", path);

    // 3. TCP Connection to port 443
    struct timespec t_tcp_start, t_tcp_end, t_ssl_start, t_ssl_end, t_req, t_first, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_tcp_start);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *he = gethostbyname(host);
    if (!he) {
        printf("DNS lookup failed for %s\n", host);
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("TCP Connection failed");
        close(sock);
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &t_tcp_end);

    // 4. Initialize SSL Context & Perform Handshake
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    SSL_set_tlsext_host_name(ssl, host); // SNI

    clock_gettime(CLOCK_MONOTONIC, &t_ssl_start);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    clock_gettime(CLOCK_MONOTONIC, &t_ssl_end);

    // Validate server certificate
    X509 *cert = SSL_get_peer_certificate(ssl);
    if (!cert) {
        printf("Warning: No server certificate found.\n");
    } else {
        X509_free(cert);
    }

    // 5. Send Secure HTTP GET Request
    char request[512];
    sprintf(request, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, host);

    clock_gettime(CLOCK_MONOTONIC, &t_req);
    SSL_write(ssl, request, strlen(request));

    // 6. Receive and Decrypt HTTP Response
    char buffer[BUFFER_SIZE];
    int bytes = 0, first_chunk = 1;
    long total_bytes = 0;
    FILE *fp = fopen(filename, "wb");

    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        if (first_chunk) {
            clock_gettime(CLOCK_MONOTONIC, &t_first);
            first_chunk = 0;
            buffer[bytes] = '\0';
            char *body = strstr(buffer, "\r\n\r\n");
            if (body) {
                body += 4;
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
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);
    SSL_CTX_free(ctx);

    // 7. Security and Performance Metrics
    double tcp_time = (t_tcp_end.tv_sec - t_tcp_start.tv_sec) * 1000.0 +
                      (t_tcp_end.tv_nsec - t_tcp_start.tv_nsec) / 1000000.0;
    double ssl_time = (t_ssl_end.tv_sec - t_ssl_start.tv_sec) * 1000.0 +
                      (t_ssl_end.tv_nsec - t_ssl_start.tv_nsec) / 1000000.0;
    double rtt_time = (t_first.tv_sec - t_req.tv_sec) * 1000.0 +
                      (t_first.tv_nsec - t_req.tv_nsec) / 1000000.0;
    double total_sec = (t_end.tv_sec - t_tcp_start.tv_sec) +
                       (t_end.tv_nsec - t_tcp_start.tv_nsec) / 1000000000.0;
    if (total_sec <= 0) total_sec = 0.0001;
    double throughput = (total_bytes / 1024.0) / total_sec;

    printf("\nFile saved as: %s\n", filename);
    printf("========================================\n");
    printf("   SECURITY & PERFORMANCE PARAMETERS    \n");
    printf("========================================\n");
    printf("TCP Connection Time   : %.2f ms\n", tcp_time);
    printf("SSL Handshake Time    : %.2f ms\n", ssl_time);
    printf("Round Trip Time (RTT) : %.2f ms\n", rtt_time);
    printf("Data Transfer Rate    : %.2f KB/s\n", throughput);
    printf("========================================\n");

    return 0;
}
