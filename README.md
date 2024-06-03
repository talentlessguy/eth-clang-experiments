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

Signs a "Hello World" message using an Ethereum and outputs an Ethereum address.

```
$ clang src/sign.c -lcurl -lsecp256k

Signature: b81efa2da5...10dfbf8b97d40027a5872400
Ethereum Address: ce9667fa..051b7f3b68643c6
```