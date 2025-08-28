// server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define TOTAL_POINTS 60

int altitude_data[TOTAL_POINTS];  // store generated values
int data_ready = 0;               // flag for synchronization
pthread_mutex_t lock;

void *generate_data(void *arg) {
    srand(time(NULL));
    int altitude = 1000;
    for (int i = 0; i < TOTAL_POINTS; i++) {
        altitude += 5;  
        altitude += (rand() % 21 - 10); // turbulence (-10 to +10)

        pthread_mutex_lock(&lock);
        altitude_data[i] = altitude;
        data_ready = 1;
        pthread_mutex_unlock(&lock);

        sleep(1); // generate once per second
    }
    return NULL;
}

void *send_data(void *arg) {
    int new_socket = *((int *)arg);
    for (int i = 0; i < TOTAL_POINTS; i++) {
        while (!data_ready) usleep(1000); // wait until data is ready

        pthread_mutex_lock(&lock);
        char buffer[32];
        sprintf(buffer, "%d\n", altitude_data[i]);
        data_ready = 0;
        pthread_mutex_unlock(&lock);

        send(new_socket, buffer, strlen(buffer), 0);
        printf("Sent: %s", buffer);
    }
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    pthread_mutex_init(&lock, NULL);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, 3);
    printf("Server waiting for connection...\n");

    new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    pthread_t gen_thread, send_thread;
    pthread_create(&gen_thread, NULL, generate_data, NULL);
    pthread_create(&send_thread, NULL, send_data, (void *)&new_socket);

    pthread_join(gen_thread, NULL);
    pthread_join(send_thread, NULL);

    close(new_socket);
    close(server_fd);
    pthread_mutex_destroy(&lock);
    return 0;
}


