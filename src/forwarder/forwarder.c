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

typedef struct
{
    int client_socket;
    char client_host[ADDR_LEN];
    int target_port;
    char target_host[ADDR_LEN];
} ForwarderArgs;
typedef struct
{
    int socket_tx;
    int socket_rx;
    char suffix[ADDR_LEN];
}  FwData;

typedef struct sockaddr_in SocketAddrIn;

typedef struct sockaddr SocketAddr;


/**
 * @brief Funcion que se encarga de forwarder entre un cliente y un servidor.
 *        El argumento debe ser un puntero a una estructura FwData con los
 *        sockets y sufijo para la informacion de logs.
 * @param pArg puntero a una estructura FwData.
 * @return NULL.
 */
void * forward_data( void *pArg ) {   
    FwData *fwData = (FwData *)pArg;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    pthread_t tid = pthread_self();
    printf("  [+] Hilo Fw %lu iniciado... \n", (unsigned long) tid );

    while ((bytes_received = recv(fwData->socket_rx, buffer, BUFFER_SIZE, 0)) > 0) {
        printf("  [+] Forwarding to %s %d bytes\n", fwData->suffix, (unsigned int) bytes_received );
        if (send(fwData->socket_tx, buffer, bytes_received, 0) == -1) {
            perror("  [+] send() failed in forward_data");
            break;
        }
    }

    if (bytes_received == -1 && errno != EINTR) {
        perror("  [+] recv() failed in forward_data");
    }

    shutdown(fwData->socket_rx, SHUT_RDWR);
    close(fwData->socket_rx);
    shutdown(fwData->socket_tx, SHUT_RDWR);
    close(fwData->socket_tx);
    free(fwData);
    
    printf("  [+] Hilo Fw %lu terminado... \n", (unsigned long) tid );
    return NULL;
}


/**
 * Crea los elementos necesarios para forwarder entre un cliente y un destino de paquetes.
 *
 * Esta función crea un socket para conectar con el destino de los paquetes y crea dos
 * threads para forwarder los datos entre el cliente y el destino de los paquetes.
 *
 * Al terminar, libera la memoria utilizada y devuelve NULL.
 *
 * @param args Un puntero a una estructura de tipo ForwarderArgs que contiene
 *             la información necesaria para crear el forwarder.
 *
 * @return NULL.
 *
 * @throws No exceptions are thrown.
 */
