#include "http_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#include "http_utils.h"

void * process_data( void *pArg ) {  
    char buffer_rx[BUFFER_SIZE], buffer_tx[BUFFER_SIZE];
    ssize_t bytes_received = 0, bytes_tx = 0; 
    
    Data_Client *pData = (Data_Client *)pArg;
    pthread_t tid = pthread_self();
    printf("  [HILO] Conexión TCP aceptada de IP:%s Puerto:%d Hilo:%lu\n", pData->client_host, pData->connection_port, (unsigned long) tid);

    while ((bytes_received = recv(pData->client_socket, buffer_rx, BUFFER_SIZE, 0)) > 0) {
        printf("  [HILO] Metodo: %s\n",  get_http_method(buffer_rx) );

        printf("  [HILO] Rx to %d bytes\n%s", (unsigned int) bytes_received, buffer_rx );
        memset(&buffer_tx, 0, BUFFER_SIZE );
        const char *html_body = "{\"texto\": \"Mensaje de Prueba\"}";
        // 1. Crear la Línea de Estado y las Cabeceras
        snprintf(buffer_tx, BUFFER_SIZE,
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "Connection: close\r\n"
            "\r\n" // Línea en blanco esencial para separar cabeceras del cuerpo
            "%s",
            strlen(html_body), // Usamos strlen para Content-Length + 10 bytes
            html_body);

        if ((bytes_tx = send(pData->client_socket, buffer_tx, strlen(buffer_tx), 0)) == -1) {
            perror("  [HILO] send() failed in forward_data");
            break;

        }else {
            printf("  [HILO] Tx to %d bytes\n", (unsigned int) bytes_tx );
            // sleep(50);
        }
    }

    if (bytes_received == -1 && errno != EINTR) {
        perror("  [+] recv() failed in forward_data");
    }

    shutdown(pData->client_socket, SHUT_RDWR);
    close(pData->client_socket);
    free(pData);

    return NULL;

}
