#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <fcntl.h>

#define MAX_TOKENS 128
#define MAX_LINE 1024

/**
 * Shell que trata tanto comandos internos como comandos externos.
 * @author diesanz
 */

bool redireccion = false; // Flag para indicar si se debe redirigir la salida

/**
 * Imprime un mensaje de error genérico.
 */
void print_error_message(void) {
    char error_message[30] = "An error has occurred\n";
    fprintf(stderr, "%s", error_message);
}

/**
 * Maneja el comando 'exit'.
 * @param args Argumentos del comando.
 * @return 1 si hay argumentos adicionales, 0 si no.
 */
int exit2(char **args) {
    if (args[1] != NULL) {
        print_error_message(); // Error si hay argumentos adicionales
        return 1;
    } else {
        return 0; // Salida sin errores
    }
}

/**
 * Maneja el comando 'cd'.
 * @param args Argumentos del comando.
 * @return 1 para continuar ejecutando la shell.
 */
int cd(char **args) {
    char cwd[1024]; // Buffer para el directorio actual

    if (args[1] == NULL) {
        print_error_message(); // Error si no se especifica el directorio
    } else {
        if (chdir(args[1]) != 0) {
            print_error_message(); // Error si el directorio no existe
        } else {
            if (getcwd(cwd, sizeof(cwd)) == NULL) { // Obtener el directorio de trabajo actual
                print_error_message();
                exit(1);
            }
        }
    }
    return 1;
}

// Array de punteros a funciones para comandos internos
int (*posible_func[])(char **) = {
    &cd,
    &exit2
};

/**
 * Ejecuta el comando dado.
 * @param args Argumentos del comando.
 * @return 1 para continuar ejecutando la shell.
 */
int lsh_execute(char **args) {
    int posibles_programas = 2;

    // Comandos internos disponibles
    char *posibilidadesInternas[] = {
        "cd",
        "exit"
    };

    if (args[0] == NULL) {
        return 1; // Retorna si no hay comando
    }
    
    // Ejecutar comando interno si coincide
    for (int i = 0; i < posibles_programas; i++) {
        if (strcmp(args[0], posibilidadesInternas[i]) == 0) {
            return (posible_func[i](args));
        }
    }
    
    pid_t pid;
    int status;
    pid = fork(); // Crear un proceso hijo

    if (pid == 0) { // Proceso hijo
        if (redireccion) {
            int pos_txt = -1;
            int encontrado = 1;
            // Buscar la posición del operador de redirección '>'
            for (int i = 0; args[i] != NULL; i++) {
                if (strcmp(args[i], ">") == 0) {
                    if (encontrado && i != 0) {
                        encontrado = 0;
                        pos_txt = i + 1;
                    } else {
                        print_error_message();
                        exit(1);
                    }              
                }
            }
            
            if (args[pos_txt + 1] != NULL) {
                print_error_message();
            }
            
            if (pos_txt != -1) {
                int salida = open(args[pos_txt], O_WRONLY | O_CREAT | O_TRUNC, 0666);

                if (salida == -1) {
                    print_error_message();
                    exit(1);
                }

                close(STDOUT_FILENO); // Cerrar salida estándar

                if (dup2(salida, STDOUT_FILENO) == -1) {
                    perror("Error en la redireccion");
                    exit(EXIT_FAILURE);
                }

                // Limpiar los argumentos relacionados con la redirección
                for (int i = pos_txt - 1; args[i] != NULL; i++) {
                    free(args[i]);
                    args[i] = NULL;
                }

                close(salida);
            }

            redireccion = false;
        }
        
        if (execvp(args[0], args) == -1) { // Ejecutar el comando
            print_error_message();
            exit(1);
        }

    } else if (pid < 0) {
        perror("lsh");
        exit(EXIT_FAILURE);
    } else {
        // Esperar a que el proceso hijo termine
        do {
            pid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

/**
 * Divide la línea de entrada en tokens basados en delimitadores.
 * @param lin Línea de entrada.
 * @param delimitador Delimitador para dividir la línea.
 * @return Array de tokens.
 */
char **lsh_split_line(char *lin, char *delimitador) {
    int bufsize = MAX_TOKENS;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));

    if (!tokens) {
        perror("Error de asignacion de memoria");
        exit(EXIT_FAILURE);
    }    

    char *token = strtok(lin, delimitador);
    
    if (token == NULL) {
        tokens[position] = strdup(delimitador); 
        if (!tokens[position]) {
            perror("Error de asignacion de memoria");
            exit(EXIT_FAILURE);
        }
        position++;
    }

    while (token != NULL) {
        tokens[position] = strdup(token);
        if (!tokens[position]) {
            perror("Error de asignacion de memoria");
            exit(EXIT_FAILURE);
        }
        position++;

        // Reasignar memoria si es necesario
        if (position >= bufsize) {
            bufsize += MAX_TOKENS;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                perror("Error de asignacion de memoria");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, delimitador);
    }
    
    tokens[position] = NULL; // Finalizar el array de tokens
   
    return tokens;
}

/**
 * Lee una línea de entrada del usuario.
 * @return Línea leída.
 */
char *lsh_read_line(void) {
    char *line = NULL;
    ssize_t bufsize = 0;

    if (getline(&line, (size_t*)&bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS); // Salir si se alcanza el final de la entrada
        } else {
            perror("Error de lectura");
            exit(EXIT_FAILURE);
        }
    }
    
    return line;
}

