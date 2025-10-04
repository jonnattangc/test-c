#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define ADDR_LEN 16
#define MAX_QUEUE_SIZE 10

typedef struct sockaddr_in SocketAddrIn;

typedef struct sockaddr SocketAddr;

int main(int argc, char *argv[]) {

    int server_fd = -1, new_socket = -1, opt = 1, bind_port = 0;
    SocketAddrIn *pAddress = NULL;
    char *bind_ip = NULL;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = 0, bytes_tx = 0;

    if (argc < 3)
    {
        printf("Utilizar el programa con 2 argumentos: addr, port\n");
        return 0;
    }

    bind_ip = (char *)&argv[1][0];
    bind_port  = atoi((char *)&argv[2][0]);

    socklen_t addrlen = sizeof(SocketAddrIn);
    pAddress = (SocketAddrIn *)malloc(addrlen);
    memset(pAddress, 0, addrlen);

    // se crea el socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("No se puede crear el socket");
        return EXIT_FAILURE;
    }
    // permitir reutilizaci'on del socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
    {
        perror("setsockopt(SO_REUSEADDR) failed in main");
        close(server_fd);
        return EXIT_FAILURE;
    }
#ifdef __APPLE__
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int)) == -1)
    {
        perror("setsockopt(SO_REUSEPORT) failed in main (macOS)");
        close(server_fd);
        return EXIT_FAILURE;
    }
#else
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int)) == -1)
    {
        perror("setsockopt(SO_REUSEPORT) failed in main");
        close(server_fd);
        return EXIT_FAILURE;
    }
#endif
    pAddress->sin_family = AF_INET;
    pAddress->sin_addr.s_addr = INADDR_ANY;
    pAddress->sin_port = htons(bind_port);

    if (bind(server_fd, (SocketAddr *)pAddress, sizeof(SocketAddrIn)) < 0)
    {
        perror("bind() failed in main");
        close(server_fd);
        return EXIT_FAILURE;
    }
    // lo marca como escuchando (que acepte conexiones)
    if (listen(server_fd, MAX_QUEUE_SIZE) < 0)
    {
        perror("listen() failed in main");
        close(server_fd);
        return EXIT_FAILURE;
    }

    char * pDataTx = ( char *) malloc( BUFFER_SIZE );

    while (1)
    {
        // se aceptan las conexiones cuando entran
        printf("[0] Escuchando %s:%d\n", bind_ip, bind_port );
        if ((new_socket = accept(server_fd, (SocketAddr *)pAddress, &addrlen)) == -1)
        {
            perror("accept() recibio un error, se ignorará");
            continue;
        }
        else
        {
            char * client_ip = (char *)&inet_ntoa(pAddress->sin_addr)[0];
            printf("[+] Conexión TCP aceptada de %s:%d\n", client_ip, ntohs(pAddress->sin_port));
            while ((bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0) {
                printf("[+] %s\n",  buffer );
                printf("[+] Rx to %d bytes\n", (unsigned int) bytes_received );
                memset(pDataTx, 0, BUFFER_SIZE );
                const char *html_body = "{\"texto\": \"Mensaje de Prueba\"}";
                // 1. Crear la Línea de Estado y las Cabeceras
                snprintf(pDataTx, BUFFER_SIZE,
                    "HTTP/1.0 200 OK\r\n"
                    "Content-Type: application/json\r\n"
                    "Content-Length: %zu\r\n"
                    "Connection: close\r\n"
                    "\r\n" // Línea en blanco esencial para separar cabeceras del cuerpo
                    "%s",
                    strlen(html_body), // Usamos strlen para Content-Length + 10 bytes
                    html_body);

                if ((bytes_tx = send(new_socket, pDataTx, strlen(pDataTx), 0)) == -1) {
                    perror("  [+] send() failed in forward_data");
                    break;
                }else {
                    printf("[+] Tx to %d bytes\n", (unsigned int) bytes_tx );
                    // sleep(50);
                }
            }

            if (bytes_received == -1 && errno != EINTR) {
                perror("  [+] recv() failed in forward_data");
            }

            shutdown(new_socket, SHUT_RDWR);
            close(new_socket);
        }
    }
    close(server_fd);
    free(pAddress);
    free(pDataTx);

    return 0;
}
