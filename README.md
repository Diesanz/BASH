# Simple Shell

Este proyecto es una implementación básica de una shell en C. La shell soporta comandos internos, como `cd` y `exit`, y comandos externos, además de redirección de salida.

## Características

### Comandos Internos:
- **cd**: Cambia el directorio de trabajo.
- **exit**: Sale de la shell.

### Comandos Externos:
- Ejecuta cualquier comando disponible en el sistema.

### Redirección de Salida:
- Permite redirigir la salida estándar de un comando a un archivo utilizando el operador `>`.

## Lectura de Comandos:

### Desde Entrada Estándar:
- Permite ingresar comandos directamente en la shell.

### Desde Archivo:
- Permite leer y ejecutar comandos desde un archivo de texto.

## Compilación

Para compilar el programa, usa el siguiente comando en la terminal:

```bash
gcc -o simple_shell simple_shell.c
