#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void mostrarAyudaGoogler() {
    printf("\n---- Ayuda ----\n");
    printf("  n, p                 Obtener los siguientes o anteriores resultados de búsqueda\n");
    printf("  index                Abrir el resultado correspondiente al índice en el navegador\n");
    printf("  f                    Ir a la primera página\n");
    printf("  o [index|range|a ...] Abrir resultados separados por espacios, rangos numéricos\n");
    printf("                       o 'a' para abrir todos, en el navegador\n");
    printf("  O [index|range|a ...] Similar a 'o', pero intenta abrir en un navegador GUI\n");
    printf("  g keywords           Nueva búsqueda de Google para 'keywords' con opciones originales\n");
    printf("  c index              Copiar URL al portapapeles\n");
    printf("  u                    Alternar expansión de URL\n");
    printf("  q, ^D, doble Enter   Salir de Googler\n");
    printf("  ?                    Mostrar ayuda de omniprompt\n");
    printf("  *                    Otros comandos realizan una nueva búsqueda con opciones originales\n");
    printf("\nPara cerrar la búsqueda en Googler, presiona 'q', ^D, o doble Enter.\n");
    printf("---- Fin de la Ayuda ----\n\n");
}

void buscarEnLinea(const char *busqueda) {
    char comando[256];
    snprintf(comando, sizeof(comando), "googler %s", busqueda);

    printf("Realizando búsqueda en línea: %s\n", comando);

    // Ejecutar el comando y capturar la salida
    FILE *pipe = popen(comando, "r");
    if (pipe) {
        mostrarAyudaGoogler();
        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
            printf("%s", buffer); // Imprimir cada línea de la salida
        }
        
        pclose(pipe);
    } else {
        fprintf(stderr, "Error al ejecutar el comando\n");
    }
    
}
