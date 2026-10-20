#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t len = sizeof(cli_addr);
    char roll[20], buffer[1024];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        close(sock);
        return 1;
    }
    printf("UDP Server listening on port %d...\n", PORT);

    while (1) {
        memset(roll, 0, sizeof(roll));

        // 1. Receive Roll Number from client
        recvfrom(sock, roll, sizeof(roll) - 1, 0, (struct sockaddr *)&cli_addr, &len);
        roll[strcspn(roll, "\r\n")] = '\0';

        // 2. Search in students.csv
        FILE *fp = fopen("students.csv", "r");
        char line[200], r[20], name[50], dept[50], sem[10], cgpa[10];
        int found = 0;

        if (fp) {
            while (fgets(line, sizeof(line), fp)) {
                sscanf(line, "%[^,],%[^,],%[^,],%[^,],%s", r, name, dept, sem, cgpa);
                if (strcmp(r, roll) == 0) {
                    found = 1;
                    // Send back ONLY Roll Number and Student Name
                    sprintf(buffer, "\nRoll Number: %s\nName: %s\n", r, name);
                    break;
                }
            }
            fclose(fp);
        }

        // 3. Send record or error message
        if (!found) {
            strcpy(buffer, "Student Record Not Found\n");
        }

        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&cli_addr, len);
    }

    close(sock);
    return 0;
}
