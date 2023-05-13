#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cJSON/cJSON.h>

int main()
{
    CURL *curl;
    CURLcode res;
    char *url = "http://api.weatherapi.com/v1/current.json?key=<cd2aed2790b64f01b29151853231305>&q=New York City&aqi=no";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        /* Define our callback to get called when there's data to be written */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);

        /* Set a user-agent */
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        /* Set the output file */
        FILE *fp;
        fp = fopen("weather.json", "wb");
        if (fp == NULL) {
            printf("File cannot be opened");
            return 0;
        }
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        }
        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    /* Parse the response */
    FILE *json_file = fopen("weather.json", "rb");
    fseek(json_file, 0, SEEK_END);
    long json_file_size = ftell(json_file);
    fseek(json_file, 0, SEEK_SET);

    char *json_str = malloc(json_file_size + 1);
    fread(json_str, 1, json_file_size, json_file);
    fclose(json_file);

    cJSON *root = cJSON_Parse(json_str);
    cJSON *location = cJSON_GetObjectItem(root, "location");
    cJSON *current = cJSON_GetObjectItem(root, "current");

    /* Extract the desired weather data */
    char *city = cJSON_GetObjectItem(location, "name")->valuestring;
    char *country = cJSON_GetObjectItem(location, "country")->valuestring;
    double temp_c = cJSON_GetObjectItem(current, "temp_c")->valuedouble;
    char *condition = cJSON_GetObjectItem(current, "condition")->valuestring;

    /* Display the weather data */
    printf("Current weather in %s, %s:\n", city, country);
    printf("Temperature: %.1f degrees Celsius\n", temp_c);
    printf("Condition: %s\n", condition);

    /* Free memory */
    cJSON_Delete(root);
    free(json_str);

    return 0;
}

