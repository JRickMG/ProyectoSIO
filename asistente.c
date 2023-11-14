#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void buscarEnLinea(const char *parametros);
void realizarCalculo(const char *parametros);
void obtenerDatosClima(const char *parametros);

int main() {
    char entrada[100];
    char comando[50];
    char parametros[50];

    printf("Asistente Virtual - Comandos disponibles:\n");
    printf("  - Para buscar en línea: buscar <término de búsqueda>\n");
    printf("  - Para realizar cálculos: calcular <expresión matemática>\n");
    printf("  - Para obtener datos climáticos: clima <ciudad>\n");
    printf("Ejemplos de comandos válidos:\n");
    printf("  - buscar gatos\n");
    printf("  - calcular 5 + 3\n");
    printf("  - clima hoy New York\n");

    while (1) {
        printf("Ingrese un comando: ");
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            break;  // Salir si no se puede leer la entrada
        }

        entrada[strcspn(entrada, "\n")] = '\0';

        if (sscanf(entrada, "%s %[^\n]", comando, parametros) == 2) {
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("Error al crear la tubería");
                exit(EXIT_FAILURE);
            }

            pid_t child_pid = fork();
            if (child_pid == -1) {
                perror("Error al crear el proceso hijo");
                exit(EXIT_FAILURE);
            }

            if (child_pid == 0) {
                // Este es el proceso hijo
                close(pipefd[0]); // Cerramos el extremo de lectura de la tubería
                if (strcmp(comando, "buscar") == 0) {
		            buscarEnLinea(parametros);
                } else if (strcmp(comando, "calcular") == 0) {
                    realizarCalculo(parametros);
                } else if (strcmp(comando, "clima") == 0) {
                    obtenerDatosClima(parametros);
                } else {
                    char respuesta[] = "Comando no válido";
                    write(pipefd[1], respuesta, strlen(respuesta) + 1);
                }

                close(pipefd[1]); // Cierra el extremo de escritura de la tubería
                exit(EXIT_SUCCESS);
            } else {
                // Este es el proceso principal
                close(pipefd[1]); // Cerramos el extremo de escritura de la tubería

                char respuesta[4096];
                ssize_t bytes_leidos = read(pipefd[0], respuesta, sizeof(respuesta));
                respuesta[bytes_leidos] = '\0';
                printf(" Respuesta del comando '%s': %s\n", comando, respuesta);
                close(pipefd[0]);
                wait(NULL);
            }
        } else if (sscanf(entrada, "%s", comando) == 1) {
            printf("Comando: %s\n", comando);
            printf("Sin parámetros\n");
        } else {
            printf("Entrada no válida\n");
        }
    }

    return 0;
}
