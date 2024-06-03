#include <stdio.h>
#include <string.h>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <stdlib.h>
#include <sodium/crypto_hash_sha256.h>

// Function to print a byte array in hex format
void print_hex(const unsigned char *data, size_t len) {
    printf("0x");
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

// Function to compute Ethereum address from public key
void pubkey_to_ethereum_address(const unsigned char *pubkey, size_t pubkey_len) {
    unsigned char hash[crypto_hash_sha256_BYTES];
    unsigned char address[20];

    // Hash the public key using SHA-256
    crypto_hash_sha256(hash, pubkey, pubkey_len);

    // Apply Keccak-256 hash to the result
    // Keccak-256 is essentially SHA3 with different padding
    // Implement Keccak-256 hash function here

    // Take the last 20 bytes of the Keccak-256 hash as the address
    memcpy(address, hash + 12, 20);

    // Print the Ethereum address without "0x" prefix
    print_hex(address, 20);
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
    printf("Signature: ");
    print_hex(signature, sizeof(signature));

    pubkey_to_ethereum_address(pubkey_serialized + 1, 64);

    // Cleanup secp256k1 context
    secp256k1_context_destroy(ctx);

    return 0;
}
