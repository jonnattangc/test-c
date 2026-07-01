#include <stdio.h>
#include <stdlib.h>
// Tiene un error, pero no encuentro cual seria ...
unsigned int s,entrada, cont, inicializa, registro;
void semilla(unsigned int); 
unsigned int LFSR32(void);
void pausa(void);
int main (void){
    printf("Ingrese un numero para la funcion semilla\n");
    scanf("%u", &inicializa);
    printf("ingreso: %u  \n", inicializa);
    semilla(inicializa);
    cont=1;
    pausa();
    while(1){
      if (s==LFSR32()) break;
      cont++;
    }
    printf("\nLa funcion se uso %u veces antes que se repitiera la semilla\n", cont);
    system("pause");
    return 0;
    }    
unsigned int LFSR32(void) {
char bit25 = (registro & 0x10000000) != 0;
char bit26 = (registro & 0x20000000) != 0;
char bit30 = (registro & 0x20000000) != 0;
char bit32 = (registro & 0x80000000) != 0;
printf("%c,%c,%c,%c", &bit25,&bit26,&bit30,&bit32);
entrada = ( (bit30 ^ bit32) ^ bit26 ) ^ bit25;
registro=(registro<<1)|entrada; 
return registro;
}

void semilla(unsigned int h){
s=h;
registro=s;     
}

void pausa(void){
char c ;
printf("\nAPRIETE ENTER PARA CONTINUAR ") ;
while( (c = getchar()) != '\n') ;
}
