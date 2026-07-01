#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define REPOSO 1
#define CONECTADO 2
#define PASIVO 3
#define ACTIVO 4

extern int errno;
char dato='-' ;
char env='-' ;
int estado=REPOSO;
main(argc,argv)
int argc;
char *argv[];
{
        struct  sockaddr_in server, clt;
        int x, adrl, sock;
/********************************************************************/
        sock= socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
                perror ("no se puede crear socket");
                exit (1);
        }
/********************************************************************/
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(1999);
        if (bind(sock, &server, sizeof(server))<0){
                close(sock);
                perror("no se puede hacer bind del socket");
                exit(1);
        }
        adrl = sizeof (struct sockaddr_in);
        getsockname(sock, &server, &adrl);
       // printf("Me asignaron el puerto: %d\n",ntohs(server.sin_port));
/********************************************************************/
        printf("Esperando . . .\n");
        while(recvfrom(sock, &dato ,1, MSG_NOSIGNAL, (struct sockaddr *)&clt, &adrl) > 0 ){
	    printf("Letra %c \n estado %d ...\n", dato, estado);
	    if(estado==REPOSO){
               if(dato=='a') {env='5'; estado=PASIVO; goto aca;}
	       if(dato=='1') {env='a'; estado=ACTIVO; goto aca;}
               if (dato!='a' && dato!='1') { env='-'; estado=REPOSO; goto aca;}	
            }
	    if(estado==PASIVO){
               if(dato=='3') {env='c'; estado=CONECTADO;goto aca;}
	       //else if(dato=='1') {env='1'; estado=ACTIVO;}
               if (dato!='3') { env='-'; estado=PASIVO;goto aca; }	
            }
	    if(estado==ACTIVO){
               if(dato=='c') {env='7'; estado=CONECTADO;goto aca;}
	       //else if(dato=='1') {env='1'; estado=ACTIVO;}
               if(dato!='c') { env='-'; estado=ACTIVO;goto aca;}	
            }
	    if(estado==CONECTADO){
               if(dato=='2') {env='b'; estado=REPOSO;goto aca;}
	       if(dato=='4') {env='d'; estado=CONECTADO;goto aca;}
               if(dato=='b') {env='6'; estado=REPOSO;goto aca;}
               if(dato=='d') {env='8'; estado=CONECTADO;goto aca;}
          if (dato!='2' && dato!='4' && dato!='b'&& dato!='d'){ env='-'; estado=CONECTADO;goto aca;}	
            }
      aca:
            clt.sin_port=htons(10156);
	    sendto(sock,&env,1,MSG_NOSIGNAL,(struct sockaddr *)&clt, sizeof(clt));
	}	
	printf("Termine\n");
}
