#include <stdio.h>
#include <stdlib.h>
#include <ethc/account.h>
#include <ethc/ecdsa.h>
#include <ethc/hex.h>
#include <string.h>

char *sig_to_hex(struct eth_ecdsa_signature sig_obj)
{

    // r + s + v
    uint8_t sig[65];

    // Copy r into the first 32 bytes of sig
    memcpy(sig, sig_obj.r, 32);

    // Copy s into the next 32 bytes of sig
    memcpy(sig + 32, sig_obj.s, 32);

    // Convert the sig array to a hexadecimal string
    char *sig_hex = NULL;

    int s = 27;
    int s_1 = 28;
    if (sig_obj.recid % 2 == 0)
    {
        memcpy(sig + 64, &s, 1);
    }
    else
    {
        memcpy(sig + 64, &s_1, 1);
    }

    if (-1 == eth_hex_from_bytes(&sig_hex, sig, 65))
    {
        fprintf(stderr, "Failed to convert raw sig to hex\n");
        return NULL;
    }

    return sig_hex;
}

int main()
{
    const char *hexstr = getenv("PK"); // 0x9ef...
    uint8_t *privkey;

    struct eth_account acc;

    eth_hex_to_bytes(&privkey, hexstr, strlen(hexstr));
    eth_account_from_privkey(&acc, privkey);

    char addr[40];
    eth_account_address_get(addr, &acc);

    printf("Address: 0x%s", addr);

    struct eth_signed signature;
    char str[] = "Hello World";
    uint8_t result = (uint8_t)atoi(str);

    eth_account_sign(&signature, &acc, str, strlen(str));

    printf("\n\nSignature: ");
    printf("\nr = 0x");
    char *r;
    eth_hex_from_bytes(&r, signature.r, 32);
    printf("%s", r);
    printf("\ns = 0x");
    char *s;
    eth_hex_from_bytes(&s, signature.s, 32);
    printf("%s", s);

    char *sigHex = sig_to_hex(signature);
    if (sigHex != NULL)
    {
        printf("\n\nSignature: 0x%s\n", sigHex);
        free(sigHex);
    }

    return 0;
}