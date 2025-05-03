// PARA COMPILAR el siguiente programa escribir: 
// gcc -o servidor servidor.c socket.c 

#include <math.h>
#include <stdio.h>
#include <string.h>

extern  acepta_conexion();
extern  call_on_sock();

FILE *arch ;

struct PDU {
        char cabeza;
        char mensaje[100];
}

main()
{
	int i;
        int dirlisten, conexion;
        struct PDU peticion, respuesta;

	dirlisten=socket_servidor(7000);
	while(1){
                printf("\n\n\n *****Servidor Corriendo*****   \n\n\n\n");
                while((conexion=acepta_conexion(dirlisten))<0);
                        read(conexion, &peticion, sizeof(peticion));
			if( (arch=fopen(peticion.mensaje,"rb"))== NULL)
					{
					printf ("peticion erronea ");
					strcpy(respuesta.mensaje," Archivo pedido no existe \n");
					write(conexion,&respuesta,sizeof(respuesta));
                                        //strcpy(respuesta.mensaje,"Fin");
					//write(conexion,&respuesta,sizeof(respuesta));				
					//exit (1);
					}
					else {
	                                    do{ 
					    fgets(&respuesta,sizeof(respuesta),arch);
   				            write(conexion,&respuesta,sizeof(respuesta)); 
		                             }while(!feof(arch));
					}	
					   strcpy(respuesta.mensaje,"Fin");
					   write(conexion,&respuesta,sizeof(respuesta));
					   	//		printf("DATO:%s\n",peticion.mensaje);
                        
	//		strcpy(respuesta.mensaje,"Mensaje recibido");
//                        write(conexion, &respuesta, sizeof(respuesta));
                        close(conexion);
        }
}

