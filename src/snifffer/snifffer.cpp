#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <features.h>    /* for the glibc version number */

#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
  #include <netpacket/packet.h>
  #include <net/ethernet.h>     /* the L2 protocols */
#else
  #include <asm/types.h>
  #include <linux/if_packet.h>
  #include <linux/if_ether.h>   /* The L2 protocols */
#endif

#define TIMEOUT 					10
#define LEN_TEXT_BYTES 		256
#define LEN_BUFFER_BYTES 	(1024*128)

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <time.h>
#include <unistd.h>

using namespace std;

//-----------------------------------------------------------------------------
int crea_socket( const char * pInterfaz, char *pamyIP )
{
  struct sockaddr_ll  sa;    // para manejar el socket
  struct ifreq        ifr;   // para manejar una interfaz

  char mac[LEN_TEXT_BYTES], aux[LEN_TEXT_BYTES];
  int lar=-1, b=-1, eth_index=-1, ret=-1, sock=-1;

  //
  memset( pamyIP, 0, LEN_TEXT_BYTES );
  memset( &mac[0], 0, LEN_TEXT_BYTES );
  memset( &aux[0], 0, LEN_TEXT_BYTES );

  memset(&sa, 0, sizeof(struct sockaddr_ll));
  // a nivel de tarjeta de red y un socket sin procesar
  if((sock = socket( AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)/*IPPROTO_RAW */)) < 0 ) // AF_INET AF_PACKET - IPPROTO_RAW
  {
    printf("No se puede crear socket\n");
    return -1;
  }
  memset(&ifr,0,sizeof(struct ifreq));
  memcpy(ifr.ifr_name, pInterfaz, strlen(pInterfaz) );
  // obtiene el indice de la interfaz
  if( ioctl(sock, SIOCGIFINDEX, &ifr) == 0 )
    eth_index = ifr.ifr_ifindex;
  else
    printf("ERROR: No se puede obtener el indice de la tarjeta %s", pInterfaz);

  memset (&ifr, 0, sizeof (struct ifreq));
  memcpy(ifr.ifr_name, pInterfaz, strlen(pInterfaz) );
  if( ioctl(sock, SIOCGIFADDR, &ifr) == 0 )
  {
    switch( ifr.ifr_addr.sa_family )
    {
      case AF_UNSPEC:
          return -1;
      case AF_INET:
      case AF_INET6:
          if( getnameinfo(&ifr.ifr_addr, sizeof(ifr.ifr_addr), &pamyIP[0], LEN_TEXT_BYTES, 0, 0, NI_NUMERICHOST) < 0)
            printf("ERROR: No se puede rescatar nombre del host\n");
          break;
      default:
          sprintf(pamyIP, "unknown family");
    }
  }

  memset (&ifr, 0, sizeof (struct ifreq));
  memcpy(ifr.ifr_name, pInterfaz, strlen(pInterfaz) );
  if( ioctl(sock, SIOCGIFHWADDR, &ifr) == 0)
  {
    sprintf(&mac[0], "%02X:%02X:%02X:%02X:%02X:%02X",
						( ifr.ifr_hwaddr.sa_data[0] & 0xFF ),
						( ifr.ifr_hwaddr.sa_data[1] & 0xFF ),
						( ifr.ifr_hwaddr.sa_data[2] & 0xFF ),
						( ifr.ifr_hwaddr.sa_data[3] & 0xFF ),
						( ifr.ifr_hwaddr.sa_data[4] & 0xFF ),
						( ifr.ifr_hwaddr.sa_data[5] & 0xFF ) );
  }

 printf("Socke[%d] Interfaz[%s] Indice[%d] Host[%s] MAC: %s\n",
      sock, pInterfaz, eth_index, pamyIP, mac );


  sa.sll_protocol = htons(ETH_P_ALL); //recibe todos a esta y broadcast
  sa.sll_family = PF_PACKET; // PF_PACKET o AF_PACKET... PACKET_MR_MULTICAST
  sa.sll_ifindex = eth_index;

  if( bind(sock, (struct sockaddr *)&sa, sizeof(struct sockaddr_ll)) < 0 ) // para elegir interfaz
    printf("No se puede bindear interfaz %s \n", pInterfaz );

	struct packet_mreq  mr;
  // pone el dispositivo en modo promiscuo.
  memset(&mr, 0, sizeof(struct packet_mreq));
  mr.mr_ifindex = eth_index;
  mr.mr_type = PACKET_MR_PROMISC;
  mr.mr_alen = strlen(pInterfaz);
  memcpy(&mr.mr_address[0], pInterfaz, strlen(pInterfaz) );

  if ( setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, (void *)&mr, (socklen_t)sizeof(mr) ) < 0 )
    printf("No se puede poner interfaz %s en modo promiscuo\n", pInterfaz );

  return sock;
}

