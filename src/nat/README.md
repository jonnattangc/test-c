# nat

Experimento de bajo nivel con sockets raw (`AF_PACKET`/`SOCK_RAW`) e interfaces de red, orientado a NAT.

## Estado: no compila en Linux

Incluye `<net/if_dl.h>` (`struct sockaddr_dl`), que es un header de BSD/macOS y no
existe en glibc/Linux. Tal como está, solo compila en macOS.

## Compilar (macOS)

```sh
g++ nat.cpp -o nat -Wall -Wextra -Wpedantic -Werror -std=c++17
```

## Ejecutar

Requiere privilegios para abrir sockets raw:

```sh
sudo ./nat [argumento]
```
