#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <ethc/hex.h>
#include <ethc/abi.h>

#include "./jsonrpc.h"
#include "./ens.h"

char *decode_content_hash(const char *contentHash)
{
    const char *ipfsHash = contentHash + 4; // Skip the first 4 characters

    int ipfsHashLength = strlen(ipfsHash);
    uint8_t *bytes;

    // Convert hex string to bytes
    eth_hex_to_bytes(&bytes, ipfsHash, ipfsHashLength);

    const char *alphabet = "abcdefghijklmnopqrstuvwxyz234567";
    int byteArrayLength = ipfsHashLength / 2;
    char *output = (char *)malloc(byteArrayLength * 8 / 5 + 1); // Allocate enough space for base32
    int outputIndex = 0;
    int bits = 0;
    int value = 0;

    for (int i = 0; i < byteArrayLength; i++)
    {
        value = (value << 8) | bytes[i];
        bits += 8;

        while (bits >= 5)
        {
            output[outputIndex++] = alphabet[(value >> (bits - 5)) & 31];
            bits -= 5;
        }
    }

    if (bits > 0)
    {
        output[outputIndex++] = alphabet[(value << (5 - bits)) & 31];
    }

    output[outputIndex] = '\0'; // Null-terminate the string

    free(bytes);
    return output;
}

char *resolve_ens_content_hash(const char *name)
{
    char *hash = namehash(name);
    if (!hash)
    {
        fprintf(stderr, "Error computing name hash!\n");
        return NULL;
    }

    uint8_t *hash_bytes = NULL;
    eth_hex_to_bytes(&hash_bytes, hash, strlen(hash));

    struct eth_abi abi;
    char *fn = "contenthash(bytes32)", *hex;
    eth_abi_init(&abi, ETH_ABI_ENCODE);
    eth_abi_call(&abi, &fn, NULL);     // addr(
    eth_abi_bytes32(&abi, hash_bytes); //   0x6dd56164f699a101d6063add452dfed7c6c09fe17b8e4acf3328f9387f5030b9
    eth_abi_call_end(&abi);            // )
    size_t hexlen;
    eth_abi_to_hex(&abi, &hex, &hexlen);
    eth_abi_free(&abi);

    char hexWith0x[256];
    sprintf(hexWith0x, "0x%s", hex);

    const char *url = "https://rpc.payload.de";

    JsonBuilder *builder = json_builder_new();
    json_builder_begin_array(builder);
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "to");
    json_builder_add_string_value(builder, "0x231b0Ee14048e9dCcD1d247744d114a4EB5E8E63");
    json_builder_set_member_name(builder, "data");
    json_builder_add_string_value(builder, hexWith0x);
    json_builder_end_object(builder);
    json_builder_end_array(builder);
    JsonNode *params = json_builder_get_root(builder);

    // Call send_json_rpc_request
    char *response = send_json_rpc_request(url, "eth_call", params);

    JsonParser *parser = json_parser_new();
    json_parser_load_from_data(parser, response, -1, NULL);
    JsonObject *root = json_node_get_object(json_parser_get_root(parser));

    if (json_object_has_member(root, "result"))
    {
        JsonNode *result_node = json_object_get_member(root, "result");
        if (JSON_NODE_HOLDS_VALUE(result_node))
        {
            const gchar *result = json_node_get_string(result_node);

            // Remove the "0x" prefix and the leading 64-character prefix
            const gchar *sanitized_result = result + 130; // Skip "0x" + 64 characters

            // Create a modifiable copy of the sanitized result
            gchar *result_copy = g_strdup(sanitized_result);

            // Remove trailing zeros
            int len = strlen(result_copy);
            while (len > 0 && result_copy[len - 1] == '0')
            {
                len--;
            }
            result_copy[len] = '\0'; // Terminate string after last non-zero character

            return decode_content_hash(result_copy);
        }
    }
    else
    {
        printf("\nNo result found in the JSON response.\n");
        printf(response);

        return NULL;
    }
}