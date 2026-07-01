# latency_measure

Medición de latencia contra una dirección IP y puerto.

## Estado: no compila

Usa `EINPROGRESS`, `EWOULDBLOCK` y `ETIMEDOUT` sin incluir `<errno.h>`. Hay que
agregar `#include <errno.h>` para que compile.

## Compilar (tras agregar `#include <errno.h>`)

```sh
gcc latency_measure.c -o latency_measure -Wall -Wextra -Wpedantic -Werror
```

## Ejecutar

```sh
./latency_measure <direccion_ip> <puerto>
```
