#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#include "http_client.h"

#define MAX_QUEUE_SIZE 10

typedef struct sockaddr_in SocketAddrIn;

typedef struct sockaddr SocketAddr;

int main(int argc, char *argv[]) {

    int server_fd = -1, new_socket = -1, opt = 1, bind_port = 0;
    SocketAddrIn *pAddress = NULL;
    char *bind_ip = NULL;

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

    while (1)
    {
        // se aceptan las conexiones cuando entran
        printf("[MAIN] Servidor Escuchando %s:%d\n", bind_ip, bind_port );
        if ((new_socket = accept(server_fd, (SocketAddr *)pAddress, &addrlen)) == -1)
        {
            perror("accept() recibio un error, se ignorará");
            continue;
        }
        else
        {            
            Data_Client *pData = (Data_Client *)malloc(sizeof(Data_Client));
            memset(pData, 0, sizeof(Data_Client));
            pData->server_socket = server_fd;
            pData->client_socket = new_socket;
            pData->connection_port = ntohs(pAddress->sin_port); 
            char * client_ip = (char *)&inet_ntoa(pAddress->sin_addr)[0];
            memcpy(pData->client_host, client_ip, strlen(client_ip));
            pthread_t thread_process;
            if (pthread_create(&thread_process, NULL, process_data, pData) < 0)
            {
                perror(" [MAIN]pthread_create() failed (client to target)");
                close(pData->client_socket);
                free(pData);
            }
            // se le da independencia al hilo
            if ( 0 != pthread_detach(thread_process)){
                perror(" [MAIN]pthread_detach() failed (client to target)");
                close(pData->client_socket);
                free(pData);
            }
        }
    }
    close(server_fd);
    free(pAddress);

    return EXIT_SUCCESS;
}
