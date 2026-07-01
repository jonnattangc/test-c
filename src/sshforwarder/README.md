# sshforwarder

Forwarder de puertos a través de un túnel SSH usando `libssh`.

## Dependencias

Requiere la librería `libssh` instalada (paquete `libssh-dev` en Debian/Ubuntu).

## Compilar

```sh
g++ sshforwarder.cpp -o sshforwarder -Wall -Wextra -Wpedantic -Werror -std=c++17 -lssh
```

## Ejecutar

```sh
./sshforwarder
```
