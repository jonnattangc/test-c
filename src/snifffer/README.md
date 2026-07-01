# snifffer

Sniffer de paquetes a nivel de enlace usando sockets raw (`AF_PACKET`/`SOCK_RAW`) sobre una interfaz de red.

## Compilar

Tiene variables sin usar que rompen los flags estrictos del proyecto, por lo que se
compila sin `-Werror`:

```sh
g++ snifffer.cpp -o snifffer -Wall -Wextra -std=c++17
```

## Ejecutar

Requiere privilegios para abrir sockets raw:

```sh
sudo ./snifffer <interfaz>
```
