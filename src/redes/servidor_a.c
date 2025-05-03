#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <string.h>

extern int errno;
sem_t semaforo;
int numero;

/********************************************************************/
static void *hebra(void * arg){
    char bufer[256];
    int con, l;
    con=(int)arg;
    l=read(con, bufer, sizeof(bufer));
    write(1, bufer, l);
    sem_wait(&semaforo);
    numero++;
    sleep(5);
    sprintf(bufer,"Eres el cliente No.: %d\n",numero);
    sem_post(&semaforo);
    write(con,bufer,strlen(bufer)+1);
    printf("cerro la conexion\n");
    close(con);
    return NULL;
}
/********************************************************************/

main(argc,argv)
int argc;
char *argv[];
{
    pthread_t id;
    int i;
    struct  sockaddr_in server;
    int x, adrl, sock, conexion;
/********************************************************************/
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
            perror ("no se puede crear socket");
            exit (1);
    }
/********************************************************************/
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));
    x = bind(sock, (struct  sockaddr *)&server, sizeof(server));
    if (x<0){
            close(sock);
            perror("no se puede hacer bind del socket");
            exit(1);
    }
/********************************************************************/
    if (listen(sock, 5) < 0) {
            perror ("no listen");
            exit (1);
    }
/********************************************************************/

    sem_init(&semaforo,0, 1);

/********************************************************************/
    adrl = sizeof (struct sockaddr_in);
    while(1){
            conexion = accept(sock, (struct  sockaddr *)&server, &adrl);
            printf("acepto una conexion de: %s\n",inet_ntoa(server.sin_addr));
            pthread_create(&id, NULL, hebra, conexion);
    }
}
