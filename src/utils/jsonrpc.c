#include <json-glib/json-glib.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct to store response data
struct Memory
{
    char *response;
    size_t size;
};

// Callback function to write response data
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->response, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        // out of memory!
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

// Function to send JSON-RPC request
char *send_json_rpc_request(const char *url, const char *method, JsonNode *params)
{
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0};

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl)
    {
        // Create a JSON object for the JSON-RPC request
        JsonBuilder *builder = json_builder_new();
        json_builder_begin_object(builder);
        json_builder_set_member_name(builder, "jsonrpc");
        json_builder_add_string_value(builder, "2.0");
        json_builder_set_member_name(builder, "method");
        json_builder_add_string_value(builder, method);
        json_builder_set_member_name(builder, "params");

        if (params)
        {
            json_builder_add_value(builder, params);
        }
        else
        {
            JsonBuilder *array_builder = json_builder_new();
            json_builder_begin_array(array_builder);
            json_builder_end_array(array_builder);
            JsonNode *empty_array = json_builder_get_root(array_builder);
            json_builder_add_value(builder, empty_array);
            g_object_unref(array_builder);
        }

        json_builder_set_member_name(builder, "id");
        json_builder_add_int_value(builder, 1);
        json_builder_end_object(builder);

        JsonGenerator *generator = json_generator_new();
        JsonNode *root = json_builder_get_root(builder);
        json_generator_set_root(generator, root);
        gchar *json_rpc_data = json_generator_to_data(generator, NULL);

        g_object_unref(generator);
        g_object_unref(builder);
        json_node_free(root);

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
        g_free(json_rpc_data);
    }

    curl_global_cleanup();

    return chunk.response;
}
