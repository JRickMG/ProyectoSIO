#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

size_t escribirRespuestaClima(void *contenido, size_t tamaño, size_t nmemb, void *usuario) {
    size_t total_size = tamaño * nmemb;
    char *respuesta = (char *)usuario;

    strncat(respuesta, (char *)contenido, total_size);
    return total_size;
}

void procesarClimaJSON(const char *json_data) {
    // Analizar el JSON manualmente (sin usar cJSON)
    char *ptr = strstr(json_data, "\"weather\":[{");
    if (ptr != NULL) {
        ptr += strlen("\"weather\":[{");

        char main[50] = "";
        char description[50] = "";

        sscanf(ptr, "\"main\":\"%49[^\"]\",\"description\":\"%49[^\"]\"", main, description);

        ptr = strstr(ptr, "\"temp\":");
        if (ptr != NULL) {
            double temp_kelvin;
            sscanf(ptr, "\"temp\":%lf", &temp_kelvin);

            // Convertir la temperatura de Kelvin a Celsius
            double temp_celsius = temp_kelvin - 273.15;

            // Imprimir la información del clima de manera legible
            printf("El clima actual es: %s, %s, y la temperatura es %.2f°C\n", main, description, temp_celsius);
        }
    }
}

void obtenerDatosClima(const char *parametros) {
    CURL *curl;
    CURLcode res;

    char apiKey[] = "cfe0c0ed03500dbf74ca855903993da5"; // Reemplaza con tu clave de API de OpenWeatherMap
    char ciudad[100]; // Proporciona la ciudad de la que deseas obtener el clima

    if (sscanf(parametros, "%s", ciudad) != 1) {
        char respuesta[] = "Formato de solicitud de clima incorrecto. Usa 'hoy' o 'mañana' seguido del nombre de la ciudad";
        write(STDOUT_FILENO, respuesta, strlen(respuesta));
        return;
    }

    char url[256];
    snprintf(url, sizeof(url), "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", ciudad, apiKey);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");

        // Almacenar la respuesta en un buffer
        char respuesta[4096];
        memset(respuesta, 0, sizeof(respuesta));

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, respuesta);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuestaClima);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Error en la solicitud HTTP: %s\n", curl_easy_strerror(res));
        } else {
            // Aquí puedes procesar la respuesta JSON y mostrar los datos climáticos según tus necesidades
            procesarClimaJSON(respuesta);
        }

        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Error al inicializar libcurl\n");
    }
}
