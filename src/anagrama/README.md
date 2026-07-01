# anagrama

Verificador de anagramas.

## Compilar

Tiene comparaciones `int`/`size_t` con signo distinto que rompen los flags estrictos
del proyecto, por lo que se compila sin `-Werror`:

```sh
g++ anagrama.cpp -o anagrama -Wall -Wextra -std=c++17
```

## Ejecutar

```sh
./anagrama
```
