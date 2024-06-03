#include <stdio.h>
#include <string.h>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <stdlib.h>
#include <sodium.h>
#include "keccak256.h"

void print_hex(const unsigned char *data, size_t len) {
    printf("0x");
    for (size_t i = 0; i < len; ++i) {
        printf("%02x", data[i]);
    }
}

void keccak256(const uint8_t *data, uint16_t length, uint8_t *result) {
    SHA3_CTX context;
    keccak_init(&context);
    keccak_update(&context, (const unsigned char*)data, (size_t)length);
    keccak_final(&context, (unsigned char*)result);

    // Clear out the contents of what we hashed (in case it was secret)
    memset((char*)&context, 0, sizeof(SHA3_CTX));
}

void pubkey_to_eth_addr(const uint8_t *pubkey, uint16_t pubkey_len, char *address) {
    uint8_t hash[32];
    
    // Hash the public key using keccak256
    keccak256(pubkey, pubkey_len, hash);
    
    // Convert the hash to a hexadecimal string
    char hex_hash[65]; // 32 bytes * 2 characters per byte + 1 for null terminator
    for (int i = 0; i < 32; i++) {
        sprintf(hex_hash + i * 2, "%02x", hash[i]);
    }
    
    // Take the substring starting from the 27th character
    memcpy(address, hex_hash + 26, 40); // 40 characters for the Ethereum address
    address[40] = '\0'; // Null-terminate the string
}



int main() {
    // Set the private key in hexadecimal format
    char *pk_env = getenv("PK");
    if (pk_env == NULL) {
        fprintf(stderr, "Error: PK environment variable not set.\n");
        return 1;
    }

    // Adjust if the private key starts with "0x" prefix
    size_t len = strlen(pk_env);
    int offset = (len >= 66 && strncmp(pk_env, "0x", 2) == 0) ? 2 : 0;

    // Check if the private key has a valid length
    if (len - offset != 64) {
        fprintf(stderr, "Error: Invalid private key format.\n");
        return 1;
    }

    // Convert the environment variable to a binary private key
    unsigned char privkey[32];
    for (int i = 0; i < 32; i++) {
        sscanf(pk_env + offset + 2 * i, "%2hhx", &privkey[i]);
    }

    // Initialize secp256k1 context
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

    // Derive public key from private key
    secp256k1_pubkey pubkey;
    if (!secp256k1_ec_pubkey_create(ctx, &pubkey, privkey)) {
        fprintf(stderr, "Error: Failed to derive public key.\n");
        return 1;
    }

    // Serialize the public key
    unsigned char pubkey_serialized[65];
    size_t pubkey_serialized_len = sizeof(pubkey_serialized);
    if (!secp256k1_ec_pubkey_serialize(ctx, pubkey_serialized, &pubkey_serialized_len, &pubkey, SECP256K1_EC_UNCOMPRESSED)) {
        fprintf(stderr, "Error: Failed to serialize public key.\n");
        return 1;
    }

    printf("\nPubkey: ");
    print_hex(pubkey_serialized, pubkey_serialized_len);

    // Sign a message
    const char *msg = "Hello World";
    unsigned char signature[65];
    secp256k1_ecdsa_signature sig;
    if (!secp256k1_ecdsa_sign(ctx, &sig, (const unsigned char *)msg, privkey, NULL, NULL)) {
        fprintf(stderr, "Error: Failed to sign message.\n");
        return 1;
    }

    // Serialize the signature
    if (!secp256k1_ecdsa_signature_serialize_compact(ctx, signature, &sig)) {
        fprintf(stderr, "Error: Failed to serialize signature.\n");
        return 1;
    }

    // Print the signature
    printf("\nSignature: ");
    print_hex(signature, sizeof(signature));

    char address[42]; // Ethereum address is 42 characters including '0x' prefix
    pubkey_to_eth_addr(pubkey_serialized, 65, address);

    
    printf("\nAddress: ");
    print_hex((const unsigned char *)address, 40);

    // Cleanup secp256k1 context
    secp256k1_context_destroy(ctx);

    return 0;
}
