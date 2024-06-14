#ifndef CONTENT_HASH_DECODER_H
#define CONTENT_HASH_DECODER_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <ethc/hex.h>
#include <ethc/abi.h>

#include "./jsonrpc.h"
#include "./ens.h"

char *decode_content_hash(const char *contentHash);
char *resolve_ens_content_hash(const char *name);

#endif /* CONTENT_HASH_DECODER_H */
