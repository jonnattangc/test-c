#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>

#define NOBLOCK         0x0FFFB
extern int errno;

int
socket_servidor(side)
int side;
{
        struct  sockaddr_in server;
        int   sock, x, flags;
        sock= socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                perror ("no able of creat socket");
                exit (1);
        }

        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(side);

        x = bind(sock, &server, sizeof(server));
        if (x<0){
        close(sock);
        perror("no bbind");
        exit(1);
        }
        if (listen(sock, 5) < 0) {
                perror ("no listen");
                exit (1);
        }
        return sock;
}

int 
acepta_conexion(sock)
int sock;
{
        struct  sockaddr_in server;
        int   adrl, x, flags;
        adrl = sizeof (struct sockaddr_in);
        x = accept (sock, &server, &adrl);
        return x;
}
/*********** cliente ***********************/

int
socket_cliente(host, port)
char *host;
int port;
{
        int csock;
	int sc;
        struct  sockaddr_in client;
        struct  hostent *hp, *gethostbyname();

        bzero(&client, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_port = htons(port);

        if (isdigit(host[0]))
                client.sin_addr.s_addr = inet_addr(host);
        else {
                hp = gethostbyname(host);
                if (hp == NULL) {
                        perror (" which host ? ");
                        exit (1);
                }
                bcopy(hp->h_addr, &client.sin_addr, hp->h_length);
        }

        csock = socket(AF_INET, SOCK_STREAM, 0);
        if (csock < 0) {
                perror ("no able of creat socket");
                exit (1);
        }

	connect(csock, &client, sizeof(client));
	return csock;
}


