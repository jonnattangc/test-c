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
        int x, l, adrl, sock, conexion;
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
        server.sin_port = 0;
        x = bind(sock, (struct  sockaddr *)&server, sizeof(server));
        if (x<0){
                close(sock);
                perror("no se puede hacer bind del socket");
                exit(1);
        }
        adrl = sizeof (struct sockaddr_in);
        getsockname(sock, (struct  sockaddr *)&server, &adrl);
        printf("Me asignaron el puerto: %d\n",ntohs(server.sin_port));
/********************************************************************/
        if (listen(sock, 5) < 0) {
                perror ("no listen");
                exit (1);
        }
/********************************************************************/
        printf("Servidor activo\n");
        while(1){
                conexion = accept(sock, (struct  sockaddr *)&server, &adrl);
                printf("Llama IP: %s\n",inet_ntoa(server.sin_addr));


/**********************TIMEOUT*******************************/
{ 
	struct timeval t;
	int result; 
	int ol = sizeof(struct timeval);
	result = getsockopt(conexion,        
			SOL_SOCKET,      
			SO_RCVTIMEO,     
			&t,             
			&ol); 
	if (result < 0)perror("timeout");
	printf("el timeOut era: %d\n", t.tv_sec);

	t.tv_sec = atoi(argv[1]);  /* se asigna 10 segundos de timeOut */
	t.tv_usec = 0;
	result = setsockopt(conexion,    /* socket o conexion afectado */
			SOL_SOCKET,      /* opcion al nivel TCP */
			SO_RCVTIMEO,     /* nombre de la opcion */
			&t,              /* tiempo */
			sizeof(struct timeval)); /* largo de la estructura */
	if (result < 0)perror("timeout");

	result = getsockopt(conexion,        
			SOL_SOCKET,      
			SO_RCVTIMEO,     
			&t,             
			&ol); 
	if (result < 0)perror("timeout");
	printf("el timeOut ahora es: %d\n", t.tv_sec);

}
/******************TIMEOUT****************/

		while(1){
                	l=read(conexion, bufer, sizeof(bufer));
			if (l==0){
				printf("fin de llamada\n");
				break;
			}
			if (l>0) write(1, bufer, l);
                	if (l<0){
				printf("timeOut!!!\n");
				break;
		  	}
		}
                close(conexion);
        }
}
