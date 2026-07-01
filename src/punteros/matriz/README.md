# matriz

Ejercicio de manejo de matrices con punteros.

## Compilar

`main` declara `argc`/`argv` sin usarlos, lo que rompe los flags estrictos del
proyecto, por lo que se compila sin `-Werror`:

```sh
gcc matriz.c -o matriz -Wall -Wextra -Wpedantic
```

## Ejecutar

```sh
./matriz
```
