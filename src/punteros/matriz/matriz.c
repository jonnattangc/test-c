#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef float * Matriz_rc;
typedef float** Matriz_f;

void fill_matriz(Matriz_f, int, int);
void print_matriz(Matriz_f, int, int);

int main( int argc, char *argv[] ) {
  int rows = 0, columns = 0;
   
  printf("filas:\n");
  scanf("%d", &rows);
  printf("columnas\n");
  scanf("%d", &columns);
   
  Matriz_f pMatriz = (Matriz_f) malloc (sizeof(Matriz_rc) * rows); 
  memset(pMatriz, 0, sizeof(Matriz_rc) * rows);

  for(int i = 0; i < rows; i++) {
    pMatriz[i] = (Matriz_rc) malloc( sizeof(float) * columns);
    memset(pMatriz[i], 0, sizeof(float) * columns);
  }

  fill_matriz(pMatriz, rows, columns);
                
  print_matriz(pMatriz, rows, columns);
        


  
  for(int i = 0; i < rows; i++)
    free(pMatriz[i]);
         
  free(pMatriz);
  return EXIT_SUCCESS;
}
   
/**
 * @brief Rellena una matriz con valores introducidos por el usuario
 * 
 * Esta funcion pide al usuario que introduzca los valores para cada elemento
 * de la matriz, y los almacena en la matriz.
 * 
 * @param M puntero a la matriz a rellenar
 * @param f numero de filas de la matriz
 * @param c numero de columnas de la matriz
 */
void fill_matriz(Matriz_f M, int f, int c) {
  float dato = 0.0f;
  for(int i = 0; i < f; i++)
    for(int j = 0; j < c; j++) {
      printf("[%d][%d]\n", i, j);
      scanf("%f", &dato); 
      M[i][j] = dato;
    }  
  return;  
}        
      
/**
 * @brief Imprime una matriz por pantalla
 * 
 * Esta funcion itera sobre cada elemento de la matriz, imprimiendo su valor
 * seguido de un tabulador, y un salto de linea al final de cada fila.
 * 
 * @param M puntero a la matriz a imprimir
 * @param f numero de filas de la matriz
 * @param c numero de columnas de la matriz
 */
void print_matriz(Matriz_f M, int f, int c) {
  for(int i = 0; i < f; i++) {
    for(int j = 0; j < c; j++) {
      printf("%f\t", M[i][j]);
    }
    printf("\n");
  }
  return;
}
