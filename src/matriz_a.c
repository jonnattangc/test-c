#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*FUNCIONES ANALIZADAS*/
float ** multiplica(float **, float **, int , int , int );
float ** strassen(int NN, float **, float **);
/*FUNCIONES AYUDA*/
void llena_matriz(float **, int, int);
void print_matriz(float **, int, int);
float ** suma(int , float **, float **);
float ** resta(int , float **, float **);
float ** multNormal(float **,float **, int, int, int);
/*--------------------------------------------------------------------*/
int main(void) {
  float **A,**B; /*matrices 1 y 2*/
  float **Mresp; /* Matriz Respuesta*/
  int f1,c1,f2,c2,i,j,n;
  printf("Tarea 3\n");
  printf("Dimension:\t");
  scanf("%d", &n);
  f1=n;
  f2=n;
  c1=n;
  c2=n;
  /*
  printf("Numero Filas Primera Matriz:\t");
  scanf("%d", &f1);
  printf("\nNumero Columnas Primera Matriz:\t");
  scanf("%d", &c1);
  printf("\nNumero Filas Segunda Matriz:\t");
  scanf("%d", &f2);
  printf("\nNumero Columnas segunda Matriz:\t");
  scanf("%d", &c2);
  */
  printf("\n");
  if(c1!=f2){ 
    printf("No se pueden multiplicar, por problemas en las dimensiones\n");
    system("pause");
    exit(0);
  }
  /*contruimos las matrices con numeros aleatorios de 1-100 */
  A = (float**)malloc(sizeof(float*)*f1);
  for(i=0;i<f1;i++) A[i]=(float*)malloc(sizeof(float)*c1);
  llena_matriz(A,f1,c1);
  B = (float**)malloc(sizeof(float*)*f2);
  for(i=0;i<f2;i++) B[i]=(float*)malloc(sizeof(float)*c2);  
  llena_matriz(B,f2,c2);
  /*Inicializa Matriz de Respuesta*/
  Mresp = (float**)malloc(sizeof(float*)*f1);
  for(i=0;i<f1;i++) Mresp[i]=(float*)malloc(sizeof(float)*c2);  
  llena_matriz(Mresp,f1,c2); /*Solo para inicializarla en algun valor!=0*/
  printf("Primero Ejecutamos Algoritmo Normal\nEspere Por favor...\n");
  Mresp=multiplica(A,B,f1,f2,c2); 
  printf("Desea Ver Matriz Respuesta??(1:SI, 2:NO)");
  scanf("%d",&i);
  if(i==1) print_matriz(Mresp,f1,c2);
  Mresp=strassen(n,A,B);
  printf("Desea Ver Matriz Respuesta??(1:SI, 2:NO)");
  scanf("%d",&i);
  if(i==1) print_matriz(Mresp,f1,c2);
  system("pause");
  /*soltar la memoria*/
  for(i=0;i<f1;i++) free(A[i]);       
  free(A);
  for(i=0;i<f2;i++) free(B[i]);       
  free(B);
  for(i=0;i<f1;i++) free(Mresp[i]);       
  free(Mresp);
  return 0;
}
/*--------------------------------------------------------------------*/
float ** multiplica(float **A, float **B, int f1, int f2, int c2){ 
float **C; 
int i, j, k;
time_t start, end; /* variables para calcular tiempos de ejecucion*/
C = (float**)malloc(sizeof(float*)*f1);
for(i=0;i<f1;i++) C[i]=(float*)malloc(sizeof(float)*c2);  
start = time(NULL);
for (i = 0; i < f1; i++)
 for (j = 0; j < c2; j++){
    C[i][j] = 0;
    for (k = 0; k < f2; k++)
       C[i][j] = C[i][j] + A[i][k]*B[k][j];
 }
end = time(NULL);
printf("Tiempo Algoritmo Normal : %1f \n",difftime(end,start));
return C;
} 
/*--------------------------------------------------------------------*/
float ** multNormal(float **A, float **B, int f1, int f2, int c2){ 
float **C; 
int i, j, k;
C = (float**)malloc(sizeof(float*)*f1);
for(i=0;i<f1;i++) C[i]=(float*)malloc(sizeof(float)*c2);  
for (i = 0; i < f1; i++)
 for (j = 0; j < c2; j++){
    C[i][j] = 0;
    for (k = 0; k < f2; k++)
       C[i][j] = C[i][j] + A[i][k]*B[k][j];
 }
return C;
} 
/*--------------------------------------------------------------------*/   
float ** suma(int n, float **a, float **b){
  int i,j;
  float **c;
  c = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) c[i]=(float*)malloc(sizeof(float)*n);
  for (i=0;i<n;i++){ 
    for (j = 0; j < n; j++){
      c[i][j]=a[i][j]+b[i][j];
    }
  }
  return c;  
}
/*--------------------------------------------------------------------*/
float ** resta(int n, float **a, float **b){
  int i,j;
  float **c;
  c = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) c[i]=(float*)malloc(sizeof(float)*n);
  for (i=0;i<n;i++){ 
    for (j = 0; j < n; j++){
      c[i][j]=a[i][j]-b[i][j];
    }
  }
  return c;  
}
/*--------------------------------------------------------------------*/
void llena_matriz(float **M, int f, int c) {
  int i, j;
  float dato;
  for(i = 0; i < f; i++)
    for(j = 0; j < c; j++) {
      /*printf("[%d][%d]\n", i, j);
      scanf("%f", &dato);*/
      dato=(float)(rand()%100)+1; 
      M[i][j] = dato;
    }    
}             
/*--------------------------------------------------------------------*/
void print_matriz(float **M, int f, int c) {
  int i, j;
  for(i = 0; i < f; i++) {
    for(j = 0; j < c; j++) {
       printf("%f\t", M[i][j]);
    }
    printf("\n");
  }
}
/*--------------------------------------------------------------------*/
/*ALGORITMO DE STRASSEN*/
float ** strassen(int NN, float **a, float **b){
  int i,j,k,n;
  int p1=0,q1=0,p2=0,q2=0,p3=0,q3=0,p4=0,q4=0;
  float **a11, **a12, **a21, **a22;
  float **b11, **b12, **b21, **b22;
  float **M0, **M1, **M2, **M3, **M4, **M5, **M6;
  float **c1, **c2, **d1, **d2;
  float **c, **c11, **c12, **c21, **c22;
  time_t start, end; /* variables para calcular tiempos de ejecucion*/
  start=time(NULL);
  
  /*LIMITE DE LA RECURSIVIDAD*/
  if(NN<3){     
      c=(float**)malloc(sizeof(float*)*NN);
      for(i=0;i<NN;i++) c[i]=(float*)malloc(sizeof(float)*NN);   
      c= multiplica(a,b,NN,NN,NN);
      return c;
  }
  
  c=(float**)malloc(sizeof(float*)*NN);
  for(i=0;i<NN;i++) c[i]=(float*)malloc(sizeof(float)*NN);

  n = NN/2;
  /*PEDIMOS LA MEMORIA PARA LAS SUB MATRICES*/
  a11 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) a11[i]=(float*)malloc(sizeof(float)*n);
  a12 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) a12[i]=(float*)malloc(sizeof(float)*n);
  a21 =(float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) a21[i]=(float*)malloc(sizeof(float)*n);
  a22 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) a22[i]=(float*)malloc(sizeof(float)*n);
  b11 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) b11[i]=(float*)malloc(sizeof(float)*n);
  b12 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) b12[i]=(float*)malloc(sizeof(float)*n);
  b21 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) b21[i]=(float*)malloc(sizeof(float)*n);
  b22 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) b22[i]=(float*)malloc(sizeof(float)*n);
  
  /*SACAMOS LAS SUB MATRICES*/    
  p1=0;q1=0;p2=0;q2=0;p3=0;q3=0;
  
  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
        a11[i][j]=a[i][j];
        b11[p3][q3]=b[i][j];
    }
  } 
  
  for(i=n;i<2*n;i++){        
    for(j=0;j<n;j++){            
        a21[p1][q1]=a[i][j];
        b21[p3][q3]=b[i][j];
        q1++;
    }
    p1++;
  } 
  
  for(i=0;i<n;i++){        
    for(j=n;j<2*n;j++){            
        a12[p2][q2]=a[i][j];
        b12[p3][q3]=b[i][j];
        q2++;
    }
    p2++;
  } 

