#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>

#define BUFFER_SIZE 4096

struct ForwarderArgs
{
    int client_socket;
    char *target_host;
    int target_port;
};

/**
 * Forward data from a source socket to a destination socket, until there
 * is no more data to forward.
 *
 * @param arg A pointer to an array of two ints, where arg[0] is the source
 * socket and arg[1] is the destination socket.
 *
 * @return NULL.
 *
 * @throws No exceptions are thrown.
 */
void *forward_data(void *arg)
{
    int source_fd = ((int *)arg)[0];
    int dest_fd = ((int *)arg)[1];
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(source_fd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        if (send(dest_fd, buffer, bytes_received, 0) == -1)
        {
            perror("send() failed in forward_data");
            break;
        }
    }

    if (bytes_received == -1 && errno != EINTR)
    {
        perror("recv() failed in forward_data");
    }

    shutdown(source_fd, SHUT_RDWR);
    close(source_fd);
    shutdown(dest_fd, SHUT_RDWR);
    close(dest_fd);
    free(arg);
    return NULL;
}

/**
 * Handle an incoming client connection by establishing a connection to the
 * target host and port, and then forwarding data between the client and target
 * connections until either of them is closed.
 *
 * @param args A pointer to a ForwarderArgs struct containing the client socket,
 * target host and target port.
 *
 * @return NULL.
 *
 * @throws No exceptions are thrown.
 */
void *handle_client(void *args)
{
    struct ForwarderArgs *f_args = (struct ForwarderArgs *)args;
    int client_socket = f_args->client_socket;
    char *target_host = f_args->target_host;
    int target_port = f_args->target_port;
    int target_socket;

    if ((target_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() failed in handle_client");
        close(client_socket);
        free(f_args->target_host);
        free(f_args);
        return NULL;
    }

    struct sockaddr_in target_addr;
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(target_port);
    if (inet_pton(AF_INET, target_host, &target_addr.sin_addr) <= 0)
    {
        perror("inet_pton() failed for target host in handle_client");
        close(client_socket);
        close(target_socket);
        free(f_args->target_host);
        free(f_args);
        return NULL;
    }

    if (connect(target_socket, (struct sockaddr *)&target_addr, sizeof(target_addr)) == -1)
    {
        perror("connect() failed in handle_client");
        close(client_socket);
        close(target_socket);
        free(f_args->target_host);
        free(f_args);
        return NULL;
    }

    printf("[+] Conexión establecida con %s:%d para el cliente %d\n", target_host, target_port, client_socket);

    pthread_t thread_client_to_target, thread_target_to_client;
    int *sockets1 = malloc(sizeof(int) * 2);
    int *sockets2 = malloc(sizeof(int) * 2);

    if (!sockets1 || !sockets2)
    {
        perror("malloc() failed for sockets");
        close(client_socket);
        close(target_socket);
        free(f_args->target_host);
        free(f_args);
        free(sockets1);
        free(sockets2);
        return NULL;
    }

    sockets1[0] = client_socket;
    sockets1[1] = target_socket;
    sockets2[0] = target_socket;
    sockets2[1] = client_socket;

    if (pthread_create(&thread_client_to_target, NULL, forward_data, (void *)sockets1) < 0)
    {
        perror("pthread_create() failed (client to target)");
        close(client_socket);
        close(target_socket);
        free(f_args->target_host);
        free(f_args);
        free(sockets1);
        free(sockets2);
        return NULL;
    }

    if (pthread_create(&thread_target_to_client, NULL, forward_data, (void *)sockets2) < 0)
    {
        perror("pthread_create() failed (target to client)");
        pthread_cancel(thread_client_to_target);
        close(client_socket);
        close(target_socket);
        free(f_args->target_host);
        free(f_args);
        free(sockets1);
        free(sockets2);
        return NULL;
    }

    pthread_detach(thread_client_to_target);
    pthread_detach(thread_target_to_client);

    free(f_args->target_host);
    free(f_args);
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
int main( int argc, char *argv[]  )
{
    if( argc < 5 )
	{
  	    printf("Utilizar el programa con 4 argumentos: addr_listen, port_listen, addr_dst, port_dst\n");
  	    return 0;
	}
    
    char * bind_ip = (char *) &argv[1][0];
    int bind_port = atoi((char *) &argv[2][0]);
    char * target_host = (char *) &argv[3][0];
    int target_port = atoi((char *) &argv[4][0]);

    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket() failed in main");
        return 1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt(SO_REUSEADDR) failed in main");
        close(server_fd);
        return 1;
    }

#ifdef __APPLE__
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt(SO_REUSEPORT) failed in main (macOS)");
        close(server_fd);
        return 1;
    }
#else
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt(SO_REUSEPORT) failed in main");
        close(server_fd);
        return 1;
    }
#endif

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(bind_port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind() failed in main");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("listen() failed in main");
        close(server_fd);
        return 1;
    }

    printf("[*] Escuchando en %s:%d\n", bind_ip, bind_port);
    printf("[*] Reenviando tráfico a %s:%d\n", target_host, target_port);

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == -1)
        {
            perror("accept() failed in main");
            continue;
        }

        printf("[+] Aceptada conexión de %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        pthread_t thread_id;
        struct ForwarderArgs *args = malloc(sizeof(struct ForwarderArgs));
        if (args == NULL)
        {
            perror("malloc() failed for ForwarderArgs");
            close(new_socket);
            continue;
        }
        args->client_socket = new_socket;
        args->target_host = strdup(target_host);
        args->target_port = target_port;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)args) < 0)
        {
            perror("pthread_create() failed in main");
            close(new_socket);
            free(args->target_host);
            free(args);
        }
        else
        {
            pthread_detach(thread_id);
        }
    }

    close(server_fd);
    return 0;
}
