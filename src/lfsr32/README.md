# LFSR32

Generador de secuencia con registro de desplazamiento con retroalimentación lineal (LFSR) de 32 bits.

## Compilar

El propio archivo trae un comentario del autor ("Tiene un error, pero no encuentro
cual seria...") — el `printf("%c,%c,%c,%c", &bit25, ...)` pasa punteros a `%c` en vez
de los valores `char`, lo que rompe los flags estrictos del proyecto. Se compila sin
`-Werror`:

```sh
g++ LFSR32.cpp -o LFSR32 -Wall -Wextra -std=c++17
```

## Ejecutar

```sh
./LFSR32
```
