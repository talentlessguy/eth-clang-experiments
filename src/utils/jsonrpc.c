#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

struct Memory
{
    char *response;
    size_t size;
};

// Callback function to handle the response
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        fprintf(stderr, "Not enough memory to allocate buffer.\n");
        return 0;
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

// Function to send JSON-RPC request
char *send_json_rpc_request(const char *url, const char *method, cJSON *params)
{
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0};

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        cJSON *json_rpc_request = cJSON_CreateObject();
        cJSON_AddStringToObject(json_rpc_request, "jsonrpc", "2.0"); // Automatically add "jsonrpc" key
        cJSON_AddStringToObject(json_rpc_request, "method", method);

        if (params)
        {
            cJSON_AddItemToObject(json_rpc_request, "params", params);
        }
        else
        {
            cJSON_AddItemToObject(json_rpc_request, "params", cJSON_CreateArray());
        }

        cJSON_AddNumberToObject(json_rpc_request, "id", 1);

        char *json_rpc_data = cJSON_PrintUnformatted(json_rpc_request);
        cJSON_Delete(json_rpc_request);

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_rpc_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        free(json_rpc_data);
    }

    curl_global_cleanup();

    return chunk.response;
}