for(i=n;i<2*n;i++){        
    for(j=n;j<2*n;j++){            
        a22[p3][q3]=a[i][j];
        b22[p3][q3]=b[i][j];
        q3++;
    }
    p3++;
  } 

/*PEDIMOS MEMORIA PAR LOS CALCULOS INTERMEDIOS, VARIABLES AUXILIARES Y M FINALES*/
  d1  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) d1[i]=(float*)malloc(sizeof(float)*n);
  
  d2  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) d2[i]=(float*)malloc(sizeof(float)*n);
  M0  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) M0[i]=(float*)malloc(sizeof(float)*n);
  
  M1  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) M1[i]=(float*)malloc(sizeof(float)*n);
  
  M2  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) M2[i]=(float*)malloc(sizeof(float)*n);
  
  M3  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) M3[i]=(float*)malloc(sizeof(float)*n);
  M4  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) M4[i]=(float*)malloc(sizeof(float)*n);
  
  M5  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) M5[i]=(float*)malloc(sizeof(float)*n);
  
  M6  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) M6[i]=(float*)malloc(sizeof(float)*n);
  
  c1  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) c1[i]=(float*)malloc(sizeof(float)*n);
  
  c2  = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) c2[i]=(float*)malloc(sizeof(float)*n);
  
  c11 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) c11[i]=(float*)malloc(sizeof(float)*n);
  
  c12 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) c12[i]=(float*)malloc(sizeof(float)*n);
  c21 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) c21[i]=(float*)malloc(sizeof(float)*n);
  
  c22 = (float**)malloc(sizeof(float*)*n);
  for(i=0;i<n;i++) c22[i]=(float*)malloc(sizeof(float)*n);
  
  /*HACEMOS EL CALCULO*/
  d1=suma (n, a11, a22);
  d2=suma (n, b11, b22);
  M0=strassen(n,d1,d2);
  d1=suma(n, a21, a22);
  M1=strassen(n,b11,d1);
  d2=resta(n,b12,b22);
  M2=strassen(n,a11,d2);
  d1=resta(n, b21, b11);
  M3=strassen(n,a22,d1);
  d1=suma(n, a11, a12);
  M4=strassen(n,d1,b22);
  d1=resta(n,a21,a11);
  d2=suma(n,b11,b12);
  M5=strassen(n,d1,d2);
  d1=resta(n, a12, a22);
  d2=suma(n, b21, b22);
  M6=strassen(n,d1,d2);
  
  c1=suma(n, M0, M3);
  c2=resta(n, c1, M4);
  c11=suma(n,c2,M6);
  c12=suma(n, M2, M4);
  c21=suma(n, M1, M3);
  
  c1=suma(n, M0, M2);
  c2=suma(n, c1, M1);
  c22=suma(n, c2,M5);
  
  
  /*ARMAMOS LA MATRIZ FINAL*/

   end = time(NULL);
  printf("Tiempo Algoritmo Strassen : %1f \n",difftime(end,start));
    system("pause");
  
  for(i=0;i<2*n;i++){
    for(j=0;j<2*n;j++){
        if(i<n && j<n) c[i][j]=c11[i][j];
        if(i>=n && j<n) c[i][j]=c21[i][j];
        if(i>n && j>=n) c[i][j]=c22[i][j];
        if(i<n && j>n) c[i][j]=c12[i][j];
    }
  }  
  
  return c;
}
