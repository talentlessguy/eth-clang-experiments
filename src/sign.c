#include <stdio.h>
#include <stdlib.h>
#include <ethc/account.h>
#include <ethc/hex.h>
#include <string.h>
#include <ethc/keccak256.h>
#include <sodium/crypto_hash_sha256.h>

char* sig_to_hex(uint8_t *r, uint8_t *s) {
    uint8_t sig[64];

    // Copy r into the first 32 bytes of sig
    memcpy(sig, r, 32);

    // Copy s into the next 32 bytes of sig
    memcpy(sig + 32, s, 32);

    // Convert the sig array to a hexadecimal string
    char *sig_hex = NULL;

    if (-1 == eth_hex_from_bytes(&sig_hex, sig, 64)) {
        fprintf(stderr, "Failed to convert raw sig to hex\n");
        return NULL;
    }

    return sig_hex;
}

void print_hex(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n"); // Newline for better readability
}

int main() {
    // Example hex string
    const char *hexstr = getenv("PK"); // 0x9ef...
    uint8_t *privkey;
    
    struct eth_account acc;

    // Call the function with the correct parameters
    eth_hex_to_bytes(&privkey, hexstr, strlen(hexstr));
    eth_account_from_privkey(&acc, privkey);

    char addr[40];
    eth_account_address_get(&addr, &acc);

    printf("Address: 0x%s", addr);

    struct eth_signed signature;
    char str[] = "Hello World"; // Example string
    uint8_t result = (uint8_t)atoi(str);

    // Compute keccak256 + prefix hash;
    uint8_t keccak[32];
    eth_keccak256p(keccak, &result, strlen(str));

    eth_account_sign(&signature, &acc, keccak, 32);

    printf("\n\nSignature: ");
    printf("\nr = 0x");
    print_hex(signature.r, 32);
    printf("s = 0x");
    print_hex(signature.s, 32);

    // char* sigHex = sig_to_hex(signature.r, signature.s);

    // if (sigHex != NULL) {
    //     printf("\n\nSignature: %s\n", sigHex);
    //     free(sigHex);
    // }
    
    return 0;
}