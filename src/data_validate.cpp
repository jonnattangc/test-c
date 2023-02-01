#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN_LINE 1024
#define LEN_ARRAY 1000000

int main(void)
{
    char *pLine = new char[LEN_LINE];
    int number = 0, pos = 0, i = 0, found = 0;
    char letra = 0;
    int *pIds = new int[LEN_ARRAY];
    memset(pIds, 0, (sizeof(int) * LEN_ARRAY) );

    FILE *pFile = NULL;
    int size = 0;
    printf("Proceso de archivos!!!!\n");

    pFile = fopen("enex_items_ejemplo_IDs.csv", "r");
    if (pFile == NULL)
        exit(EXIT_FAILURE);
    int ret = fseek (pFile, 0, SEEK_END);   // non-portable
    if( ret  == 0 ) {
      size = ftell (pFile);
      fseek(pFile, 0, SEEK_SET);
    }
    memset( pLine, 0, LEN_LINE );
    letra = getc(pFile);
    while( letra != EOF ) {
      if( letra != '\n' ){
        pLine[i] = letra;
        i++;
      } else {
        sscanf (pLine, "%d;%*s", &number);
        pIds[pos] = number;
        pos++;
        i = 0;
        memset( pLine, 0, LEN_LINE );
      }
      letra = getc(pFile);
    }
    fclose(pFile);

    printf("Hay que evaluar: %d registros\n", pos);

    pFile = NULL;
    pFile = fopen("enex_items_202201101947.csv", "r");
    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    long lSize = ftell (pFile);
    rewind (pFile);
    char * pBuffer = new char [lSize];
    memset( pBuffer, 0, lSize );
    // copy the file into the buffer:
    long result = fread(pBuffer, 1, lSize, pFile);
    if (result != lSize) {
      printf( "ERROR: %ld != %ld\n", lSize, result );
      exit(3);
    }
    printf("Cargo archivo enex_items_202201101947.csv de %ld bytes \n", lSize);
    fclose (pFile);

    // ahora busca
    void *pch = NULL;
    for( i = 0; i < pos ; i++) {
      memset( pLine, 0, LEN_LINE );
      sprintf(pLine,"|%d", pIds[i]);
      pch = strstr(pBuffer, pLine);
      // printf ("%s%sfound ... %.2f%\n",pLine,(pch!=NULL ?" ":" no "), ((float)i*100.0f/(float)pos));
      if( i%1000 == 0 )
        printf ("Procesados %.2f% [%d de %d]\n", ((float)i*100.0f/(float)pos), found, i );
      if( pch!=NULL ) found++;
      pch = NULL;
    }
    printf("Se encontraron %d de %d\n", found, pos);

    delete [] pIds;
    delete [] pLine;
    delete [] pBuffer;
    exit(EXIT_SUCCESS);
}
