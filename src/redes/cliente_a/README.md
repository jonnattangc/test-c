# cliente_a

Cliente TCP simple (estilo K&R, pre-C99): se conecta a un host:puerto, envía una línea leída de stdin y muestra la respuesta.

## Compilar

Código legado; compila con advertencias pero no con los flags estrictos del proyecto:

```sh
gcc cliente_a.c -o cliente_a -Wall -Wextra
```

## Ejecutar

```sh
./cliente_a <host_o_ip> <puerto>
```
