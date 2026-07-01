# cksumcrc32

Cálculo de checksum CRC32.

## Compilar

Tiene variables sin usar que rompen los flags estrictos del proyecto, por lo que se
compila sin `-Werror`:

```sh
g++ cksumcrc32.cpp -o cksumcrc32 -Wall -Wextra -std=c++17
```

## Ejecutar

```sh
./cksumcrc32
```