void *procesa_fw(void *pAgr)
{
    ForwarderArgs *pFargs = (ForwarderArgs *)pAgr;
    int target_socket = -1;

    pthread_t tid = pthread_self();
    printf(" [-] Hilo Process %lu iniciado... \n", (unsigned long) tid );

    // se crea el socket como escuchador
    if ((target_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror(" [-]socket() failed in handle_client");
        close(pFargs->client_socket);
        free(pFargs);
        return NULL;
    }
    // crea conexion
    SocketAddrIn *pTarget = malloc(sizeof(SocketAddrIn));
    pTarget->sin_family = AF_INET;
    pTarget->sin_port = htons(pFargs->target_port);
    
    if (inet_pton(AF_INET, pFargs->target_host, &pTarget->sin_addr) <= 0)
    {
        perror(" [-]inet_pton() failed for target host in handle_client");
        close(pFargs->client_socket);
        close(target_socket);
        free(pFargs);
        return NULL;
    }
    // Conecta con destino de los paquetes
    if (connect(target_socket, (SocketAddr *)pTarget, sizeof(SocketAddr)) == -1)
    {
        perror(" [-]connect() failed in handle_client");
        close(pFargs->client_socket);
        close(target_socket);
        free(pFargs);
        return NULL;
    }

    printf(" [-] Conexión establecida con %s:%d para forwarder de datos\n", pFargs->target_host, pFargs->target_port);

    FwData *pFwData1 = malloc(sizeof(FwData));
    FwData *pFwData2 = malloc(sizeof(FwData));

    pFwData1->socket_rx = pFargs->client_socket;
    pFwData1->socket_tx = target_socket;
    sprintf(pFwData1->suffix, "%s", pFargs->target_host);

    pFwData2->socket_rx = target_socket;
    pFwData2->socket_tx = pFargs->client_socket;
    sprintf(pFwData2->suffix, "%s", pFargs->client_host);

    pthread_t thread_client_to_target, thread_target_to_client;
    if (pthread_create(&thread_client_to_target, NULL, forward_data, pFwData1) < 0)
    {
        perror(" [-]pthread_create() failed (client to target)");
        close(pFargs->client_socket);
        close(target_socket);
        free(pFargs);
        free(pFwData1);
        free(pFwData2);
        return NULL;
    }

    if (pthread_create(&thread_target_to_client, NULL, forward_data, pFwData2) < 0)
    {
        perror(" [-]pthread_create() failed (target to client)");
        pthread_cancel(thread_client_to_target);
        close(pFargs->client_socket);
        close(target_socket);
        free(pFargs);
        free(pFwData1);
        free(pFwData2);
        return NULL;
    }

    pthread_detach(thread_client_to_target);
    pthread_detach(thread_target_to_client);

    free(pFargs);
    pFargs = NULL;
    printf(" [-] Hilo Process %lu terminado... \n", (unsigned long) tid );
    return NULL;
}

/**
 * @brief Main entry point of the program.
 *
 * The program takes four parameters:
 *   1. addr_listen: The address to listen on.
 *   2. port_listen: The port to listen on.
 *   3. addr_dst: The address of the target host.
 *   4. port_dst: The port of the target host.
 *
 * The program will listen on the given address and port, and forward any incoming
 * connections to the target host and port.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @return 0 on success, 1 on failure.
 */
int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("Utilizar el programa con 4 argumentos: addr_listen, port_listen, addr_dst, port_dst\n");
        return 0;
    }

    char *bind_ip = (char *)&argv[1][0];
    int bind_port = atoi((char *)&argv[2][0]);
    char *target_host = (char *)&argv[3][0];
    int target_port = atoi((char *)&argv[4][0]);

    int server_fd = -1, new_socket = -1;

    SocketAddrIn *pAddress = NULL;
    // Se limpia el elemento principal
    socklen_t addrlen = sizeof(SocketAddrIn);
    pAddress = (SocketAddrIn *)malloc(addrlen);
    memset(pAddress, 0, addrlen);

    int opt = 1;

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

    printf("[*] Escuchando en %s:%d y ", bind_ip, bind_port);
    printf("reenviando tráfico a %s:%d\n", target_host, target_port);

    while (1)
    {
        // se aceptan las conexiones cuando entran
        if ((new_socket = accept(server_fd, (SocketAddr *)pAddress, &addrlen)) == -1)
        {
            perror("accept() recibio un error, se ignorará");
            continue;
        }
        else
        {
            char * client_ip = (char *)&inet_ntoa(pAddress->sin_addr)[0];
            printf("[+] Conexión TCP aceptada de %s:%d\n", client_ip, ntohs(pAddress->sin_port));
            
            ForwarderArgs *pArgs = malloc(sizeof(ForwarderArgs));
            memset(pArgs, 0, sizeof(ForwarderArgs));
            memcpy(pArgs->target_host, target_host, strlen(target_host) );
            memcpy(pArgs->client_host, client_ip, strlen(client_ip) );
            pArgs->client_socket = new_socket;
            pArgs->target_port = target_port;
            
            // atributos del hilo
            pthread_attr_t attr;
            memset(&attr, 0,  sizeof(pthread_attr_t));
            if( pthread_attr_init(&attr) != 0 )
            {
                perror("Error al crear los atributos del hilo, se cierra programa");
                close(new_socket);
                free(pArgs);
                return EXIT_FAILURE;
            }
            // establece el estado de detach
            if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
                perror("Error al establecer el atributo de detach, se cierra programa");
                close(new_socket);
                pthread_attr_destroy(&attr);
                free(pArgs);
                return EXIT_FAILURE;
            }
            // Crea el hilo y lo deja en background
            pthread_t thread_id;
            memset(&thread_id, 0, sizeof(pthread_t));
            if (pthread_create(&thread_id, &attr, procesa_fw, pArgs) < 0)
            {
                perror("Error al crear el hilo, se cierra programa");
                close(new_socket);
                pthread_attr_destroy(&attr);
                free(pArgs);
                return EXIT_FAILURE;
            }
            else
                pthread_detach(thread_id);
        }
    }

    close(server_fd);
    free(pAddress);
    return 0;
}
