#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ethc/keccak256.h>
#include <ethc/hex.h>
#include <ethc/abi.h>
#include "./utils/jsonrpc.h"
#include "./utils/ens.h"

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
    printf("Encoded ABI: 0x%s\n", hex); // 0x3b3b57de6dd56164f699a101d6063add452dfed7c6c09fe17b8e4acf3328f9387f5030b9

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

    // Parse response JSON
    JsonParser *parser = json_parser_new();
    json_parser_load_from_data(parser, response, -1, NULL);
    JsonObject *root = json_node_get_object(json_parser_get_root(parser));

    if (json_object_has_member(root, "result"))
    {
        JsonNode *result_node = json_object_get_member(root, "result");
        if (JSON_NODE_HOLDS_VALUE(result_node))
        {
            const gchar *result = json_node_get_string(result_node);
            if (strncmp(result, "0x", 2) == 0)
            {
                result += 2; // Move the pointer past "0x"
                while (*result == '0')
                {
                    result++; // Skip leading zeros
                }
            }
            printf("Address: 0x%s\n", result);
        }
    }
    else
    {
        printf("\nNo result found in the JSON response.\n");
        printf(response);
    }

    return 0;
}