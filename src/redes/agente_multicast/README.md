# agente_multicast

Laboratorio de transferencia de archivos vía multicast entre "agentes". `agente.c`
llama a las funciones `leer_archivo`, `cambiar_dato`, `tiempo_listo` y `ran`, definidas
en `funciones.c`, y ambas usan `maestro.dat` como archivo de estado compartido (debe
estar en el directorio de trabajo al ejecutar). En tiempo de ejecución, `agente.c`
además compila (`gcc -o servidor servidor.c`) y lanza un binario `servidor` — para eso
necesitará una copia de `../servidor/servidor.c` (y `socket.c`) en este mismo directorio.

`agente_a.c` es un borrador/variante anterior e **incompleto**: no compila (usa una
variable `control` no declarada y tiene llaves desbalanceadas). Se conserva como referencia.

## Compilar

```sh
gcc agente.c funciones.c -o agente -Wall -Wextra
```

## Ejecutar

```sh
./agente <puerto>
```
