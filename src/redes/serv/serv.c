#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

extern int errno;
char bufer[256];

main(argc,argv)
int argc;
char *argv[];
{
        struct  sockaddr_in server;
        int x,l,adrl, sock, conexion;
/********************************************************************/
        sock= socket(AF_INET, SOCK_STREAM, 0);
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
        adrl = sizeof (struct sockaddr_in);
        printf("Servidor activo\n");
        while(1){
                conexion = accept(sock, (struct  sockaddr *)&server, &adrl);
                printf("Llama IP: %s\n",inet_ntoa(server.sin_addr));
                while((l=read(conexion, bufer, sizeof(bufer)))>0) 
                write(1, bufer, l);
                printf("Fin de la llamada\n");
                close(conexion);
        }
}
