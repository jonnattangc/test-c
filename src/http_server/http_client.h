#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

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

typedef struct
{
    int server_socket;
    int client_socket;
    int connection_port;
    char client_host[ADDR_LEN];
} Data_Client;

void * process_data( void *pArg );

#endif