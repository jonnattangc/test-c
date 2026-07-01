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
#define GRUPO "224.0.1.1"
#define NUM_PC 2
#define MI_IP "172.16.10.111"

extern int errno;
extern leer_archivo();
extern cambiar_dato();
extern enviar_archivo();
extern tiempo_listo();
extern ran();

struct ips{
char ip[15];
} IPs[50];

char nombre_archivo[]="servidor.c";

struct pdu_control {
  char ip_manda[15];
  char ip_destino[15];
  char bufer[1024];
  int largo;
  int operacion;  // 0= recivir archivo 1=enviar archivo 2=cnfirmacion
};

main(int argc,char *argv[]){

        struct pdu_control pdu;
        struct  sockaddr_in enviador, recibidor;
        int sock_envio, sock_rec, aux;
	pid_t pid;
        struct ip_mreq mreq;
        FILE *file;
        char *args[]={"NULL"};
/*
        struct sigaction sigalrm;
        sigalrm.sa_handler = tiempo_listo;
        sigaction(SIGALRM, &sigalrm, NULL);
*/
strcpy(IPs[0].ip,"172.16.10.110");
/********************************************************************/   
while(1){ 
if(leer_archivo()==1){
 bzero(&enviador, sizeof(enviador));
 enviador.sin_family = AF_INET;
 enviador.sin_port = htons(atoi(argv[1]));
  pdu.operacion=2;
  strcpy(pdu.bufer,"Confirmacion");
  pdu.largo=13;
  strcpy(pdu.ip_manda,MI_IP);
  strcpy(pdu.ip_destino,"CUAL_QUIERA");
  if (inet_aton(GRUPO, &enviador.sin_addr) < 0) {
    perror("inet_aton");
    return 1;
  }
  if ((sock_envio = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return 1;
  }
  printf("AGENTE SERVIDOR: enviando ...\n");
  sleep(10);
  if(sendto(sock_envio, &pdu,sizeof(pdu), 0, (struct sockaddr *)&enviador, sizeof(enviador)) < 0) {perror("sento");} 
  else {
      fprintf(stdout, "AGENTE SERVIDOR: Envia operacion %d con  mensaje: %s\n", pdu.operacion,pdu.bufer);
      if((file = fopen(nombre_archivo,"r+"))==NULL){printf("fopen");}
      while(!feof(file)){
        fgets(pdu.bufer,sizeof(pdu.bufer),file);
        pdu.largo=strlen(pdu.bufer);
        if(sendto(sock_envio,&pdu,sizeof(pdu),0,(struct sockaddr *)&enviador, sizeof(enviador)) < 0) {perror("sento");} 
      }
      pdu.largo=0;
      fclose(file);//pasando los 3 seg
      system("rm -f servidor.c");
      strcpy(pdu.bufer,"FIN_ARCHIVO");
      if(sendto(sock_envio, &pdu,sizeof(pdu), 0, (struct sockaddr *)&enviador, sizeof(enviador)) < 0) {perror("sento");}                
    }   
  close(sock_envio);
  cambiar_dato();
}  
  

if(leer_archivo()==0){
  bzero(&enviador, sizeof(enviador));
  bzero(&recibidor, sizeof(recibidor)); 
  recibidor.sin_family = AF_INET;
  recibidor.sin_port = htons(atoi(argv[1]));
  if (inet_aton(GRUPO, &recibidor.sin_addr) < 0) {  perror("inet_aton"); return 1; }
  if ((sock_rec = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { perror("socket");  return 1; }
  if (bind(sock_rec, (struct sockaddr *)&recibidor, sizeof(recibidor)) < 0) {  perror("bind"); }
  if (inet_aton(GRUPO, &mreq.imr_multiaddr) < 0) { perror("inet_aton"); return 1; }
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sock_rec, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {  perror("setsockopt");  return 1; }
  printf("AGENTE CLIENTE: Preparado para recibir ... \n");
  aux=open(nombre_archivo,O_CREAT|O_WRONLY,0644);
  while (1) {
   if (read(sock_rec,&pdu,sizeof(pdu))<0){perror("recvfrom");} 
   else {
    if(pdu.largo!=0 && strcmp(pdu.bufer,"Confirmacion")!=0)
       write(aux,&pdu.bufer,pdu.largo);
    if(pdu.largo==0) {printf("AGENTE CLIENTE: Termino pasar archivo\n");
         system("gcc -o servidor servidor.c");
         pid = fork();
         if (pid == 0) { 
           printf("Exec como nuevo proceso... %d\n",getpid());
           cambiar_dato();
	   system("./servidor");
          //exec("./servidor");  
         }
         else if (pid < 0){printf("Failed to fork");  exit(1); }
         break;
    }
   }
  }
  close(aux);
  close(sock_rec);
}
}
}
