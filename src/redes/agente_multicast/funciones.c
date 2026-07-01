#include<stdio.h>
#include <string.h>
#define GRUPO "224.0.1.1"
char nombre[]="maestro.dat";

//-------------------------------------
int leer_archivo(void){
  int aux=2;
  FILE *arch;
  arch = fopen(nombre, "r");
  if (arch!=NULL) fscanf(arch,"%d",&aux);
  fclose(arch);
  return aux;
}
//-------------------------------------
void cambiar_dato(void){
  int aux=2;
  FILE *arch;
  arch = fopen(nombre, "r+");
  if (arch!=NULL){
  fseek( arch, 0L, SEEK_SET);
    fscanf(arch,"%d",&aux);
    fseek( arch, 0L, SEEK_SET);
    if(aux==0){aux=1;fprintf(arch,"%d",aux);}
    else if(aux==1){aux=0;fprintf(arch,"%d",aux);}
    else printf("no tiene nada...\n");
  }
  fclose(arch);
 }
//------------------------------------
void tiempo_listo(void){
    printf("Comienza cambio servidor movil\n");
    return;
}
//------------------------------------
int ran(void){
        int randFile;
        unsigned short rand;
        randFile = open("/dev/random","r");
	read(randFile,&rand,sizeof(rand));
        while (rand/2 >8) rand=rand/5;
	close(randFile);
        return rand;
}
//---------------------------------------
