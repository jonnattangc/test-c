#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define LEN_LINE 1024
#define LEN_FILE_SIZE 800000000 // en MB

bool analize(const char *pCode, const char *pDate, const char *pFolder);
bool search(const char *pCode, const char *pDate, const char *pFolder1, const char *pFolder2, const char *pFolder3);

int main(int argc, char *argv[])
{
    char line[LEN_LINE];
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
    char sinext[LEN_LINE];
    memset( sinext, 0, LEN_LINE );
    memcpy( sinext, pNamefile, (strlen(pNamefile) - 4) );
    memset(outName, 0, LEN_LINE );
    memcpy(outName, sinext, strlen(sinext) );
    memcpy(&outName[strlen(sinext)], "_out.csv", 8 );
    pOutFile = fopen(outName, "w");
    if (pOutFile == NULL){
        printf("Error: No se pudo crear el archivo de salida\n");
        exit(EXIT_FAILURE);
    }
    memset(outNoFound, 0, LEN_LINE );
    memcpy(outNoFound, sinext, strlen(sinext) );
    memcpy(&outNoFound[strlen(sinext)], "_notfound.csv", 13 );
    pNoFoundFile = fopen(outNoFound, "w");
    if (pNoFoundFile == NULL){
        printf("Error: No se pudo crear el archivo de no encontrados\n");
        exit(EXIT_FAILURE);
    }

    int ret = fseek (pFile, 0, SEEK_END);
    if (ret != 0) {
      printf("Error: fseek retorno %d\n", ret);
      exit(EXIT_FAILURE);
    }

    size = ftell (pFile);
    fseek(pFile, 0, SEEK_SET);
    printf("Cargo archivo %s de %d bytes\n", pNamefile, size );
    memset( line, 0, LEN_LINE );

    while (fgets(line, LEN_LINE, pFile) != NULL) {
      memset( code, 0, LEN_LINE );
      memset( date, 0, LEN_LINE );
      memset( mccode, 0, LEN_LINE );
      monto = 0.0f;
      number = sscanf(line, "%[^,],%[^,],%f,%s", mccode, code, &monto, date);
      if( number == 4 ) {
        if(search(code, date, pFolder1, pFolder2, pFolder3)){
          fprintf(pOutFile, "%s\n", line);
        }else{
          printf("%s no encontrado en archivos\n", code);
          fprintf(pNoFoundFile, "%s, %s\n", date, code);
        }
        fflush(pOutFile);
        fflush(pNoFoundFile);
        i++;
      }
      memset( line, 0, LEN_LINE );
    }
    fclose(pFile);
    fclose(pOutFile);
    fclose(pNoFoundFile);
    printf("Proceso terminado, se procesaron %d lineas\n", i );
    printf("Archivo de salida: %s\n", outName );
    exit(EXIT_SUCCESS);
}


bool search(const char *pCode, const char *pDate, const char *pFolder1, const char *pFolder2, const char *pFolder3){
    // printf("Buscando [%s] en fecha [%s] \n", pCode, pDate );
    bool finded = analize(pCode, pDate, pFolder1);
    if( finded ) return true;
    if( pFolder2 != NULL ) {
      if( analize(pCode, pDate, pFolder2) ) return true;
    }
    if( pFolder3 != NULL ) {
      if( analize(pCode, pDate, pFolder3) ) return true;
    }
    return false;
}

bool analize( const char *pCode, const char *pDate, const char *pFolder ) {
  // printf("Buscando en [%s]\n", pFolder );
  DIR *pDir = NULL;
  char fileName[LEN_LINE];
  char *pBuffer = new char[LEN_FILE_SIZE];
  struct dirent *pEntry = NULL;
  bool finded = false;

  if (pFolder != NULL ) {
    pDir = opendir(pFolder);
    if (pDir == NULL) {
      printf("Error: No se pudo abrir la carpeta %s\n", pFolder );
      return finded;
    }
    while ((pEntry = readdir(pDir)) != NULL) {
      // printf("Abro archivos del directorio [%d]\n", pEntry->d_type);
      if( pEntry->d_type == DT_REG ) {
        //printf("Name Date [%s] [%s] ", pDate, pEntry->d_name);
        char *pResp = strstr(pEntry->d_name, pDate);
        char *pGz = strstr(pEntry->d_name, ".gz");
        //printf("Resp [%s]\n", pResp!=NULL?"OK":"NULL");
        if( pResp != NULL && pGz == NULL) {
          // printf("Analizo Archivo: %s\n", pEntry->d_name);
          memset( fileName, 0, LEN_LINE );
          memcpy( fileName, pFolder, strlen(pFolder) );
          memcpy( &fileName[strlen(pFolder)], pEntry->d_name, strlen(pEntry->d_name) );
          // printf("Archivo: %s se analizara\n", fileName);
          FILE *pFile = fopen(fileName, "r");
          if (pFile == NULL) {
              printf("Error: No se pudo abrir el archivo %s\n", fileName);
              continue;
          }
          memset( pBuffer, 0, LEN_FILE_SIZE );
          size_t result = fread(pBuffer, 1, LEN_FILE_SIZE, pFile);
          if (result <= 0) {
            printf( "ERROR: Leyendo archivo %s\n", pEntry->d_name );
            fclose(pFile);
            continue;
          }
          // no se considera may'usculas o min'usculas
          char *pValue = strstr(pBuffer, pCode);
          if( pValue != NULL ) {
            printf("%s encontrado en %s\n", pCode, fileName );
            finded = true;
          }
          fclose(pFile);

        }
      }
      if( finded ) break;
    }
    // Cerrar el directorio
    closedir(pDir);
  }
  delete [] pBuffer;
  return finded;
}
