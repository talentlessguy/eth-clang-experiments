#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ethc/keccak256.h>
#include <ethc/hex.h>
#include <ethc/abi.h>
#include "./utils/jsonrpc.h"

char *labelhash(const char *label)
{
    uint8_t result[32] = {0};
    char *hash = NULL;

    if (!label || strlen(label) == 0)
    {
        eth_hex_from_bytes(&hash, result, 32);
        return hash;
    }

    if (strlen(label) == 66 && label[0] == '[' && label[65] == ']')
    {
        char encoded_hash[65];
        strncpy(encoded_hash, label + 1, 64);
        encoded_hash[64] = '\0';

        uint8_t *hash_bytes = NULL;
        if (1 == eth_hex_to_bytes(&hash_bytes, encoded_hash, 64))
        {
            hash = (char *)malloc(67);
            sprintf(hash, "0x%s", encoded_hash);
            free(hash_bytes);
            return hash;
        }
    }

    // Otherwise, compute the keccak256 hash of the label
    size_t label_len = strlen(label);
    uint8_t *label_bytes = (uint8_t *)malloc(label_len);
    memcpy(label_bytes, label, label_len);

    eth_keccak256(result, label_bytes, label_len);
    eth_hex_from_bytes(&hash, result, 32);

    free(label_bytes);
    return hash;
}

char *namehash(const char *name)
{
    uint8_t result[32] = {0};
    char *final_hash = NULL;
    char *labels_copy = strdup(name);
    char *token;
    char *labels[256]; // Array to hold labels, assuming a maximum of 256 labels
    int label_count = 0;

    // Split the name by '.' and store labels
    token = strtok(labels_copy, ".");
    while (token != NULL)
    {
        labels[label_count++] = token;
        token = strtok(NULL, ".");
    }

    // Iterate over labels in reverse order
    for (int i = label_count - 1; i >= 0; i--)
    {
        char *label = labels[i];
        char *hash = labelhash(label);

        uint8_t *hash_bytes = NULL;
        eth_hex_to_bytes(&hash_bytes, hash, strlen(hash));

        // Concatenate current result and label hash
        uint8_t combined[64];
        memcpy(combined, result, 32);
        memcpy(combined + 32, hash_bytes, 32);

        // Compute keccak256 hash of the combined array
        eth_keccak256(result, combined, 64);

        free(hash_bytes);
        free(hash);
    }

    eth_hex_from_bytes(&final_hash, result, 32);
    free(labels_copy);
    return final_hash;
}

int main()
{
    size_t hexlen;
    const char *name = "v1rtl.eth";
    printf("ENS name: %s\n", name);
    char *hash = namehash(name);
    if (!hash)
    {
        fprintf(stderr, "Error computing name hash!\n");
        return 1;
    }
    printf("namehash: 0x%s\n", hash);

    uint8_t *hash_bytes = NULL;
    eth_hex_to_bytes(&hash_bytes, hash, strlen(hash));

    struct eth_abi abi;
    char *fn = "addr(bytes32)", *hex;
    eth_abi_init(&abi, ETH_ABI_ENCODE);
    eth_abi_call(&abi, &fn, NULL);     // addr(
    eth_abi_bytes32(&abi, hash_bytes); //   0x6dd56164f699a101d6063add452dfed7c6c09fe17b8e4acf3328f9387f5030b9
    eth_abi_call_end(&abi);            // )

    eth_abi_to_hex(&abi, &hex, &hexlen);
    eth_abi_free(&abi);
    printf("Encoded ABI: 0x%s\n", hex); // 3b3b57dee0920cc7aa5a000000000000000000000051f908b2afa579f053c63afe7f0000 abi: 70a08231000000000000000000000000876d477bd5cd050e6162cf757e1bc02d93cdc0fe

    char hexWith0x[256];
    sprintf(hexWith0x, "0x%s", hex);

    const char *url = "https://rpc.payload.de";

    cJSON *params_array = cJSON_CreateArray();
    cJSON *params_object = cJSON_CreateObject();
    cJSON_AddStringToObject(params_object, "to", "0x231b0Ee14048e9dCcD1d247744d114a4EB5E8E63");
    cJSON_AddStringToObject(params_object, "data", hexWith0x);
    cJSON_AddItemToArray(params_array, params_object);

    char *response = send_json_rpc_request(url, "eth_call", params_array);
    if (response)
    {
        printf("Response: %s\n", response);

        cJSON *root = cJSON_Parse(response);
        if (root == NULL)
        {
            printf("Error before: [%s]\n", cJSON_GetErrorPtr());
            return 1;
        }

        // Get values from the JSON object
        cJSON *jsonrpc = cJSON_GetObjectItem(root, "jsonrpc");
        cJSON *id = cJSON_GetObjectItem(root, "id");
        cJSON *result = cJSON_GetObjectItem(root, "result");

        // Trim leading zeros after "0x" in the result field
        const char *trimmed_result = result->valuestring;
        if (strncmp(trimmed_result, "0x", 2) == 0)
        {
            trimmed_result += 2; // Move the pointer past "0x"
            while (*trimmed_result == '0')
            {
                trimmed_result++; // Skip leading zeros
            }
        }

        printf("Address: %s\n", trimmed_result);

        // Free memory
        cJSON_Delete(root);
    }

    return 0;
}