# forwarder

Reenviador TCP (port forwarder) multihilo: acepta conexiones en `addr_listen:port_listen` y reenvía el tráfico hacia `addr_dst:port_dst`.

## Compilar

```sh
gcc forwarder.c -o forwarder -Wall -Wextra -Wpedantic -Werror -lpthread
```

## Ejecutar

```sh
./forwarder <addr_listen> <port_listen> <addr_dst> <port_dst>
```
