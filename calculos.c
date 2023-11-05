#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

void realizarCalculo(const char *parametros) {
    float num1, num2;
    char operador;
    float resultado;

    if (sscanf(parametros, "%f %c %f", &num1, &operador, &num2) != 3) {
        char respuesta[] = "Formato de cálculo incorrecto. Use <número> <operador> <número>";
        write(STDOUT_FILENO, respuesta, strlen(respuesta) + 1);
        return;
    }

    switch (operador) {
        case '+':
            resultado = num1 + num2;
            break;
        case '-':
            resultado = num1 - num2;
            break;
        case '*':
            resultado = num1 * num2;
            break;
        case '/':
            if (num2 == 0) {
                char respuesta[] = "Error: División por cero";
                write(STDOUT_FILENO, respuesta, strlen(respuesta) + 1);
                return;
            } else {
                resultado = num1 / num2;
            }
            break;
        default:
            char respuesta[] = "Operador no válido. Use +, -, *, o /";
            write(STDOUT_FILENO, respuesta, strlen(respuesta) + 1);
            return;
    }

    char respuesta[100];
    snprintf(respuesta, sizeof(respuesta), "Resultado: %.3f", resultado);
    write(STDOUT_FILENO, respuesta, strlen(respuesta) + 1);
}
