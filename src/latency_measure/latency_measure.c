#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h> // For gettimeofday

#ifdef __APPLE__
    #include <errno.h>
#endif

#define TIMEOUT_SEC 5 // Timeout for connection in seconds

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <direccion_ip> <puerto>\n", argv[0]);
        return 1;
    }

    const char *ip_address = argv[1];
    int port = atoi(argv[2]);

    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Error: El puerto debe ser un número entre 1 y 65535.\n");
        return 1;
    }
#ifdef __APPLE__
    printf("Detectado macOS.\n");
#elif __linux__
    printf("Detectado Linux.\n");
#else
    printf("Sistema operativo desconocido.\n");
#endif
    int sockfd;
    struct sockaddr_in serv_addr;
    struct timeval start_time, end_time;
    long latency_us; // Latency in microseconds

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    // Set up server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IP address from string to network format
    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        perror("Dirección IP inválida/no soportada");
        close(sockfd);
        return 1;
    }

    // Set a timeout for the connection attempt
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);

    printf("Intentando conectar a %s:%d...\n", ip_address, port);

    // Get start time
    gettimeofday(&start_time, NULL);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        // Check for timeout error specifically
        if (errno == EINPROGRESS || errno == EWOULDBLOCK || errno == ETIMEDOUT) {
            fprintf(stderr, "Error: Conexión a %s:%d timed out después de %d segundos.\n", ip_address, port, TIMEOUT_SEC);
        } else {
            perror("Error al conectar");
        }
        close(sockfd);
        return 1;
    }

    // Get end time
    gettimeofday(&end_time, NULL);

    // Calculate latency in microseconds
    latency_us = (end_time.tv_sec - start_time.tv_sec) * 1000000L +
                 (end_time.tv_usec - start_time.tv_usec);

    printf("Conexión exitosa a %s:%d\n", ip_address, port);
    printf("Latencia de conexión: %ld microsegundos (%.3f ms)\n", latency_us, (double)latency_us / 1000.0);

    close(sockfd);

    return 0;
}
