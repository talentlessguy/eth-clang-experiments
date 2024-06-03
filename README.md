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

### ECSDA

Requirements: `libethc`

Loads an Ethereum account from env, signs a "Hello World" message and outputs an Ethereum address.

```
$ clang src/sign.c -lethc -L /usr/local/lib -rpath /usr/local/lib

Signature: 0xb81efa243127sda5...00
Ethereum Address: 0xce9667fa..43c6
```