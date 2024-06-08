#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ethc/keccak256.h>
#include <ethc/hex.h>
#include <ethc/abi.h>

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
    char *hash = namehash(name);
    printf("namehash: 0x%s\n", hash);

    struct eth_abi abi;
    char *fn = "addr(bytes32)", *hex;
    eth_abi_init(&abi, ETH_ABI_ENCODE);
    eth_abi_call(&abi, &fn, NULL); // balanceOf(
    eth_abi_bytes32(&abi, &hash);  //   0x6dd56164f699a101d6063add452dfed7c6c09fe17b8e4acf3328f9387f5030b9
    eth_abi_call_end(&abi);        // )

    eth_abi_to_hex(&abi, &hex, &hexlen);
    eth_abi_free(&abi);
    printf("encoded abi: %s\n", hex); // 3b3b57dee0920cc7aa5a000000000000000000000051f908b2afa579f053c63afe7f0000 abi: 70a08231000000000000000000000000876d477bd5cd050e6162cf757e1bc02d93cdc0fe
    free(hex);
    return 0;
}