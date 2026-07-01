# ip_fragment

Fragmento de código de resolución de dirección IP (estilo K&R).

## Estado: no compila

`ip.c` **no es un programa completo**: es un cuerpo de sentencias suelto sin una
función que lo contenga (no tiene `main`), por lo que falla al compilar. Se conserva
como referencia/snippet. Para usarlo, hay que envolver su contenido en una función
(por ejemplo `main`) e incluir `<string.h>` y `<stdio.h>`.

## Compilar (tras envolverlo en una función)

```sh
gcc ip.c -o ip -Wall -Wextra
```
