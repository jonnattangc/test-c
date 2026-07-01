# lab_md5

Ejercicio de laboratorio de sockets con soporte de operación MD5 (estilo K&R, pre-C99).

## Estado: no compila

`Lab.c` incluye `"md5.h"`, que **no está presente en el repositorio**. Para compilarlo
hace falta conseguir/crear una implementación de MD5 (por ejemplo la clásica de RSA
Data Security, o adaptar `openssl/md5.h`) y colocarla como `md5.h` en este directorio.

## Compilar (una vez resuelto md5.h)

```sh
gcc Lab.c -o Lab -Wall -Wextra
```

## Ejecutar

```sh
./Lab
```
