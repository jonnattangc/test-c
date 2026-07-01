# http_server

Servidor HTTP concurrente en C, multihilo (`http_server.c`), con un manejador de
conexiones/cliente en `http_client.c`/`http_client.h` y utilidades (parsing con
regex, etc.) en `http_utils.c`/`http_utils.h`.

## Compilar

```sh
gcc http_server.c http_client.c http_utils.c -o http_server -Wall -Wextra -Wpedantic -Werror -lpthread
```

## Ejecutar

```sh
./http_server <addr> <port>
```
