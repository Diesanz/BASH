Simple Shell.
Este proyecto es una implementación básica de una shell en C. La shell soporta comandos internos, como cd y exit, y comandos externos, además de redirección de salida.

Características
Comandos Internos:

cd: Cambia el directorio de trabajo.
exit: Sale de la shell.
Comandos Externos: Ejecuta cualquier comando disponible en el sistema.

Redirección de Salida: Permite redirigir la salida estándar de un comando a un archivo utilizando el operador >.

Lectura de Comandos:

Desde Entrada Estándar: Permite ingresar comandos directamente en la shell.
Desde Archivo: Permite leer y ejecutar comandos desde un archivo de texto.
Compilación
Para compilar el programa, usa el siguiente comando en la terminal:

gcc -o simple_shell simple_shell.c
Uso
Ejecutar la Shell
Una vez compilado, ejecuta el programa con el siguiente comando:

./simple_shell
Esto abrirá una shell interactiva donde podrás ingresar comandos.

Comandos Internos
cd <directorio>: Cambia el directorio de trabajo actual a <directorio>. Si no se proporciona un directorio, se muestra un mensaje de error.
exit: Sale de la shell. Si se proporcionan argumentos, se muestra un mensaje de error.
Comandos Externos
Simplemente ingresa el nombre del comando y sus argumentos como lo harías en cualquier otra shell.

Redirección de Salida
Para redirigir la salida de un comando a un archivo, usa el operador > seguido del nombre del archivo:

comando > archivo.txt
Lectura desde Archivo
Puedes ejecutar comandos desde un archivo especificando el nombre del archivo como argumento al iniciar la shell:

./simple_shell archivo_de_comandos.txt
Ejemplos
Cambiar directorio:

cd /home/user
Ejecutar un comando externo y redirigir su salida:

ls > lista_de_archivos.txt
Leer comandos desde un archivo:

./simple_shell comandos.txt
Contribución
Si deseas contribuir al proyecto, por favor, haz un fork del repositorio y envía un pull request con tus mejoras o correcciones.

Licencia
Este proyecto está licenciado bajo la Licencia MIT.
