#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct sockaddr whereto;
struct hostent *hp;
struct sockaddr_in *to;
char *target;
char *hostname;
 
memset(&whereto, 0, sizeof(struct sockaddr));
to = (struct sockaddr_in *)&whereto;
to->sin_family = AF_INET;
to->sin_addr.s_addr = inet_addr(target);
if (to->sin_addr.s_addr != -1)
   hostname = target;
else
{
   hp = gethostbyname(target);
   if (!hp)
      printf("host desconocido%s\n", target);
   else
   {
      to->sin_family = hp->h_addrtype;
      memcpy(&(to->sin_addr.s_addr), hp->h_addr, hp->h_length);
      hostname = hp->h_name;
      printf("gethostbyname ejecutado correctamente\n");
   }
}
