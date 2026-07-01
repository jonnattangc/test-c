# servidor

Servidor TCP (estilo K&R, pre-C99). Requiere enlazarse junto con `socket.c`, que
provee las funciones auxiliares `socket_servidor`, `acepta_conexion` y `socket_cliente`
(así lo indica el propio comentario original del autor en `servidor.c`).

## Compilar

Código legado; compila con advertencias pero no con los flags estrictos del proyecto:

```sh
gcc servidor.c socket.c -o servidor -Wall -Wextra
```

## Ejecutar

```sh
./servidor
```
