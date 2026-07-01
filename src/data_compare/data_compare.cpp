#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define LEN_LINE 1024
#define LEN_FILE_SIZE 800 * 1024 * 1024 // en MB

bool search(char *pCode, char *pDate, char *pFolder1, char *pFolder2, char *pFolder3);
bool analize(char *pCode, char *pDate, char *pFolder);

int main(int argc, char *argv[])
{
    char *pLine = new char[LEN_LINE];
    char code[LEN_LINE], date[LEN_LINE], mccode[LEN_LINE], outName[LEN_LINE], outNoFound[LEN_LINE];
    float monto = 0.0f;
    int number = 0, i=0, size = 0;
    char *pNamefile = NULL;
    char *pFolder1 = NULL, *pFolder2 = NULL, *pFolder3 = NULL;
    FILE *pFile = NULL, *pOutFile = NULL, *pNoFoundFile = NULL;
    
    printf("Iniciando...\n");
    if( argc > 2 ) {
      pNamefile = (char *) &argv[1][0];
      pFolder1 = (char *) &argv[2][0];
      if( argc > 3 )
        pFolder2 = (char *) &argv[3][0];
      if( argc > 4 )
        pFolder3 = (char *) &argv[4][0];
      
      printf("Nombre de archivo a procesar: %s\n", pNamefile );
      printf("Carpeta 1: %s Carpeta 2: %s Carpeta 3: %s\n", pFolder1, pFolder2!=NULL?pFolder2:"Nada", pFolder3!=NULL?pFolder3:"Nada" );
    } else {
      printf("Debe indicar el nombre del archivo a procesar y la(s) carpeta de logs de entrada\n");
      exit(EXIT_FAILURE);
    } 

    pFile = fopen(pNamefile, "r");
    if (pFile == NULL){
        printf("Error: No se pudo abrir el archivo\n");
        exit(EXIT_FAILURE);
    }
    // le sacamos las la extension
    char *pExt = new char[LEN_LINE];
    memset( pExt, 0, LEN_LINE );
    memcpy( pExt, pNamefile, (strlen(pNamefile) - 4) );
    memset(outName, 0, LEN_LINE );
    memcpy(outName, pExt, strlen(pExt) );
    memcpy(&outName[strlen(pExt)], "_out.csv", 8 );
    pOutFile = fopen(outName, "w");
    if (pOutFile == NULL){
        printf("Error: No se pudo crear el archivo de salida\n");
        exit(EXIT_FAILURE);
    }
    memset(outNoFound, 0, LEN_LINE );
    memcpy(outNoFound, pExt, strlen(pExt) );
    memcpy(&outNoFound[strlen(pExt)], "_notfound.csv", 13 );
    pNoFoundFile = fopen(outNoFound, "w");
    if (pNoFoundFile == NULL){
        printf("Error: No se pudo crear el archivo de no encontrados\n");
        exit(EXIT_FAILURE);
    }
    delete [] pExt;

    int ret = fseek (pFile, 0, SEEK_END); 
    if (ret != 0) {
      printf("Error: fseek retorno %d\n", ret);
      exit(EXIT_FAILURE);
    }

    size = ftell (pFile);
    fseek(pFile, 0, SEEK_SET);
    printf("Cargo archivo %s de %d bytes\n", pNamefile, size );
    memset( pLine, 0, LEN_LINE );

    while (fgets(pLine, LEN_LINE, pFile) != NULL) {
      memset( code, 0, LEN_LINE );
      memset( date, 0, LEN_LINE );
      memset( mccode, 0, LEN_LINE );
      monto = 0.0f;
      number = sscanf(pLine, "%[^,],%[^,],%f,%s", mccode, code, &monto, date);
      if( number == 4 ) {
        i++;
        if(search(code, date, pFolder1, pFolder2, pFolder3)){
          fprintf(pOutFile, "%s\n", pLine);
        }else{
          fprintf(pNoFoundFile, "%s, %s\n", date, code);
        }
      }
      memset( pLine, 0, LEN_LINE );
    }
    fclose(pFile);
    fclose(pOutFile);
    fclose(pNoFoundFile);
    delete [] pLine;
    printf("Proceso terminado, se procesaron %d lineas\n", i );
    printf("Archivo de salida: %s\n", outName );
    exit(EXIT_SUCCESS);
}


bool search(char *pCode, char *pDate, char *pFolder1, char *pFolder2, char *pFolder3){
    // printf("Buscando %s en fecha %s\n", pCode, pDate );
    if( analize(pCode, pDate, pFolder1) ) return true;
    if( pFolder2 != NULL ) {
      if( analize(pCode, pDate, pFolder2) ) return true;
    }
    if( pFolder3 != NULL ) {
      if( analize(pCode, pDate, pFolder3) ) return true;
    }
    return false;
}

bool analize(char *pCode, char *pDate, char *pFolder) {
  DIR *pDir;
  FILE *pFile = NULL;
  char * pfileName = new char[LEN_LINE];
  struct dirent *pEntry;  
  bool finded = false;
  if (pFolder != NULL ) {
      pDir = opendir(pFolder);
      if (pDir == NULL) {
        printf("Error: No se pudo abrir la carpeta %s\n", pFolder );
        return false;
      }
      while ((pEntry = readdir(pDir)) != NULL) {
        if( pEntry->d_type == DT_REG ) {
          if( strstr(pEntry->d_name, pDate) != NULL ) {
            memset( pfileName, 0, LEN_LINE );
            memcpy( pfileName, pFolder, strlen(pFolder) );
            memcpy( &pfileName[strlen(pFolder)], pEntry->d_name, strlen(pEntry->d_name) );
            // printf("Archivo: %s Se considera\n", pfileName);
            pFile = fopen(pfileName, "rb");
            if (pFile == NULL){
                printf("Error: No se pudo abrir el archivo %s\n", pfileName);
                continue;
            }
            char *pBuffer = new char [LEN_FILE_SIZE];
            memset( pBuffer, 0, LEN_FILE_SIZE );
            size_t result = fread(pBuffer, 1, LEN_FILE_SIZE, pFile);
            if (result <= 0) {
              printf( "ERROR: Leyendo archivo %s\n", pEntry->d_name );
              fclose(pFile);
              delete [] pBuffer;
              continue;
            }
            // no se considera may'usculas o min'usculas
            char *pValue = strstr(pBuffer, pCode);
            if( pValue != NULL ) {
              printf("%s encontrado en %s\n", pCode, pfileName );
              finded = true;
            }
            fclose(pFile);
            delete [] pBuffer;
          }
        }
        if( finded ) break;
      }
      // Cerrar el directorio
      closedir(pDir);
      delete [] pfileName;
    }
  return finded;
}