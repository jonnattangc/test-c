#include <stdio.h>
#include <stdlib.h>

void lee_matriz(float **, int, int);
void escribe_matriz(float **, int, int);

int main() {
  float **A, **B, **C;
  int af, ac;
  int i, j;
   
  printf("filas:\n");
  scanf("%d", &af);
  printf("columnas\n");
  scanf("%d", &ac);
   
  A = (float**)malloc(sizeof(float*)*af);
  for(i = 0; i < af; i++)
    A[i] = (float*)malloc(sizeof(float)*ac);
      
  lee_matriz(A, af, ac);
                
  escribe_matriz(A, af, ac);
        
  for(i = 0; i < af; i++)
    free(A[i]);
         
  free(A);
  return 0;
}
   
void lee_matriz(float **M, int f, int c) {
  int i, j;
  float dato;
   
  for(i = 0; i < f; i++)
    for(j = 0; j < c; j++) {
      printf("[%d][%d]\n", i, j);
      scanf("%f", &dato); 
      M[i][j] = dato;
    }    
}        
      
void escribe_matriz(float **M, int f, int c) {
  int i, j;

  for(i = 0; i < f; i++) {
    for(j = 0; j < c; j++) {
      printf("%f\t", M[i][j]);
    }
    printf("\n");
  }
}
