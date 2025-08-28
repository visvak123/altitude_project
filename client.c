// client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define TOTAL_POINTS 60

int received_data[TOTAL_POINTS];
int count = 0;
int anomalies = 0;
pthread_mutex_t lock;

void *receive_data(void *arg) {
    int sock = *((int *)arg);
    char buffer[1024];
    int valread;

    while ((valread = read(sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[valread] = '\0';
        char *token = strtok(buffer, "\n");
        while (token != NULL) {
            pthread_mutex_lock(&lock);
            received_data[count++] = atoi(token);
            pthread_mutex_unlock(&lock);
            token = strtok(NULL, "\n");
        }
    }
    return NULL;
}

void *validate_data(void *arg) {
    int last_alt = -1;
    while (count < TOTAL_POINTS) {
        pthread_mutex_lock(&lock);
        if (count > 0) {
            int altitude = received_data[count - 1];
            if (last_alt != -1 && abs(altitude - last_alt) > 100) {
                printf("WARNING: Anomaly detected! %d -> %d\n", last_alt, altitude);
                anomalies++;
            }
            printf("Received: %d ft\n", altitude);
            last_alt = altitude;
        }
        pthread_mutex_unlock(&lock);
        usleep(100000); // 0.1s delay
    }
    return NULL;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    pthread_mutex_init(&lock, NULL);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    pthread_t recv_thread, val_thread;
    pthread_create(&recv_thread, NULL, receive_data, (void *)&sock);
    pthread_create(&val_thread, NULL, validate_data, NULL);

    pthread_join(recv_thread, NULL);
    pthread_join(val_thread, NULL);

    // Export to CSV
    FILE *f = fopen("altitude_data.csv", "w");
    if (f) {
        fprintf(f, "Index,Altitude\n");
        for (int i = 0; i < count; i++) {
            fprintf(f, "%d,%d\n", i + 1, received_data[i]);
        }
        fclose(f);
        printf("\nData exported to altitude_data.csv ✅\n");
    }

    printf("\n--- Summary ---\n");
    printf("Total Data Points: %d\n", count);
    printf("Anomalies Detected: %d\n", anomalies);

    close(sock);
    pthread_mutex_destroy(&lock);
    return 0;
}
