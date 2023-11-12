#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <jansson.h>

size_t escribirRespuestaClima(void *contenido, size_t tamaño, size_t nmemb, void *usuario)
{
    size_t total_size = tamaño * nmemb;
    char *respuesta = (char *)usuario;

    strncat(respuesta, (char *)contenido, total_size);
    return total_size;
}

void procesarClimaJSON(const char *json_data)
{
    json_t *root;
    json_error_t error;

    char *main_str = NULL;
    char *description_str = NULL;
    double temp;
    double temp_celsius;

    // parseamos el json antes de usarlo
    root = json_loads(json_data, 0, &error);
    // verificamos errores en el json
    if (!root)
    {
        fprintf(stderr, "error en la linea %d: %s\n", error.line, error.text);
    }
    // obtenemos el objeto weather
    json_t *weather = json_object_get(root, "weather");
    if (json_is_array(weather) && json_array_size(weather) > 0)
    {
        json_t *weather_entry = json_array_get(weather, 0);

        // obtenemos las variables main y descripcion del objeto weather
        main_str = (char *)json_string_value(json_object_get(weather_entry, "main"));
        description_str = (char *)json_string_value(json_object_get(weather_entry, "description"));
    }
    else
    {
        fprintf(stderr, "error: el objeto weather del array no es valido\n");
    }

    // obtenemos el objeto main del arreglo principal
    json_t *main_object = json_object_get(root, "main");

    // Verificamos si main es un objeto
    if (json_is_object(main_object))
    {
        // obtenemos info de la propiedad temp en el objeto main
        temp = json_real_value(json_object_get(main_object, "temp"));
        temp_celsius = temp - 273.15;
    }
    else
    {
        fprintf(stderr, "error: el objeto main no es valido\n");
    }

    printf("El clima actual es: %s, %s, y la temperatura es %.2f\u00b0C\n", main_str, description_str, temp_celsius);

    // limpiamos el root
    json_decref(root);
}

void obtenerDatosClima(const char *parametros)
{
    CURL *curl;
    CURLcode res;

    char apiKey[] = "cfe0c0ed03500dbf74ca855903993da5"; // Reemplaza con tu clave de API de OpenWeatherMap
    char ciudad[100];                                   // Proporciona la ciudad de la que deseas obtener el clima

    if (sscanf(parametros, "%s", ciudad) != 1)
    {
        char respuesta[] = "Formato de solicitud de clima incorrecto. Usa 'hoy' o 'mañana' seguido del nombre de la ciudad";
        write(STDOUT_FILENO, respuesta, strlen(respuesta));
        return;
    }

    char url[256];
    snprintf(url, sizeof(url), "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s", ciudad, apiKey);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");

        // Almacenar la respuesta en un buffer
        char respuesta[4096];
        memset(respuesta, 0, sizeof(respuesta));

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, respuesta);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuestaClima);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Error en la solicitud HTTP: %s\n", curl_easy_strerror(res));
        }
        else
        {
            // Aquí puedes procesar la respuesta JSON y mostrar los datos climáticos según tus necesidades
            procesarClimaJSON(respuesta);
        }

        curl_easy_cleanup(curl);
    }
    else
    {
        fprintf(stderr, "Error al inicializar libcurl\n");
    }
}
