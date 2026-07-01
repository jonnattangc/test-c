# eth_sapo

Sniffer de paquetes a nivel de enlace usando sockets raw (`AF_PACKET`/`SOCK_RAW`).
Es una copia idéntica de [`../../snifffer/snifffer.cpp`](../../snifffer/snifffer.cpp).

## Compilar

Tiene variables sin usar que rompen los flags estrictos del proyecto, por lo que se
compila sin `-Werror`:

```sh
g++ eth_sapo.cpp -o eth_sapo -Wall -Wextra -std=c++17
```

## Ejecutar

Requiere privilegios para abrir sockets raw:

```sh
sudo ./eth_sapo <interfaz>
```
