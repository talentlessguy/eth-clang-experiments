# Clang experiments with Ethereum

## Setup

GCC/clang 17+ is recommended.

## Experiments

### JSONRPC request

Requirements: `libcurl`, `json-glib`, `glib`

Makes an HTTP request to an Ethereum node to obtain chain ID.

```sh
gcc src/chain_id.c src/utils/jsonrpc.c -I/usr/include/json-glib-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -ljson-glib-1.0 -lgobject-2.0 -lglib-2.0 -lcurl
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

Requirements: `libethc`, `libcurl`, `json-glib`, `glib`

Resolves given ENS name to an Ethereum address.

```
$ LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib gcc src/ens-addr.c src/utils/*.c -I/usr/include/json-glib-1.0 -I/usr/include/glib-2
.0 -I/usr/lib/glib-2.0/include -ljson-glib-1.0 -lgobject-2.0 -lglib-2.0 -lcurl -lethc

ENS name: v1rtl.eth
namehash: 0x6dd56164f699a101d6063add452dfed7c6c09fe17b8e4acf3328f9387f5030b9
Encoded ABI: 0x3b3b57de6dd56164f699a101d6063add452dfed7c6c09fe17b8e4acf3328f9387f5030b9
Response: {"jsonrpc":"2.0","id":1,"result":"0x000000000000000000000000d3b282e9880cdcb1142830731cd83f7ac0e1043f"}

Address: d3b282e9880cdcb1142830731cd83f7ac0e1043f
```
