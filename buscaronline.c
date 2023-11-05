#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

size_t escribirRespuesta(void *contenido, size_t tamaño, size_t nmemb, void *usuario) {
    size_t total_size = tamaño * nmemb;
    char *respuesta = (char *)usuario;

    strncat(respuesta, (char *)contenido, total_size);
    return total_size;
}

void buscarEnLinea(const char *busqueda) {
    CURL *curl;
    CURLcode res;

    char url[256];
    snprintf(url, sizeof(url), "https://www.google.com/search?q=%s", busqueda);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");

        char respuesta[4096]; // Tamaño del buffer para la respuesta
        memset(respuesta, 0, sizeof(respuesta)); // Inicializamos el buffer

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, respuesta);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Error en la solicitud HTTP: %s\n", curl_easy_strerror(res));
        } else {
            // Utiliza lynx para convertir la respuesta HTML a texto
            FILE *pipe = popen("lynx -dump -stdin", "w");
            if (pipe) {
                fprintf(pipe, "%s", respuesta);
                pclose(pipe);
            }
        }

        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Error al inicializar libcurl\n");
    }
}
