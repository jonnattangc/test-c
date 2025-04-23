#include <stdio.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string.h>
#include <ifaddrs.h> // Necesario para getifaddrs y freeifaddrs
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if_dl.h> // For struct sockaddr_dl
#include <sys/types.h>
#include <errno.h>

#define TIMEOUT 					10
#define LEN_TEXT_BYTES 		256
#define LEN_BUFFER_BYTES 	(1024*128)


int get_mac_address(const char *interface_name, unsigned char *mac_address) {
    int sock = socket(AF_LINK, SOCK_RAW, 0);
    if(sock < 0) {
      perror("socket");
      return -1;
    }

    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    if (ioctl(sock, SIOCSIFDSTADDR, &ifr) < 0) {
        perror("ioctl (SIOCSIFDSTADDR)");
        close(sock);
        return -1;
    }

    close(sock);

    struct sockaddr_dl *sdl = (struct sockaddr_dl *)&ifr.ifr_addr;
    if (sdl->sdl_family == AF_LINK && sdl->sdl_alen == 6) {
        memcpy(mac_address, LLADDR(sdl), 6);
        return 0;
    } else {
        fprintf(stderr, "Unexpected address family or length for MAC address.\n");
        return -1;
    }
}

/**
*
*
*/
void print_eth_names() {
    struct ifaddrs *ifaddr, *ifa;
    unsigned char mac[LEN_TEXT_BYTES];
    char interfaz[LEN_TEXT_BYTES];
    memset( &interfaz[0], 0, LEN_TEXT_BYTES );
    memset( &mac[0], 0, LEN_TEXT_BYTES );

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr != NULL && (ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_LOOPBACK)) {
            memcpy(&interfaz[0], ifa->ifa_name, strlen(ifa->ifa_name));
            if (get_mac_address(&interfaz[0], mac) == 0) {
                printf("MAC address of %s: %02x:%02x:%02x:%02x:%02x:%02x\n", interfaz, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }
        }
    }
    freeifaddrs(ifaddr);
    return ;
}

/**
int crea_socket( const char * pInterfaz, char *pamyIP ){
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
  if((sock = socket( AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0 ) // IPPROTO_RAW AF_INET AF_PACKET - IPPROTO_RAW
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

*/

/** */
int main(int argc, char *argv[] ) {
  if( argc < 2 )
  {
#if defined (SIOCSIFDSTADDR)
    printf("Value of SIOCSIFDSTADDR on macOS: %u\n", SIOCSIFDSTADDR);
#else
    printf("NO SIOCSIFDSTADDR Defined\n");
    return 0;
#endif
    print_eth_names();

    printf("Utilizar el programa con 1 argumento: Cual \n");
  }
  else {

  }


    return 0;
}
