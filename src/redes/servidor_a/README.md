# servidor_a

Servidor TCP concurrente con hilos y semáforos (estilo K&R, pre-C99).

## Compilar

Código legado; compila con advertencias pero no con los flags estrictos del proyecto:

```sh
gcc servidor_a.c -o servidor_a -Wall -Wextra -lpthread
```

## Ejecutar

```sh
./servidor_a
```
