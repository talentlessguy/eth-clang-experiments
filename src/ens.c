#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ethc/keccak256.h>
#include <ethc/hex.h>
#include <string.h>

char *labelhash(const char *label)
{
    uint8_t result[32] = {0};
    char *hash = NULL;

    // If the label is empty, return the zero-filled hash
    if (!label || strlen(label) == 0)
    {
        eth_hex_from_bytes(&hash, result, 32);
        return hash;
    }

    // Check if the label is in the encoded form [hash]
    if (strlen(label) == 66 && label[0] == '[' && label[65] == ']')
    {
        char encoded_hash[65];
        strncpy(encoded_hash, label + 1, 64);
        encoded_hash[64] = '\0';

        uint8_t *hash_bytes = NULL;
        if (eth_hex_to_bytes(&hash_bytes, encoded_hash, 64) == 0)
        {
            // If the conversion to bytes is successful
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
    uint8_t result[32] = {0}; // Initialize result as 32 bytes of zero
    char *final_hash = NULL;
    char *labels_copy = strdup(name); // Make a copy of the input name to tokenize
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

    // Convert final result to hexadecimal string
    eth_hex_from_bytes(&final_hash, result, 32);
    free(labels_copy);
    return final_hash;
}

int main()
{
    const char *name = "v1rtl.eth";
    char *hash = namehash(name);
    printf("namehash: 0x%s\n", hash);
    free(hash);
    return 0;
}