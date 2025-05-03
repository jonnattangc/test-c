#include "md5.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


extern int errno;

char nombre_archivo[] = "instrucciones.txt";

struct pdu_dato_st {
    int largo;     /* 0 = No hay más datos */
    unsigned char bytes[1024];  /* Con largo=0, se ignora este campo */
};

struct pdu_control_st {
  int operacion;          /* 0= Bajar archivo, 1=??, 2=??, 3=MD5, 4=?? */
  char dato[1024];        /* Con operacion=2, 3 o 4 se ignora este campo */
};
//----------------------------------------------------------------------
main(argc, argv)
int argc;
char * argv[];
{
	struct pdu_control_st pdu_control;
        struct pdu_dato_st pdu_dato;
        struct  sockaddr_in client;
        struct  hostent *hp, *gethostbyname();
        int conexion, l, arch;
        bzero(&client, sizeof(client));
        client.sin_family = AF_INET;
        client.sin_port = htons(atoi(argv[2]));
        if (isdigit(argv[1][0]))
                client.sin_addr.s_addr = inet_addr(argv[1]);
        else {
                hp = gethostbyname(argv[1]);
                if (hp == NULL) {
                   printf ("No hay Host \n ");
                   exit (1);
                }
                bcopy(hp->h_addr, &client.sin_addr, hp->h_length);
        }
        conexion = socket(AF_INET, SOCK_STREAM, 0);
        if (conexion < 0) {
                printf("No es posible crear socket");
                exit (1);
        }
        connect(conexion,(struct sockaddr*)&client,sizeof(client));
//-----------------------------------------------------     
        pdu_control.operacion = 0;
        strcpy(pdu_control.dato,nombre_archivo);
        send(conexion,&pdu_control, sizeof(pdu_control),MSG_NOSIGNAL);
        arch=open("file.txt",O_WRONLY | O_CREAT | O_TRUNC | O_EXCL);
        if ( arch<0 ) {
           printf("Error del open");
	   exit(1);
        }
        while(recv(conexion,&pdu_dato,sizeof(pdu_dato),MSG_NOSIGNAL)>0){
           if(pdu_dato.largo == 0){ 
	      printf("No hay mas datos"); 
              break;
           }
           write(arch, &pdu_dato.bytes, pdu_dato.largo);
           send(conexion,&pdu_control,sizeof(pdu_control),MSG_NOSIGNAL);
        }
//-----------------------------------------------------
        pdu_control.operacion=3;
        strcpy(pdu_control.dato, MDFile("file.txt"));
        send(conexion,&pdu_control, sizeof(pdu_control),MSG_NOSIGNAL);
//-----------------------------------------------------
        pdu_control.operacion=2;
        send(conexion,&pdu_control, sizeof(pdu_control),MSG_NOSIGNAL);
        while(recv(conexion,&pdu_dato,sizeof(pdu_dato),MSG_NOSIGNAL)>0){
           if(pdu_dato.largo==0) break;
           write(arch, &pdu_dato.bytes, pdu_dato.largo);
        }
//------------------------------------------------------
        pdu_control.operacion=4;
        send(conexion,&pdu_control, sizeof(pdu_control),MSG_NOSIGNAL);
}
