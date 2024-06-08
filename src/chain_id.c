#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./utils/jsonrpc.h"

int main(void)
{
    const char *url = "https://rpc.payload.de";

    char *response = send_json_rpc_request(url, "eth_chainId", NULL);
    if (response)
    {
        printf("Response: %s\n", response);
        free(response);
    }

    return 0;
}
