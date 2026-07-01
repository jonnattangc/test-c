# cliente

Cliente UDP/multicast (se une al grupo `224.0.1.1:5000`) de un laboratorio de sockets (estilo K&R, anterior a C99).

## Compilar

Código legado que compila con advertencias pero no cumple el estándar estricto del
proyecto (usa `bzero`/`strlen` sin sus headers y falta `<string.h>`), por lo que
se compila sin `-Werror`:

```sh
gcc cliente.c -o cliente -Wall -Wextra
```

## Ejecutar

```sh
./cliente
```
