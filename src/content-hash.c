#include <stdio.h>
#include "./utils/content-hash.h"

int main()
{
    const char *name = "v1rtl.eth";
    printf("ENS name: %s\n", name);

    const char *ipfsHash = resolve_ens_content_hash(name);
    printf("IPFS CID: %s", ipfsHash);
    // Parse response JSON

    return 0;
}