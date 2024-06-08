# Clang experiments with Ethereum

## Setup

Install [Foundry](https://getfoundry.sh).

## Experiments

### JSONRPC request

Requirements: `libcurl`, `cjson`

Makes an HTTP request to an Anvil node to obtain chain ID.

```sh
clang src/jsonrpc.c -lcurl -lcjson
```

### Sign a message

Requirements: `libethc`

Loads an Ethereum account from env, signs a "Hello World" message and outputs an
Ethereum address.

```
$ LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib clang src/sign.c -lethc

Signature: 0xb81efa243127sda5...00
Ethereum Address: 0xce9667fa..43c6
```
