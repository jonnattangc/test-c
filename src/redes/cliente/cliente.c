#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#define MAXBUF 256
#define PUERTO 5000
#define GRUPO "224.0.1.1"

int main(void) {
  int s, n, r;
  int test1;
  struct sockaddr_in srv, cli;
  struct ip_mreq mreq;
  char buf[MAXBUF];
  char buf2[MAXBUF]="ok";
  int contador;

  bzero(&srv, sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(PUERTO);
  if (inet_aton(GRUPO, &srv.sin_addr) < 0) {
    perror("inet_aton");
    return 1;
  }

  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return 1;
  }

  if (bind(s, (struct sockaddr *)&srv, sizeof(srv)) < 0) {
    perror("bind");
    return 1;
  }

  if (inet_aton(GRUPO, &mreq.imr_multiaddr) < 0) {
    perror("inet_aton");
    return 1;
  }
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

//IP_ADD_MEMBERSHIP 	struct ip_mreq 	Unirse a un grupo multicast.
  if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt");
    return 1;
  }

//printf("SOY EL CLIENTE %s\n", inet_ntoa(cli.sin_addr));
//printf("SOY EL CLIENTE %s\n", inet_ntoa(srv.sin_addr));
  n = sizeof(cli);
 contador=0;
  while (1) 
   {
    printf("contador en %d:\n", contador);
    if ((r = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&cli, &n)) < 0) 
    {  //recibe del servidor
      perror("recvfrom");
    } 
    else 
    {
      buf[r] = 0;
      printf("Mensaje desde servidor movil %s: %s\n", inet_ntoa(cli.sin_addr), buf);
      if (test1=(sendto(s, buf2, strlen(buf2), 0, (struct sockaddr *)&cli, sizeof(cli))< 0))//envia mensaje al servidor
      {
      printf("pifia");      
      }
    }
    contador++;
  }
}   