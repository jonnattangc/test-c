#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>    /* alarm() se encuentra aqui         */
#include <signal.h>    /* signal macros, y el prototipo de signal() */

// las direcciones multicast rango: 224.0.0.1 a 239.255.255.255
#define DIR_MULTI "224.0.1.1"
#define NUM_PC 1
#define MI_IP "152.74.52.194"
#define PUERTO 14550

extern int errno;
extern leer_archivo();
extern cambiar_dato();
extern enviar_archivo();
extern tiempo_listo();
extern ran();

struct ips{
char ip[15];
} IPs[]={"152.74.52.194","NULL","NULL","NULL","NULL"};

char nombre_archivo="servidor.c";

struct pdu_control {
  char ip_manda[15];
  char ip_destino[15];
  char bufer[1024];
  int largo;
  int operacion;  // 0= recivir archivo 1=enviar archivo 2=cnfirmacion
};

main(int argc,char *argv[]){
        struct pdu_control pdu;
           
        struct  sockaddr_in server, clt;
	
        int x, adrl, conexion_mult, sock;
        struct ip_mreq multi;
        
        int fl, i=0,file,n,r;
/*
        struct sigaction sigalrm;
        sigalrm.sa_handler = tiempo_listo;
        sigaction(SIGALRM, &sigalrm, NULL);
*/
/********************************************************************/   
 bzero(&server, sizeof(server));
 server.sin_family = AF_INET;
 server.sin_port = htons(atoi(argv[1]));
 if (inet_aton(DIR_MULTI, &server.sin_addr) < 0) {
    perror("inet_aton");
    return 1;
 }
 if ((conexion_mult = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return 1;
 }


if(leer_archivo()==1){
  pdu.operacion=2;
  strcpy(pdu.bufer,"Confirmacion");
  pdu.largo=13;
  strcpy(pdu.ip_manda,MI_IP);
  strcpy(pdu.ip_destino,"CUAL_QUIERA");

  printf("Env�o msj aviso\" Operacion: %d de %s a %s desde %s\"\n",pdu.operacion,pdu.ip_destino,pdu.ip_manda);
  if (write(conexion_mult,&control,sizeof(control))<0){ perror("Write:");} 
  else {
    fprintf(stdout, "Enviado a %s: \n", DIR_MULTI);
  }  
}

if(leer_archivo()==0){
   printf("Preparado para responder..\n");    
   
   if (bind(conexion_mult, (struct sockaddr *)&server, sizeof(server)) < 0) { perror("bind"); }
   if (inet_aton(DIR_MULTI, &multi.imr_multiaddr) < 0) { perror("inet_aton"); }
   multi.imr_interface.s_addr = htonl(INADDR_ANY);
   if (setsockopt(conexion_mult, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multi, sizeof(multi)) < 0) {perror("setsockopt");}
   while(1){
    if(read(conexion_mult,&control,sizeof(control))<0){perror("read:"); } 
    else{ 
      if(pdu.operacion == 2){
         printf("Recibio: \" Operacion: %d de %s a %s desde %s\"\n",pdu.operacion,pdu.ip_destino,pdu.ip_manda);
      }
    }
    break;
   }
}
sleep(3);
exit(0);

/********************************************************************/
        bzero(&server, sizeof(server));
	bzero(&clt, sizeof(clt));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(PUERTO);
        if (bind(sock, (struct sockaddr *)&server, sizeof(server))<0){
                close(sock);
                perror("no se puede hacer bind del socket");
                exit(1);
        }
        adrl = sizeof (struct sockaddr_in);
        //clt.sin_addr.s_addr = inet_addr(IPs[ran()%5].ip);
	while(1){
         if((r=recvfrom(conexion_mult,&control,sizeof(control),0,(struct sockaddr *)&clt,&adrl))<0){  perror("recvfrom");
           } else{ 
	       if(control.operacion == 1){
                 control.operacion=0;
	         send(sock,&control, sizeof(control),MSG_NOSIGNAL);
                 break;
               }
             }
        }
	
    }        
}*/
}