//-----------------------------------------------------------------------------
// Metodo Principal
//-----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
  if( argc < 2 )
	{
  	printf("Utilizar el programa con 1 argumento: Nombre Eth\n");
  	return 0;
	}

	unsigned char buffer[LEN_BUFFER_BYTES];//bufer para recibir unidad de datos
  char macDes[LEN_TEXT_BYTES], macSrc[LEN_TEXT_BYTES], myIP[LEN_TEXT_BYTES];
  unsigned int flag=0, flag2=0, len=-1;
  int socket, pos=0, version=0;
  unsigned short typeEthMsg = 0, lenTotal = 0;

  char pInterfaces[argc-1][LEN_TEXT_BYTES];
  char mysIPs[argc-1][LEN_TEXT_BYTES];
  int sockets[argc-1], tarjetas = 0, tarjeta =0;

  char * pInterfaz = NULL; //(char *) &argv[1][0];
	tarjetas = argc - 1;

  printf("tarjetas: %d\n", tarjetas );
	for( int i=1; i < argc ; i++ )
	{
	  char * pInterfaz = (char *) &argv[i][0];
	  memset(&pInterfaces[i-1], 0, LEN_TEXT_BYTES);
	  memcpy(&pInterfaces[i-1], pInterfaz, strlen(pInterfaz));
	  printf("%s\n", pInterfaces[i-1]);
  }

	socklen_t fromlen = -1;              // Length of sender's address.
  struct sockaddr_in srcAddr;

  for( int i=0; i < tarjetas ; i++ )
  {
	  sockets[i] = crea_socket( &pInterfaces[i][0], &mysIPs[i][0] );
  	if( sockets[i] < 0 )
    	return 0;
  }

	fromlen = sizeof(struct sockaddr_in);
  while( true )
  {
    if(tarjetas > 1) tarjeta = (++tarjeta%2);
    len = recv( sockets[tarjeta], buffer, LEN_BUFFER_BYTES, 0 );
    if( len > 0 )
    {
      //------------------- Capa Fisica ---------------------------------------
      sprintf( macDes, "%02X:%02X:%02X:%02X:%02X:%02X", buffer[0],
        buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
      sprintf( macSrc, "%02X:%02X:%02X:%02X:%02X:%02X", buffer[6],
        buffer[7], buffer[8], buffer[9], buffer[10], buffer[11]);
      memcpy(&typeEthMsg, &buffer[12], sizeof(unsigned short));
      //-----------------------------------------------------------------------
			pos = 14;
			if( typeEthMsg == 8 ) // Mensajes IPV4
			{
				//------------------- Capa RED    ---------------------------------------
				version = (int)( ( buffer[pos] >> 4 ) & 0x000000FF);
				if( version == 4 ) // Protocolo IP
				{
				  int lenHdr = ( ( buffer[pos] & 0x0F ) * sizeof(unsigned int) );
				  // printf("Largo Header in Word IHL(%d) ", ( buffer[pos] & 0x0F ) );
					// printf("Service Type(%d) ",  buffer[pos+1] );
					// printf("N° Word: ", (unsigned short) buffer[pos+2]);
					// printf("Id(%d) ", (unsigned short) buffer[pos+4] );
					// printf("CkSum(%d) ", (unsigned short) buffer[pos+6] );
					int protocol = int( buffer[pos+9] );
					// printf("Protocol(%d) ", buffer[pos+9] );
					if( protocol == 1 ) // ICMP Internet Control Message Protocol
					{
					  int posPackage = pos + lenHdr;
					  if( buffer[posPackage] == 0 )
						{
							printf("Rx %d bytes de[%s] a[%s] TypeMSG[%d] ", len, macSrc, macDes, typeEthMsg );
						  printf("Protocol ICMP Type[Respuesta PING] %d data\n", (len-posPackage));
						}
						else if( buffer[posPackage] == 8 )
						{
							printf("Rx %d bytes de[%s] a[%s] TypeMSG[%d] ", len, macSrc, macDes, typeEthMsg );
							printf("Protocol ICMP Type[PING]\n");
						}
						else if( buffer[posPackage] == 5 )
						  printf("Protocol ICMP Type[RIP]\n");
						else
						  printf("Protocol ICMP unknown Type[%d]\n", buffer[posPackage]);
						// ver http://neo.lcc.uma.es/evirtual/cdd/tutorial/red/cabipv4.html
					}
					else if( protocol == 6 )
					{
					  printf("Rx %d bytes de[%s] a[%s] TypeMSG[%d] ", len, macSrc, macDes, typeEthMsg );
					  printf("Protocolo TPC ");
					  printf("IP(%d.%d.%d.%d) a ",  buffer[26], buffer[27], buffer[28], buffer[29]);
					  printf("IP(%d.%d.%d.%d)\n",  		buffer[30], buffer[31], buffer[32], buffer[33]);
					}
					else if( protocol == 17 ) // UDP type
					{
						 unsigned short dst=0, src=0;
						 memcpy(&src, &buffer[pos+lenHdr], sizeof(short));
						 memcpy(&dst, &buffer[pos+lenHdr+2], sizeof(short));
						 printf("Rx %d bytes de[%s] a[%s] TypeMSG[%d] ", len, macSrc, macDes, typeEthMsg );
						 printf("Protocolo UDP PortSrc[%d] PortDst[%d] ", src, dst);
						 printf("IP(%d.%d.%d.%d) a ",  buffer[26], buffer[27], buffer[28], buffer[29]);
						 printf("IP(%d.%d.%d.%d)\n",  		buffer[30], buffer[31], buffer[32], buffer[33]);
					}
					else
            printf("Protocolo: %d ", protocol );
				}
			}
			else if( typeEthMsg == 1544 )
			{
				printf("Rx %d bytes de[%s] a[%s] TypeMSG[%d] ", len, macSrc, macDes, typeEthMsg );
				printf("Protocolo ARP ");
			  printf("Src: %d.%d.%d.%d ", buffer[28], buffer[29], buffer[30], buffer[31]);
				printf("Dst: %d.%d.%d.%d \n", buffer[38], buffer[39], buffer[40], buffer[41]);
			}
			else if( typeEthMsg == 56710 )
			{
			  version = (int)( ( buffer[pos] >> 4 ) & 0x000000FF);
 			  printf("Protocolo IPv6\n");
			}
			else if( typeEthMsg == 0x8060 )
				printf("Protocolo ARP\n");
			else
				printf("Type[%d] Desconocido...\n");

    }
    else
    {
    	printf("No hay data... q paso !\n");
    }
  }
}
