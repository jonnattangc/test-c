# data_validate

Validación de datos de entrada.

## Compilar

Tiene un `printf` con formato inválido y una variable sin usar que rompen los flags
estrictos del proyecto, por lo que se compila sin `-Werror`:

```sh
g++ data_validate.cpp -o data_validate -Wall -Wextra -std=c++17
```

## Ejecutar

```sh
./data_validate
```