int main(int argc, char **argv) {
    char cwd[1024]; // Buffer para el directorio actual
    char *line;
    char **args;
    char **comando;
    int status = 1;

    if (argc >= 2) { // Leer comandos desde archivos si se proporcionan argumentos
        if (argc > 2) {
            print_error_message(); // Error si se proporcionan más de un archivo
            exit(EXIT_FAILURE);
        }

        int contador = 0;
        while (contador < argc) {
            FILE *libroabierto = fopen(argv[contador + 1], "rb");

            if (libroabierto == NULL) {
                print_error_message();
                exit(1);
            }

            char lineaLibro[MAX_LINE];

            while (fgets(lineaLibro, MAX_LINE, libroabierto) != NULL) {
                args = lsh_split_line(lineaLibro, "&");

                // Ejecutar cada comando en la línea
                for (int i = 0; args[i] != NULL; i++) {
                    if (strcmp(args[i], "\n") != 0 && strlen(args[i]) != 0) {
                        if (strstr(args[i], ">") != NULL) {
                            redireccion = true;
                        }

                        comando = lsh_split_line(args[i], " \t\n\r\a");
                        if (!isspace(comando[0][0])) {
                            lsh_execute(comando);
                        }

                        // Liberar memoria
                        for (int j = 0; comando[j] != NULL; j++) {
                            free(comando[j]);
                        }
                        free(comando);
                    }
                    redireccion = false;
                }

                free(args);
            }

            fclose(libroabierto);
            contador++;
            argc--;
        }

    } else { // Leer comandos desde la entrada estándar
        do {
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("[%s]>>>>", cwd); // Mostrar el directorio actual en el prompt
            } else {
                perror("getcwd");
            }

            line = lsh_read_line();
            
            if (line != NULL && strcmp(line, "\n") != 0) {
                args = lsh_split_line(line, "&");
                
                if (args[0] != NULL && strcmp(args[0], "&") == 0 && args[1] == NULL) {
                    print_error_message();
                    printf("Soy null");
                    free(line);
                    free(args);
                } else {
                    // Ejecutar cada comando en la línea
                    for (int i = 0; args[i] != NULL; i++) {
                        if (strstr(args[i], ">") != NULL) {
                            redireccion = true;
                        }
                        
                        comando = lsh_split_line(args[i], " \t\n\r\a");
                        status = lsh_execute(comando);
                        
                        // Liberar memoria
                        for (int j = 0; comando[j] != NULL; j++) {
                            free(comando[j]);
                        }
                        free(comando);
                    }
                    free(line);
                    free(args);
                }
            } else {
                status = 1;
            }
            
            redireccion = false;

        } while (status);
    }

    return 0;
}
