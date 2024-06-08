# Clang experiments with Ethereum

## Setup

Install [Foundry](https://getfoundry.sh). Clang frontend is recommended,
version 17.

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

### Resolve ENS

Requirements: `libethc`

Resolves given ENS name to an Ethereum address.

```
$ LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib clang src/ens.c -lethc

ENS name: v1rtl.eth
Ethereum Address: 0xce9667fa..43c6
```
