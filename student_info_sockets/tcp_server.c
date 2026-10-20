#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080

// Thread function to handle each connected client
void *handle_client(void *arg) {
    int sock = *(int *)arg;
    free(arg);

    char roll[20], buffer[1024];
    memset(roll, 0, sizeof(roll));

    // 1. Receive Roll Number from client
    read(sock, roll, sizeof(roll) - 1);
    roll[strcspn(roll, "\r\n")] = '\0'; // remove newline

    // 2. Search in students.csv
    FILE *fp = fopen("students.csv", "r");
    char line[200], r[20], name[50], dept[50], sem[10], cgpa[10];
    int found = 0;

    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            sscanf(line, "%[^,],%[^,],%[^,],%[^,],%s", r, name, dept, sem, cgpa);
            if (strcmp(r, roll) == 0) {
                found = 1;
                sprintf(buffer, "\nRoll Number: %s\nName: %s\nDepartment: %s\nSemester: %s\nCGPA: %s\n",
                        r, name, dept, sem, cgpa);
                break;
            }
        }
        fclose(fp);
    }

    // 3. Send record or error message
    if (!found) {
        strcpy(buffer, "Student Record Not Found\n");
    }

    write(sock, buffer, strlen(buffer));

    // 4. Close connection
    close(sock);
    return NULL;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;

    // Allow quick restart of server
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    listen(server_fd, 5);

    printf("TCP Server listening on port %d...\n", PORT);

    // Accept multiple clients concurrently using pthreads
    while (1) {
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_fd, NULL, NULL);

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_sock);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
