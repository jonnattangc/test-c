#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>

extern int errno;
char bufer[256];

main(argc, argv)
int argc;
char * argv[];
{
        int conexion, l;
        struct  sockaddr_in client;
        struct  hostent *hp, *gethostbyname();
        bzero(&client, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_port = htons(atoi(argv[2]));
        if (isdigit(argv[1][0]))
                client.sin_addr.s_addr = inet_addr(argv[1]);
        else {
                hp = gethostbyname(argv[1]);
                if (hp == NULL) {
                        perror ("¿Cual host ? ");
                        exit (1);
                }
                bcopy(hp->h_addr, &client.sin_addr, hp->h_length);
        }
/*******************************************************************/
        conexion = socket(AF_INET, SOCK_STREAM, 0);
        if (conexion < 0) {
                perror ("no es posible crear socket");
                exit (1);
        }
/*******************************************************************/
        connect(conexion, &client, sizeof(client));
        l=read(0, bufer, sizeof(bufer));
        write(conexion, bufer, l);
        while((l=read(conexion, bufer, sizeof(bufer)))>0) write(1, bufer, 
l);
}
